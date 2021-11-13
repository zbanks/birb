#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MIDI_CHANNEL 0

// This lkup uses 16 counts per period
const uint16_t PERIOD_LKUP[] = {
38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355, 335, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168, 158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33, 31, 30, 28, 26, };

static uint32_t t;
static uint32_t u;

static void note_on(uint8_t k, uint8_t v);
static void note_off(uint8_t k);

#define MAX_NOTES 8

static uint8_t note_index[MAX_NOTES];
static uint8_t note_velocity[MAX_NOTES];

static uint8_t triggered_note;
static uint8_t triggered_velocity;
static uint16_t target_velocity;
static uint16_t velocity_change_rate = 30;
static uint8_t real_velocity;

static uint16_t arp_counter;
static uint8_t arp_index;
static uint16_t decay_counter;
//static uint16_t mod_counter;

static int16_t bend;
//static uint8_t frac_note_counter;

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

struct adsr_state {
    enum {
        ADSR_OFF,
        ADSR_ATTACK,
        ADSR_DECAY,
        ADSR_SUSTAIN,
        ADSR_RELEASE,
    } mode;
    int8_t initial_value;
    int8_t a_incr;
    int8_t a_value;
    int8_t d_incr;
    int8_t s_value;
    int8_t r_incr;
    int8_t final_value;
    int8_t output;
    uint8_t last_gate;
};

static struct adsr_state amp_env;
static struct triangle_state amp_lfo;
//static struct adsr_state pitch_env;
static struct triangle_state pitch_lfo;
static struct adsr_state pitch_lfo_env;
static struct adsr_state mod_env;
static struct triangle_state mod_lfo;

static int8_t mod_env_value;
static int8_t mod_lfo_value;

static void adsr_init(struct adsr_state *state, int8_t initial_value, int8_t a_time, int8_t a_value, int8_t d_time, int8_t s_value, int8_t r_time, int8_t final_value) {
    state->mode = ADSR_OFF;
    state->initial_value = initial_value;
    if (a_time <= 0) {
        a_time = 1;
    }
    state->a_incr = ((int16_t)a_value - initial_value) / a_time;
    if (state->a_incr == 0) {
        state->a_incr = a_value > initial_value ? 1 : -1;
    }
    state->a_value = a_value;
    if (d_time <= 0) {
        d_time = 1;
    }
    state->d_incr = ((int16_t)s_value - a_value) / d_time;
    if (state->d_incr == 0) {
        state->d_incr = s_value > a_value ? 1 : -1;
    }
    state->s_value = s_value;
    if (r_time <= 0) {
        r_time = 1;
    }
    state->r_incr = ((int16_t)final_value - s_value) / r_time;
    if (state->r_incr == 0) {
        state->r_incr = final_value > s_value ? 1 : -1;
    }
    state->final_value = final_value;
    state->output = final_value;
    state->last_gate = 0;
}

static uint8_t adsr_update(struct adsr_state *state, uint8_t gate) {
    if ((state->mode == ADSR_OFF || state->mode == ADSR_RELEASE || gate != state->last_gate) && gate) {
        state->output = state->initial_value;
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
            if ((state->a_incr > 0 && state->output >= state->a_value - state->a_incr)
             || (state->a_incr < 0 && state->output <= state->a_value - state->a_incr)) {
                state->output = state->a_value;
                state->mode = ADSR_DECAY;
            } else {
                state->output += state->a_incr;
            }
            break;
        case ADSR_DECAY:
            if ((state->d_incr > 0 && state->output >= state->s_value - state->d_incr)
             || (state->d_incr < 0 && state->output <= state->s_value - state->d_incr)) {
                state->output = state->s_value;
                state->mode = ADSR_SUSTAIN;
            } else {
                state->output += state->d_incr;
            }
            break;
        case ADSR_RELEASE:
            if ((state->r_incr > 0 && state->output >= state->final_value - state->r_incr)
             || (state->r_incr < 0 && state->output <= state->final_value - state->r_incr)) {
                state->output = state->final_value;
                state->mode = ADSR_OFF;
            } else {
                state->output += state->r_incr;
            }
            break;
    }
    return state->output;
}

struct triangle_state {
    uint8_t up;
    int8_t low;
    int8_t high;
    int8_t increment;
    int8_t output;
};

