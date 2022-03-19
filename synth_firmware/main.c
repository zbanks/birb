#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIDI_CHANNEL 0
#define PRESCALER 4
#define OCTAVE_OFFSET -2

// This lkup uses 16 counts per period
const uint16_t PERIOD_LKUP[] = {
38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355, 335, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168, 158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33, 31, 30, 28, 26, };

static void note_on(uint8_t k, uint8_t v);
static void note_off(uint8_t k);

#define N_NOTES 5
#define N_OSC 3
#define N_SELECTOR_POSITIONS 12

#define NO_NOTE 255
#define NO_OSC 255

// Notes currently held down, and how they are being voiced
static uint8_t note_index[N_NOTES];
static uint8_t note_velocity[N_NOTES];
static uint8_t note_osc[N_NOTES];

// Computed from the note_* arrays:
static uint8_t osc_note_index[N_OSC];
static uint8_t osc_note_velocity[N_OSC];


enum mode {
    MODE_MONO,
    MODE_POLY,
    MODE_OCTAVE,
};

static uint8_t mod = 0;

static int8_t sadd(int8_t x, int8_t y) {
    int16_t result = x + y;
    if (result > 127) {
        return 127;
    } else if (result < -128) {
        return -128;
    } else {
        return (int8_t)result;
    }
}

struct adsr_config {
    int16_t initial_value;
    int16_t a_incr;
    int16_t a_value;
    int16_t d_incr;
    int16_t s_value;
    int16_t r_incr;
    int16_t final_value;
};

struct adsr_state {
    enum {
        ADSR_OFF,
        ADSR_ATTACK,
        ADSR_DECAY,
        ADSR_SUSTAIN,
        ADSR_RELEASE,
    } mode;
    int16_t output;
    uint8_t last_gate;
};

static void adsr_configure(struct adsr_config *config, int16_t initial_value, int16_t a_time, int16_t a_value, int16_t d_time, int16_t s_value, int16_t r_time, int16_t final_value) {
    config->initial_value = initial_value;
    if (a_time <= 0) {
        a_time = 1;
    }
    config->a_incr = (a_value - initial_value) / a_time;
    if (config->a_incr == 0) {
        config->a_incr = a_value > initial_value ? 1 : -1;
    }
    config->a_value = a_value;
    if (d_time <= 0) {
        d_time = 1;
    }
    config->d_incr = (s_value - a_value) / d_time;
    if (config->d_incr == 0) {
        config->d_incr = s_value > a_value ? 1 : -1;
    }
    config->s_value = s_value;
    if (r_time <= 0) {
        r_time = 1;
    }
    config->r_incr = (final_value - s_value) / r_time;
    if (config->r_incr == 0) {
        config->r_incr = final_value > s_value ? 1 : -1;
    }
    config->final_value = final_value;
}

static void adsr_init(struct adsr_state *state, const struct adsr_config *config) {
    state->mode = ADSR_OFF;
    state->output = config->final_value;
    state->last_gate = 0;
}

static int16_t adsr_update(struct adsr_state *state, const struct adsr_config *config, uint8_t gate) {
    if ((state->mode == ADSR_OFF || state->mode == ADSR_RELEASE || gate != state->last_gate) && gate) {
        state->output = config->initial_value;
        state->mode = ADSR_ATTACK;
    } else if (!gate) {
        state->mode = ADSR_RELEASE;
    }
    state->last_gate = gate;
    switch (state->mode) {
        case ADSR_OFF:
        case ADSR_SUSTAIN:
            break;
        case ADSR_ATTACK:
            if ((config->a_incr > 0 && state->output >= config->a_value - config->a_incr)
             || (config->a_incr < 0 && state->output <= config->a_value - config->a_incr)) {
                state->output = config->a_value;
                state->mode = ADSR_DECAY;
            } else {
                state->output += config->a_incr;
            }
            break;
        case ADSR_DECAY:
            if ((config->d_incr > 0 && state->output >= config->s_value - config->d_incr)
             || (config->d_incr < 0 && state->output <= config->s_value - config->d_incr)) {
                state->output = config->s_value;
                state->mode = ADSR_SUSTAIN;
            } else {
                state->output += config->d_incr;
            }
            break;
        case ADSR_RELEASE:
            if ((config->r_incr > 0 && state->output >= config->final_value - config->r_incr)
             || (config->r_incr < 0 && state->output <= config->final_value - config->r_incr)) {
                state->output = config->final_value;
                state->mode = ADSR_OFF;
            } else {
                state->output += config->r_incr;
            }
            break;
    }
    return state->output;
}

