#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

#define MIDI_CHANNEL 0

// This lkup uses 16 counts per period
const uint16_t PERIOD_LKUP[] = {
38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355, 335, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168, 158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33, 31, 30, 28, 26, };

static void note_on(uint8_t k, uint8_t v);
static void note_off(uint8_t k);

#define N_VOICES 3
static uint8_t note_index[N_VOICES];
static uint8_t note_velocity[N_VOICES];

static bool poly = true;

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
    config->increment = (high - low) / period;
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

static uint8_t rand() {
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

static void check_notes() {
}

static struct triangle_config detune_lfo_config;
static struct triangle_state detune_lfo;

static void update_modulation();

struct osc_state {
    // Low-level timer stuff
    uint8_t prescaler;
    uint16_t timer_period_high;
    uint16_t timer_period_low;
    uint8_t t;

    uint8_t triggered_note;
    uint8_t triggered_velocity;

    struct adsr_state amp_env;
    struct adsr_state pitch_env;

    // Output stuff
    int8_t amplitude;
};

static bool osc_handle_timer(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // Returns the output signal level
    // and updates next_period with the time until this function should be called next

    if (state->prescaler > 4) {
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
    // Set osc as per new note
    if (note < 0) { // Highest note to avoid deadlock
        note = 0;
    }
    if (note > (100 << 8)) { // Highest note to avoid deadlock
        note = (100 << 8);
    }
    uint8_t ix = note >> 8;
    uint16_t per_low = PERIOD_LKUP[ix];
    uint16_t per_high = PERIOD_LKUP[ix + 1];
    uint8_t frac = note & 0xFF;
    uint16_t per = ((uint32_t)per_low * (256 - frac) + (uint32_t)per_high * frac) >> 8;
    return per;
}


#define MIN_PER 10

static struct global_config {
    struct adsr_config amp_env_config;
    struct triangle_config amp_lfo_config;
    struct adsr_config pitch_env_config;
    struct triangle_config pitch_lfo_config;
    uint16_t arp_period;
} global_config;

static struct global_state {
    struct osc_state osc[N_VOICES];
    int8_t mixer_inputs[N_VOICES];
    struct triangle_state amp_lfo;
    struct triangle_state pitch_lfo;
    int8_t arp_note;
} global;

static void global_osc_init() {
    for (int i = 0; i < N_VOICES; i++) {
        global.osc[i].prescaler = 0;
        global.osc[i].timer_period_high = 1250;
        global.osc[i].timer_period_low = 1250;
        global.osc[i].t = 0;
        global.osc[i].amplitude = 0;
        adsr_init(&global.osc[i].amp_env, &global_config.amp_env_config);
        adsr_init(&global.osc[i].pitch_env, &global_config.pitch_env_config);
    }
}

static void note_on(uint8_t k, uint8_t v) {
    if (v == 0) {
        note_off(k);
        return;
    }

     uint8_t n_voices = poly ? N_VOICES : 1;

    // Look for a free oscillator to use
    for (int i = 0; i < n_voices; i++) {
        if (note_velocity[i] == 0 && (note_index[i] == k || global.osc[i].amplitude == 0)) {
            note_index[i] = k;
            note_velocity[i] = v;
            check_notes();
            return;
        }
    }
    // Look for a releasing note to use
    for (int i = 0; i < n_voices; i++) {
        if (note_velocity[i] == 0) {
            note_index[i] = k;
            note_velocity[i] = v;
            check_notes();
            return;
        }
    }
    // Overwrite a currently playing note
    note_index[0] = k;
    note_velocity[0] = v;
}

static void note_off(uint8_t k) {
    for (int i = 0; i < N_VOICES; i++) {
        if (note_index[i] == k && note_velocity[i] > 0) {
            note_velocity[i] = 0;
            check_notes();
            return;
        }
    }
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
    triangle_configure(&detune_lfo_config, 0, 0, 0, 0);
    triangle_init(&detune_lfo, &detune_lfo_config);

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

    global_osc_init();

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

static void select_changed() {
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
        select_changed();
    }
}

uint8_t depth;
uint8_t freq;

// Update modulation
static void update_modulation() {
    read_knobs();

    bool notes_held = false;
    for (int i=0; i<N_VOICES; i++) {
        notes_held = notes_held || note_velocity[i] > 0;
    }

    // Advance global detune LFO
    int16_t detune_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
    detune_lfo_config.low = -detune_amt;
    detune_lfo_config.high = detune_amt;
    detune_lfo_config.increment = knobs.depth >> 1;
    uint8_t detune_lfo_value = 127 + (triangle_update(&detune_lfo, &detune_lfo_config, notes_held) >> 8);

    // Advance global amplitude LFO
    uint16_t amp_lfo_value = 32767 + triangle_update(&global.amp_lfo, &global_config.amp_lfo_config, notes_held);

    // Advance global pitch LFO
    int16_t pitch_lfo_value = triangle_update(&global.pitch_lfo, &global_config.pitch_lfo_config, notes_held);

    // Advance arp, if appropriate
    global_config.arp_period = (int16_t)knobs.freq * 10;
    uint8_t arp_gate = notes_held;
    static uint8_t last_arp_gate;
    static uint16_t arp_timer;
    static uint16_t arp_index;
    if (arp_gate && arp_gate != last_arp_gate) {
        arp_timer = 0;
        arp_index = 0;
    }
    //static int8_t arp_note;
    if (arp_gate) {
        arp_timer++;
        if (arp_timer > global_config.arp_period) {
            arp_timer = 0;
            // Advance arp
            //int8_t new_note = arp_note;
            //while (new_note == arp_note) new_note = (rand() & 7) - 4;
            //arp_note = new_note;
            arp_index++;
        }
    }
    last_arp_gate = arp_gate;
    // End arp

    global.arp_note = -12 + (arp_index & 1) * 12;

    for (int i=0; i<N_VOICES; i++) {
        struct osc_state *osc = &global.osc[i];
        uint8_t note = note_index[i];
        uint8_t velocity = note_velocity[i];

        // Store triggered velocity
        uint8_t gate = 0;
        if (velocity > 0) {
            osc->triggered_note = note;
            osc->triggered_velocity = velocity;
            gate = note + 1;
        }

        // Advance envelope generators
        int16_t amp_env_value = adsr_update(&osc->amp_env, &global_config.amp_env_config, gate);
        int16_t pitch_env_value = adsr_update(&osc->pitch_env, &global_config.pitch_env_config, gate);
        int16_t pitch_adjust = ((int32_t)pitch_env_value * pitch_lfo_value) >> 15;

        // Calculate timer periods
        uint16_t output_period = period(((osc->triggered_note + global.arp_note) << 8) + pitch_adjust);
        if (output_period < 2 * MIN_PER) {
            output_period = MIN_PER;
        }
        uint16_t per1 = ((uint32_t)output_period * detune_lfo_value) >> 8;
        uint16_t per2 = output_period - per1;
        if (per1 < MIN_PER) {
            per1 = MIN_PER;
            per2 = output_period - MIN_PER;
        } else if (per2 < MIN_PER) {
            per2 = MIN_PER;
            per1 = output_period - MIN_PER;
        }

        // Update osc
        osc->timer_period_high = per1;
        osc->timer_period_low = per2;
        int16_t amp_multiplier = ((int32_t)amp_env_value * amp_lfo_value) >> 16;
        osc->amplitude = ((int32_t)amp_multiplier * osc->triggered_velocity) >> 15;
    }

    // See if any oscillators are outputting
    bool led_on = false;
    for (int i=0; i<N_VOICES; i++) {
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
    for (int i = 0; i < N_VOICES; i++) {
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