static int8_t triangle_init(struct triangle_state *state, int8_t low, int8_t high, int8_t period) {
    state->low = low;
    state->high = high;
    if (period == 0) {
        period = 1;
    }
    state->increment = ((uint16_t)high - low) / period;
    if (state->increment == 0) {
        state->increment = 1;
    }
    state->output = (low + high) >> 1;
    state->up = 1;
}

static int8_t triangle_update(struct triangle_state *state) {
    if (state->up) {
        if (state->output > state->high - state->increment) {
            state->output = state->high;
            state->up = 0;
        } else {
            state->output += state->increment;
        }
    } else {
        if (state->output < state->low + state->increment) {
            state->output = state->low;
            state->up = 1;
        } else {
            state->output -= state->increment;
        }
    }
    return state->output;
}

static uint8_t triangle(uint8_t counter, uint8_t low, uint8_t high) {
    uint8_t tri = counter < 128 ? counter : -counter;
    return (low * (128 - tri) + high * tri);
}

static void rx(uint8_t byte) {
    static enum {
        UNKNOWN,
        WAIT_NOTE_ON_KEY,
        WAIT_NOTE_ON_VELOCITY,
        WAIT_NOTE_OFF_KEY,
        WAIT_NOTE_OFF_VELOCITY,
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
        }
    }
}

static void check_notes();

static void note_on(uint8_t k, uint8_t v) {
    if (v == 0) {
        note_off(k);
        return;
    }

    for (int i = 0; i < MAX_NOTES; i++) {
        if (note_velocity[i] == 0) {
            note_index[i] = k;
            note_velocity[i] = v;
            arp_index = i;
            check_notes();
            return;
        }
    }
}

static void note_off(uint8_t k) {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (note_index[i] == k && note_velocity[i] > 0) {
            note_velocity[i] = 0;
            if (arp_index >= i && arp_index > 0) {
                arp_index--;
            }
            for (int j = i + 1; j < MAX_NOTES; j++) {
                // Shift held notes to keep the list coherent
                if (note_velocity[j] == 0) {
                    break;
                } else {
                    note_index[j - 1] = note_index[j];
                    note_velocity[j - 1] = note_velocity[j];
                    note_velocity[j] = 0;
                }
            }
            check_notes();
            return;
        }
    }
}

ISR (USART0_RXC_vect) 
{
    rx(USART0.RXDATAL);
}

static void check_notes() {
}

int
main (void)
{
    // Set up external clock input
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA |= CLKCTRL_CLKSEL_EXTCLK_gc;
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PEN_bm;

//BREAK

//    PORTA.DIRSET = 1 << 1; // Set PA1 as output
//    for (;;) {
//        PORTA.OUT ^= 1 << 1;
//        _delay_ms(500);
//    }

//BREAK

    // Set up audio-frequency timer

    TCA0.SINGLE.PER = 1250; // Just to get the audio loop going
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;

    // Set up modulation timer

    TCB0.CCMP = 1250; // 8 KHz update rate
    TCB0.INTCTRL |= TCB_CAPT_bm;
    TCB0.CTRLA |= TCB_ENABLE_bm;

    PORTA.DIRSET = 1 << 1; // Set PA1 as output

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

    // Disable everything else on PA4
    PORTA.PIN4CTRL &= ~PORT_ISC_gm;
    PORTA.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN4CTRL &= ~PORT_PULLUPEN_bm;

    // Disable everything else on PA2
    PORTA.PIN2CTRL &= ~PORT_ISC_gm;
    PORTA.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN2CTRL &= ~PORT_PULLUPEN_bm;

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

    // Set audio interrupt at higher priority
    CPUINT.LVL1VEC = TCA0_OVF_vect_num;

    adsr_init(&amp_env, 0, 1, 127, 20, 100, 32, 0);
    //adsr_init(&amp_env, 1, 255, 127, 255);
    //triangle_init(&amp_lfo, -100, 100, 30);

    //adsr_init(&pitch_env, 0, 1, 0, 1, 0, 100, -127);
    triangle_init(&pitch_lfo, -60, 60, 30);
    adsr_init(&pitch_lfo_env, 0, 100, 100, 1, 100, 1, 100);

    adsr_init(&mod_env, 127, 100, 64 + 32, 1, 64 + 32, 100, 127);
    triangle_init(&mod_lfo, 0x3F, 0x7F, 10);
    triangle_init(&mod_lfo, 0, 0, 10); // XXX

    sei();
    //uint8_t a = 0;

    uint8_t last_a = 0;

    for(;;);
}