struct triangle_config {
    int16_t low;
    int16_t high;
    int16_t initial_value;
    int16_t increment;
};

struct triangle_state {
    uint8_t up;
    int16_t output;
    uint8_t last_gate;
};

static void triangle_configure(struct triangle_config *config, int16_t low, int16_t high, int16_t initial_value, int16_t period) {
    config->low = low;
    config->high = high;
    config->initial_value = initial_value;
    if (period == 0) {
        period = 1;
    }
    config->increment = ((uint16_t)high - (uint16_t)low) / period;
    if (config->increment == 0) {
        config->increment = 1;
    }
}

static void triangle_init(struct triangle_state *state, const struct triangle_config *config) {
    state->output = config->low;
    state->up = 1;
    state->last_gate = 0;
}

static int16_t triangle_update(struct triangle_state *state, const struct triangle_config *config, uint8_t gate) {
    if (gate && gate != state->last_gate) {
        state->output = config->initial_value;
        state->up = 1;
    } else {
        if (state->up) {
            if (state->output > config->high - config->increment) {
                state->output = config->high;
                state->up = 0;
            } else {
                state->output += config->increment;
            }
        } else {
            if (state->output < config->low + config->increment) {
                state->output = config->low;
                state->up = 1;
            } else {
                state->output -= config->increment;
            }
        }
    }
    state->last_gate = gate;
    return state->output;
}

static uint8_t rand8() {
    static uint8_t x;
    static uint8_t a;
    static uint8_t b;
    static uint8_t c;

    x++;               //x is incremented every round and is not affected by any other variable
    a = a^c^x;       //note the mix of addition and XOR
    b = b+a;         //And the use of very few instructions
    c = (c+(b>>1))^a;  //the right shift is to ensure that high-order bits from b can affect  
    return c;          //low order bits of other variables
}

struct arp_config {
    const int8_t *notes;
    uint8_t n_notes;
    int16_t period;
};

struct arp_state {
    uint8_t index;
    int16_t timer;
    uint8_t last_gate;
    int8_t note;
};

static void arp_configure(struct arp_config *config, const int8_t notes[], uint8_t n_notes, int16_t period) {
    config->notes = notes;
    config->n_notes = n_notes;
    config->period = period;
}

static void arp_init(struct arp_state *state, const struct arp_config *config) {
    state->last_gate = 0;
    state->timer = 0;
    state->index = 0;
    if (config->n_notes > 0) {
        state->note = config->notes[0];
    } else {
        state->note = 0;
    }
}

static int8_t arp_update(struct arp_state *state, const struct arp_config *config, uint8_t gate) {
    if (config->n_notes == 0) {
        // No arpeggiation is configured
        state->timer = 0;
        state->index = 0;
        state->note = 0;
    } else if (gate && gate != state->last_gate) {
        // Rising edge on gate
        state->timer = 0;
        state->index = 0;
        state->note = config->notes[0];
    } else {
        state->timer++;
        if (state->timer >= config->period) {
            // Advance arp
            state->timer = 0;
            state->index++;
            if (state->index >= config->n_notes) {
                state->index = 0;
            }
            state->note = config->notes[state->index];
        }
    }
    state->last_gate = gate;
    return state->note;
}

static void rx(uint8_t byte) {
    static enum {
        UNKNOWN,
        WAIT_NOTE_ON_KEY,
        WAIT_NOTE_ON_VELOCITY,
        WAIT_NOTE_OFF_KEY,
        WAIT_NOTE_OFF_VELOCITY,
        WAIT_CC_CONTROL,
        WAIT_MOD_VALUE,
    } state = UNKNOWN;

    static uint8_t k;
    static uint8_t v;

    if (byte == 0xFF) {
        // "Reset" message: Issue system reset
        _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
        for(;;);
    } else if ((byte & 0xF8) == 0xF8) {
        return; // Ignore real-time messages
    }

    if (byte & 0x80) {
        // Status byte
        if (byte == (0x80 | MIDI_CHANNEL)) {
            state = WAIT_NOTE_OFF_KEY;
        } else if (byte == (0x90 | MIDI_CHANNEL)) {
            state = WAIT_NOTE_ON_KEY;
        } else if (byte == (0xB0 | MIDI_CHANNEL)) {
            state = WAIT_CC_CONTROL;
        } else {
            state = UNKNOWN;
        }
    } else {
        // Handle notes, etc
        switch (state) {
            case UNKNOWN:
                break;
            case WAIT_NOTE_ON_KEY:
                k = byte;
                state = WAIT_NOTE_ON_VELOCITY;
                break;
            case WAIT_NOTE_ON_VELOCITY:
                v = byte;
                note_on(k, v);
                state = WAIT_NOTE_ON_KEY;
                break;
            case WAIT_NOTE_OFF_KEY:
                k = byte;
                state = WAIT_NOTE_OFF_VELOCITY;
                break;
            case WAIT_NOTE_OFF_VELOCITY:
                v = byte;
                note_off(k);
                state = WAIT_NOTE_OFF_KEY;
                break;
            case WAIT_CC_CONTROL:
                if (byte == 0x01) { // mod wheel
                    state = WAIT_MOD_VALUE;
                } else {
                    state = UNKNOWN;
                }
                break;
            case WAIT_MOD_VALUE:
                mod = byte;
                state = WAIT_CC_CONTROL;
                break;
        }
    }
}

ISR (USART0_RXC_vect) 
{
    rx(USART0.RXDATAL);
}

static void update_modulation();

struct osc_state {
    // Low-level timer stuff
    uint8_t prescaler;
    uint16_t timer_period_high;
    uint16_t timer_period_low;
    uint8_t t;

    uint8_t triggered_note;
    uint8_t triggered_velocity;

    struct adsr_config amp_env_config;
    struct triangle_config amp_lfo_config;
    struct adsr_config pitch_env_config;
    struct triangle_config pitch_lfo_config;
    struct triangle_config pwm_lfo_config;

    struct adsr_state amp_env;
    struct triangle_state amp_lfo;
    struct adsr_state pitch_env;
    struct triangle_state pitch_lfo;
    struct triangle_state pwm_lfo;

    // Output stuff
    int8_t amplitude;
};

#define MIN_PER 300

static struct global_config {
    enum mode mode;
    struct arp_config arp_config;
} global_config;

static struct global_state {
    struct osc_state osc[N_OSC];
    int8_t mixer_inputs[N_OSC];
    struct arp_state arp;
} global;

static bool osc_handle_timer(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // Returns the output signal level
    // and updates next_period with the time until this function should be called next

    if (state->prescaler > PRESCALER) {
        state->prescaler = 0;
    } else {
        state->prescaler++;
        return false;
    }

    *next_period = (state->t & 1) ? state->timer_period_high : state->timer_period_low;
    *out = (state->t & 1) ? state->amplitude : -state->amplitude;
    state->t++;
    return true;
}

static uint16_t period(int16_t note) {
    int16_t adj_note = note + (((OCTAVE_OFFSET) * 12) << 8);
    // Set osc as per new note
    if (adj_note < 0) { // Highest note to avoid deadlock
        adj_note = 0;
    }
    if (adj_note > (126 << 8)) { // Highest note to avoid deadlock
        adj_note = (126 << 8);
    }
    uint8_t ix = adj_note >> 8;
    uint16_t per_low = PERIOD_LKUP[ix];
    uint16_t per_high = PERIOD_LKUP[ix + 1];
    uint8_t frac = adj_note & 0xFF;
    uint16_t per = ((uint32_t)per_low * (256 - frac) + (uint32_t)per_high * frac) >> 8;
    return per;
}

static void global_osc_init() {
    for (int i = 0; i < N_OSC; i++) {
        global.osc[i].prescaler = 0;
        global.osc[i].timer_period_high = 1250;
        global.osc[i].timer_period_low = 1250;
        global.osc[i].t = 0;
        global.osc[i].amplitude = 0;
        adsr_init(&global.osc[i].amp_env, &global.osc[i].amp_env_config);
        triangle_init(&global.osc[i].amp_lfo, &global.osc[i].amp_lfo_config);
        adsr_init(&global.osc[i].pitch_env, &global.osc[i].pitch_env_config);
        triangle_init(&global.osc[i].pitch_lfo, &global.osc[i].pitch_lfo_config);
        triangle_init(&global.osc[i].pwm_lfo, &global.osc[i].pwm_lfo_config);
    }
}

static void note_off_mono(uint8_t note_index_to_remove) {
    note_velocity[note_index_to_remove] = 0;

    // See if there's another note playing
    for (int i = 0; i < N_NOTES; i++) {
        if (note_velocity[i] > 0) {
            osc_note_index[0] = note_index[i];
            osc_note_velocity[0] = note_velocity[i];
            return;
        }
    }

    // No notes left playing
    osc_note_velocity[0] = 0;
}

static void note_on_mono(uint8_t k, uint8_t v) {
    note_index[0] = k;
    note_velocity[0] = v;

    osc_note_index[0] = k;
    osc_note_velocity[0] = v;
}