// Update modulation
ISR (TCB0_INT_vect)
{
    ADC0.MUXPOS &= ~ADC_MUXPOS_AIN4_gc;
    ADC0.MUXPOS |= ADC_MUXPOS_AIN2_gc; // Read from PA2
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t knob = ADC0.RES >> 2;

    static uint8_t note_playing;

    if (!note_playing && note_velocity[0] > 0) {
        // We need to sync
        note_playing = 1;
        arp_index = 0;
        arp_counter = ~0;
        //mod_counter = 0;
    }

    if (arp_counter > (knob << 3)) {
        arp_counter = 0;
        arp_index++;
        if (arp_index >= MAX_NOTES || note_velocity[arp_index] == 0) {
            arp_index = 0;
        }
    }
    arp_counter++;

    triggered_note = note_index[arp_index];
    triggered_velocity = note_velocity[arp_index];

    if (decay_counter > velocity_change_rate) {
        //real_velocity = ((uint16_t)adsr_update(&amp_env, triggered_velocity > 0 ? triggered_note + 1 : 0) * triggered_velocity) >> 8;
        uint8_t note_gate = triggered_velocity > 0 ? triggered_note + 1 : 0;
        uint8_t smooth_gate = triggered_velocity > 0 ? 1 : 0;
        int8_t vel = adsr_update(&amp_env, note_gate);
        //int8_t pitch_env_value = adsr_update(&pitch_env, smooth_gate);
        int8_t pitch_lfo_value = triangle_update(&pitch_lfo);
        int8_t pitch_lfo_env_value = adsr_update(&pitch_lfo_env, smooth_gate);
        pitch_lfo_value = ((int16_t)pitch_lfo_value * pitch_lfo_env_value) >> 12;

        mod_env_value = adsr_update(&mod_env, note_gate);
        mod_lfo_value = triangle_update(&mod_lfo);

        bend = ((int16_t)pitch_lfo_value << 4);
        //bend += ((int16_t)pitch_env_value << 4);
        //if (vel > 0) {
            //int8_t vel_lfo = triangle_update(&amp_lfo) >> 4;
            //vel = sadd(vel, vel_lfo);
        //}
        if (vel < 0) {
            vel = 0;
        }
        real_velocity = vel * 2;
        decay_counter = 0;
    }
    decay_counter++;

    //mod_counter++;

    note_playing = real_velocity > 0;

    if (note_playing) {
        PORTA.OUT |= 1 << 1;
    } else {
        PORTA.OUT &= ~(1 << 1);
    }
    TCB0.INTFLAGS |= TCB_CAPT_bm;
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

// Update DAC
ISR (TCA0_OVF_vect) 
{
    static uint8_t last_note;
    static uint8_t last_velocity;
    static uint8_t last_mod;

    // Square
    #define T_PERIOD_BITS 6
    #define T_PERIOD_BM ((1 << T_PERIOD_BITS) - 1)

    //uint8_t t_byte = (uint8_t)(t & T_PERIOD_BM);
    //t_byte = t_byte > 0 ? 0 : 255;
    //uint8_t t_byte = (t & 0xF) == 0 ? 255 : 0;

    uint16_t t1 = t * 3;
    uint16_t t2 = t * 2;

    uint8_t t_byte = ((t1 | t2) & T_PERIOD_BM) << (8 - T_PERIOD_BITS);

    t_byte &= last_mod;

    //uint8_t t_byte = t * 5 | (t * 3 & 0x3F);

    //DAC0.DATA = t_byte > 32 ? 0 : last_velocity;

    DAC0.DATA = ((uint16_t)t_byte * last_velocity) >> 8;

    t++;

    if ((t & T_PERIOD_BM) == 0) {
        TCA0.SINGLE.PER = period(((int16_t)triggered_note << 8) + bend);
        last_note = triggered_note;
        last_velocity = real_velocity;
        last_mod = ((uint8_t)mod_lfo_value << 1) | ((uint8_t)mod_env_value << 1);
    }
    TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
}