static void note_off_poly(uint8_t note_index_to_remove) {
    uint8_t osc_index = note_osc[note_index_to_remove];
    if (osc_index != NO_OSC) {
        osc_note_velocity[osc_index] = 0;
    }
}

static void assign_osc_poly() {
    // Finds a free oscillator to use and return its index.

    uint8_t osc_index = 0;

    // First, look a completely quiet oscillators
    for (int i = 0; i < N_OSC; i++) {
        if (osc_note_velocity[i] == 0 && global.osc[i].amplitude == 0) {
            osc_index = i;
            goto found;
        }
    }

    // Failing that, look for a releasing oscillator
    for (int i = 0; i < N_OSC; i++) {
        if (osc_note_velocity[i] == 0) {
            osc_index = i;
            goto found;
        }
    }

    // Failing that, steal the oldest note's oscillator
    for (int i = N_NOTES - 1; i >= 0; i--) {
        if (note_osc[i] != NO_OSC) {
            osc_index = note_osc[i];
            goto found;
        }
    }

    found:

    // Perform the oscillator (re)assignment
    note_osc[0] = osc_index;
    for (int i = 1; i < N_NOTES; i++) {
        if (note_osc[i] == osc_index) {
            note_velocity[i] = 0;
            note_osc[i] = NO_OSC; // Stolen!
            break;
        }
    }
}

static void note_on_poly(uint8_t k, uint8_t v) {
    if (note_index[0] != k || note_osc[0] == NO_OSC) {
        // Find a suitable oscillator to use for this new note
        note_index[0] = k;
        note_velocity[0] = v;
        assign_osc_poly();
    } else {
        // Re-use the same oscillator if it's the same note
        // (i.e. don't allow oscillators in unison)
        note_velocity[0] = v;
    }

    uint8_t osc_index = note_osc[0];
    osc_note_index[osc_index] = k;
    osc_note_velocity[osc_index] = v;
}

static void note_off_octave(uint8_t note_index_to_remove) {
    note_velocity[note_index_to_remove] = 0;

    // See if there's another note playing
    for (int i = 0; i < N_NOTES; i++) {
        if (note_velocity[i] > 0) {
            osc_note_index[0] = note_index[i];
            osc_note_velocity[0] = note_velocity[i];
            osc_note_index[1] = note_index[i] - 12;
            osc_note_velocity[1] = note_velocity[i];
            return;
        }
    }

    // No notes left playing
    osc_note_velocity[0] = 0;
    osc_note_velocity[1] = 0;
}

static void note_on_octave(uint8_t k, uint8_t v) {
    note_index[0] = k;
    note_velocity[0] = v;

    osc_note_index[0] = k;
    osc_note_velocity[0] = v;
    osc_note_index[1] = k - 12;
    osc_note_velocity[1] = v;
}

static void all_notes_off() {
    for (int i = 0; i < N_NOTES; i++) {
        note_velocity[i] = 0;
        note_osc[i] = NO_OSC;
    }
    for (int i = 0; i < N_OSC; i++) {
        osc_note_velocity[i] = 0;
    }
}

static void note_on(uint8_t k, uint8_t v) {
    if (v == 0) {
        note_off(k);
        return;
    }

    // Figure out which note entry to replace with the new note.
    // If the note is already in our list, use that entry.
    // If not, default to the oldest slot.
    uint8_t replace_index = N_NOTES - 1;
    for (int i = 0; i < N_NOTES - 1; i++) {
        if (note_index[i] == k) {
            replace_index = i;
            break;
        }
    }

    uint8_t replaced_k = note_index[replace_index];
    uint8_t replaced_v = note_velocity[replace_index];
    uint8_t replaced_osc = note_osc[replace_index];

    // Add new note to beginning and shift everything right by one
    for (int i = replace_index; i >= 1; i--) {
        note_index[i] = note_index[i - 1];
        note_velocity[i] = note_velocity[i - 1];
        note_osc[i] = note_osc[i - 1];
    }

    note_index[0] = replaced_k;
    note_velocity[0] = replaced_v;
    note_osc[0] = replaced_osc;

    // Mode-specific behavior
    switch (global_config.mode) {
        case MODE_MONO:
            note_on_mono(k, v);
            break;
        case MODE_POLY:
            note_on_poly(k, v);
            break;
        case MODE_OCTAVE:
            note_on_octave(k, v);
            break;
    }
}

static void note_off(uint8_t k) {
    uint8_t note_index_to_remove = NO_NOTE;
    for (int i = 0; i < N_NOTES; i++) {
        if (note_index[i] == k && note_velocity[i] > 0) {
            note_index_to_remove = i;
            break;
        }
    }

    // If no note was sounding, do nothing
    if (note_index_to_remove == NO_NOTE) {
        return;
    }

    // Mode-specific behavior
    switch (global_config.mode) {
        case MODE_MONO:
            note_off_mono(note_index_to_remove);
            break;
        case MODE_POLY:
            note_off_poly(note_index_to_remove);
            break;
        case MODE_OCTAVE:
            note_off_octave(note_index_to_remove);
            break;
    }

    // Remove from list by shifting left
    for (int i = note_index_to_remove; i < N_NOTES - 1; i++) {
        note_index[i] = note_index[i + 1];
        note_velocity[i] = note_velocity[i + 1];
        note_osc[i] = note_osc[i + 1];
    }
    // Clear out top entry in list
    note_velocity[N_NOTES - 1] = 0;
    note_osc[N_NOTES - 1] = NO_OSC;
}


int
main (void)
{
    // Set up external clock input
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA |= CLKCTRL_CLKSEL_EXTCLK_gc;
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    // Set up TCA0 as first voice oscillator
    TCA0.SINGLE.PER = 1250; // 8 KHz update rate
    TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

    // Set up TCB0 as second voice oscillator
    TCB0.CCMP = 1250; // Just to get the audio loop going
    TCB0.INTCTRL |= TCB_CAPT_bm;
    TCB0.CTRLA |= TCB_ENABLE_bm;

    // Set up TCB1 as third voice oscillator
    TCB1.CCMP = 1250; // Just to get the audio loop going
    TCB1.INTCTRL |= TCB_CAPT_bm;
    TCB1.CTRLA |= TCB_ENABLE_bm;

    // Set up modulation timer
    TCD0.CMPBCLR = 1250; // 8 KHz update rate
    TCD0.CTRLA |= TCD_ENABLE_bm;

    PORTB.DIRSET = 1 << 0; // Set PB0 as output

    VREF_CTRLA |= VREF_DAC0REFSEL_2V5_gc; // VREF = 2.5V for DAC
    VREF_CTRLB |= VREF_DAC0REFEN_bm; // Force enable VREF

    // Disable everything else on PA6
    PORTA.PIN6CTRL &= ~PORT_ISC_gm;
    PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm;

    // Enable DAC output
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm | DAC_RUNSTDBY_bm;

    // ADC
    ADC0.CTRLA |= ADC_RESSEL_8BIT_gc; // 8 bit resolution
    ADC0.CTRLB |= ADC_PRESC_DIV8_gc; // 1.25MHz ADC clock rate
    ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc; // Use full VDD for reference

    // Disable everything else on PA1
    PORTA.PIN1CTRL &= ~PORT_ISC_gm;
    PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN1CTRL &= ~PORT_PULLUPEN_bm;

    // Disable everything else on PA2
    PORTA.PIN2CTRL &= ~PORT_ISC_gm;
    PORTA.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN2CTRL &= ~PORT_PULLUPEN_bm;

    // Disable everything else on PA4
    PORTA.PIN4CTRL &= ~PORT_ISC_gm;
    PORTA.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN4CTRL &= ~PORT_PULLUPEN_bm;

    ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC

    // Enable UART TX and RX
    // Set PB2 as output
	PORTB.DIRSET = 1 << 2;

    // Invert PB3 input because this transceiver is weird
    PORTB.PIN3CTRL |= PORT_INVEN_bm;

    // Set PB3 pullup
    PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;

    USART0.BAUD = 1280; // 31250 baud @ 10 MHz clock
    USART0.CTRLA |= USART_RXCIE_bm; // Receive data interrupt enable
	USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;	// Enable rx and tx

    sei();

    for(;;) {
        asm volatile("": : :"memory");
        if (TCD0.INTFLAGS & TCD_OVF_bm) {
            TCD0.INTFLAGS |= TCD_OVF_bm;
            update_modulation();
        }
    }
}

static struct knobs {
    uint8_t select;
    uint8_t depth;
    uint8_t freq;
} knobs;

static void init_basic() {
    global_config.mode = MODE_POLY;

    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // No PWM
        triangle_configure(&osc->pwm_lfo_config, 0, 0, 0, 1);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 100, 0);
        osc->amp_env_config.s_value = 0;
        triangle_configure(&osc->amp_lfo_config, 0, 0, 0, 1);

        // Add some vibrato
        const int16_t et = 6000;
        const int16_t ev = 6000;
        adsr_configure(&osc->pitch_env_config, 0, et, ev, 1, ev, 1, 0);
        const int16_t ot = 400;
        const int16_t ov = 200;
        triangle_configure(&osc->pitch_lfo_config, -ov, ov, 0, ot);
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_basic() {
    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Decay based on depth knob
        osc->amp_env_config.d_incr = -(knobs.depth < 127 ? (144 - knobs.depth) : (255 - knobs.depth) >> 3);
        osc->amp_env_config.s_value = knobs.depth < 255 ? 0 : 64 << 8;

        // Vibrato speed and amount based on freq knob
        int16_t vibrato_amt = knobs.freq << 4;
        osc->pitch_lfo_config.low = -vibrato_amt;
        osc->pitch_lfo_config.high = vibrato_amt;
        osc->pitch_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 11;
    }
}

static void init_chorus() {
    global_config.mode = MODE_POLY;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // No PWM
        triangle_configure(&osc->pwm_lfo_config, 0, 0, 0, 1);

        // Basic amplitude envelope
        adsr_configure(&osc->amp_env_config, 0, 1, 127 << 8, 1000, 63 << 8, 100, 0);

        // Amplitude LFO set up in mod function
        triangle_configure(&osc->amp_lfo_config, 0, 0, 0, 1);

        // No pitch LFO
        adsr_configure(&osc->pitch_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure(&osc->pitch_lfo_config, 0, 0, 0, 1);
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_chorus() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // PWM based on depth knob
        int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
        osc->pwm_lfo_config.low = -pwm_amt;
        osc->pwm_lfo_config.high = pwm_amt;
        osc->pwm_lfo_config.increment = knobs.depth >> 1;

        osc->amp_env_config.a_value = (64 + (knobs.depth >> 2)) << 8;

        // Tremolo speed and amount based on freq knob
        int16_t tremolo_amt = (knobs.freq < 127 ? knobs.freq : 127) << 7;
        osc->amp_lfo_config.low = -tremolo_amt;
        osc->amp_lfo_config.high = tremolo_amt;
        osc->amp_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 8;
    }
}

static void init_wave() {
    global_config.mode = MODE_POLY;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Constant PWM
        //triangle_configure(&osc->pwm_lfo_config, 0, -96 << 8, 96 << 8, 10000);
        triangle_configure(&osc->pwm_lfo_config, -120 << 8, 120 << 8, -120 << 8, 800);

        // Basic amplitude envelope
        adsr_configure(&osc->amp_env_config, 0, 1, 127 << 8, 1000, 63 << 8, 100, 0);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 100, 64 << 8, 1, 64 << 8, 100, 0);
        triangle_configure(&osc->amp_lfo_config, 0, 0, 0, 1);

        // No pitch LFO
        adsr_configure(&osc->pitch_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure(&osc->pitch_lfo_config, 0, 0, 0, 1);
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_wave() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Release based on depth knob
        //osc->amp_env_config.a_incr = (knobs.depth < 127 ? knobs.depth : 127) << 2;
        uint16_t inv_knob = 255 - knobs.freq;
        osc->amp_env_config.r_incr = -(((inv_knob * inv_knob) >> 11) + 1);

        osc->pwm_lfo_config.high = (((int16_t)-120 << 7) + (int16_t)knobs.depth * (240 >> 1)) << 1;
        osc->pwm_lfo_config.initial_value = (osc->pwm_lfo_config.low + osc->pwm_lfo_config.high) >> 1;
        //osc->pwm_lfo_config.increment = ((uint16_t)knobs.depth * knobs.depth) >> 11;
        osc->pwm_lfo_config.increment = knobs.depth >> 3;
    }
}

static void init_bass() {
    global_config.mode = MODE_OCTAVE;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // PWM on upper octave
        triangle_configure(&osc->pwm_lfo_config, 0, 0, 0, 1);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 300, 0);
        triangle_configure(&osc->amp_lfo_config, 0, 0, 0, 1);

        // No pitch LFO
        adsr_configure(&osc->pitch_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure(&osc->pitch_lfo_config, 0, 0, 0, 1);
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        if (i == 0) {
            // PWM based on depth knob
            int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
            osc->pwm_lfo_config.low = -pwm_amt;
            osc->pwm_lfo_config.high = pwm_amt;
            osc->pwm_lfo_config.increment = knobs.depth >> 1;
        }

        // Vibrato speed and amount based on freq knob
        int16_t vibrato_amt = knobs.freq << 4;
        osc->pitch_lfo_config.low = -vibrato_amt;
        osc->pitch_lfo_config.high = vibrato_amt;
        osc->pitch_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 11;

        if (i == 1) {
            // Bring in lower octave with depth knob
            uint16_t vol = knobs.depth < 64 ? knobs.depth : 64;
            osc->amp_env_config.a_value = vol << 8;
            osc->amp_env_config.s_value = vol << 8;

            // Tremolo on lower octave
            int16_t tremolo_amt = (knobs.freq < 127 ? knobs.freq : 127) << 7;
            osc->amp_lfo_config.low = -tremolo_amt;
            osc->amp_lfo_config.high = tremolo_amt;
            osc->amp_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 8;
        }
    }
}

static void init_empty() {
    global_config.mode = MODE_POLY;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        triangle_configure(&osc->pwm_lfo_config, 0, 0, 0, 1);

        adsr_configure(&osc->amp_env_config, 0, 1, 0, 1, 0, 1, 0);
        osc->amp_env_config.s_value = 0;
        triangle_configure(&osc->amp_lfo_config, 0, 0, 0, 1);

        adsr_configure(&osc->pitch_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure(&osc->pitch_lfo_config, 0, 0, 0, 1);
    }

    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_empty() {
}
/*

const int8_t arp_octave_notes[] = {-12, 0};
    //adsr_init(&amp_env, 0, 1, 127, 20, 100, 32, 0);
    //adsr_init(&amp_env, 1, 255, 127, 255);
    //triangle_init(&amp_lfo, -100, 100, 30);

    //adsr_init(&pitch_env, 0, 1, 0, 1, 0, 100, -127);
    //triangle_init(&pitch_lfo, -60, 60, 30);
    //adsr_init(&pitch_lfo_env, 0, 100, 100, 1, 100, 1, 100);

    //adsr_init(&mod_env, 127, 100, 64 + 32, 1, 64 + 32, 100, 127);
    //triangle_init(&mod_lfo, 20, 100, 100);
    //mod_lfo.increment = 1;
    //triangle_init(&mod_lfo, 0, 0, 10); // XXX

    // Detune
    triangle_configure(&global_config.pwm_lfo_config, 0, 0, 0, 0);
    triangle_init(&global.pwm_lfo, &global_config.pwm_lfo_config);

    // Amplitude envelope and LFO
    //adsr_configure(&global_amp_env_config, 0, 100, 127 << 8, 2000, 100 << 8, 3200, 0);
    adsr_configure(&global_config.amp_env_config, 0, 0, 127 << 8, 1, 127 << 8, 100, 0);
    //triangle_configure(&global_amp_lfo_config, 10000, 32767, 32767, 100);
    triangle_configure(&global_config.amp_lfo_config, 0, 0, 0, 100);
    triangle_init(&global.amp_lfo, &global_config.amp_lfo_config);

    // Pitch envelope and LFO
    //adsr_configure(&global_pitch_env_config, -256 * 2, 300, 0, 1, 0, 0, 0);
    const int16_t et = 6000;
    const int16_t ev = 6000;
    adsr_configure(&global_config.pitch_env_config, 0, et, ev, 0, ev, 0, ev);
    const int16_t ot = 400;
    const int16_t ov = 200;
    triangle_configure(&global_config.pitch_lfo_config, -ov, ov, 0, ot);
    triangle_init(&global.pitch_lfo, &global_config.pitch_lfo_config);

    //arp_configure(&global_config.arp_config, arp_octave_notes, sizeof (arp_octave_notes), 0);
    //arp_init(&global.arp, &global_config.arp_config);
    arp_configure(&global_config.arp_config, NULL, 0, 0);
    arp_init(&global.arp, &global_config.arp_config);

static void mod_basic() {
    // Detune based on depth knob
    int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
    global_config.pwm_lfo_config.low = -pwm_amt;
    global_config.pwm_lfo_config.high = pwm_amt;
    global_config.pwm_lfo_config.increment = knobs.depth >> 1;

    // Arp speed based on freq knob
    global_config.arp_config.period = (int16_t)knobs.freq * 10;
}
*/

static const struct patches {
    void (*init_fn)();
    void (*mod_fn)();
} patches[N_SELECTOR_POSITIONS] = {
    {
        .init_fn = init_basic,
        .mod_fn = mod_basic,
    },
    {
        .init_fn = init_chorus,
        .mod_fn = mod_chorus,
    },
    {
        .init_fn = init_wave,
        .mod_fn = mod_wave,
    },
    {
        .init_fn = init_bass,
        .mod_fn = mod_bass,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
    },
};

static void patch_init() {
    patches[knobs.select].init_fn();
}

static void patch_mod() {
    patches[knobs.select].mod_fn();
}

static void read_knobs() {
    static uint8_t last_select = 255;
    static uint8_t last_depth;
    static uint8_t last_freq;

    ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc; // Read from PA1
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t read_depth = ADC0.RES >> 2;
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc; // Read from PA2
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t read_freq = ADC0.RES >> 2;
    ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc; // Read from PA4
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t read_select = ADC0.RES >> 6;
    if (read_select >= N_SELECTOR_POSITIONS) read_select = N_SELECTOR_POSITIONS - 1;

    if (read_depth != last_depth) {
        last_depth = read_depth;
        knobs.depth = read_depth;
    }
    if (read_freq != last_freq) {
        last_freq = read_freq;
        knobs.freq = read_freq;
    }
    if (read_select != last_select) {
        last_select = read_select;
        knobs.select = read_select;
        all_notes_off();
        arp_init(&global.arp, &global_config.arp_config);
        global_osc_init();
        patch_init();
    }
}

// Update modulation
static void update_modulation() {
    read_knobs();

    patch_mod();

    bool notes_held = false;
    for (int i=0; i<N_OSC; i++) {
        notes_held = notes_held || osc_note_velocity[i] > 0;
    }

    // Advance arp
    int8_t arp_note = arp_update(&global.arp, &global_config.arp_config, notes_held);

    // Advance each oscillator
    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        uint8_t note = osc_note_index[i];
        uint8_t velocity = osc_note_velocity[i];

        // Advance pwm LFO
        uint8_t pwm_lfo_value = 127 + (triangle_update(&osc->pwm_lfo, &global.osc[i].pwm_lfo_config, notes_held) >> 8);

        // Advance amplitude LFO
        uint16_t amp_lfo_value = 32767 + triangle_update(&osc->amp_lfo, &global.osc[i].amp_lfo_config, notes_held);

        // Advance pitch LFO
        int16_t pitch_lfo_value = triangle_update(&osc->pitch_lfo, &global.osc[i].pitch_lfo_config, notes_held);

        // Store triggered velocity
        uint8_t gate = 0;
        if (velocity > 0) {
            osc->triggered_note = note;
            osc->triggered_velocity = velocity;
            gate = note + 1;
        }

        // Advance envelope generators
        int16_t amp_env_value = adsr_update(&osc->amp_env, &global.osc[i].amp_env_config, gate);
        int16_t pitch_env_value = adsr_update(&osc->pitch_env, &global.osc[i].pitch_env_config, gate);
        int16_t pitch_adjust = ((int32_t)pitch_env_value * pitch_lfo_value) >> 15;

        // Calculate timer periods
        uint16_t output_period = period(((osc->triggered_note + arp_note) << 8) + pitch_adjust);
        if (output_period < 2 * MIN_PER) {
            output_period = 2 * MIN_PER;
        }
        uint16_t per1 = ((uint32_t)output_period * pwm_lfo_value) >> 8;
        uint16_t per2 = output_period - per1;
        if (per1 < MIN_PER) {
            per1 = MIN_PER;
            per2 = output_period - MIN_PER;
        } else if (per2 < MIN_PER) {
            per2 = MIN_PER;
            per1 = output_period - MIN_PER;
        }

        int8_t adjusted_velocity = 64 + (osc->triggered_velocity >> 1);

        // Update low-level osc state
        int16_t amp_multiplier = ((int32_t)amp_env_value * amp_lfo_value) >> 16;
        osc->timer_period_high = per1;
        osc->timer_period_low = per2;
        osc->amplitude = ((int32_t)amp_multiplier * adjusted_velocity) >> 15;
    }

    // See if any oscillators are outputting
    bool led_on = false;
    for (int i=0; i<N_OSC; i++) {
        led_on = led_on || global.osc[i].amplitude > 0;
    }

    if (led_on) {
        PORTB.OUT |= 1 << 0;
    } else {
        PORTB.OUT &= ~(1 << 0);
    }

}

static void update_output(void) {
    int16_t out = 0;
    for (int i = 0; i < N_OSC; i++) {
        out += global.mixer_inputs[i];
    }
    DAC0.DATA = 127 + (out >> 2);
}

ISR (TCA0_OVF_vect) 
{
    TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
    if (osc_handle_timer(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
        update_output();
    }
}

ISR (TCB0_INT_vect) 
{
    TCB0.INTFLAGS |= TCB_CAPT_bm;
    if (osc_handle_timer(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
        update_output();
    }
}

ISR (TCB1_INT_vect) 
{
    TCB1.INTFLAGS |= TCB_CAPT_bm;
    if (osc_handle_timer(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
        update_output();
    }
}
