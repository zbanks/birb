#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MIDI_CHANNEL 0

// This lkup uses 16 counts per period
const uint16_t PERIOD_LKUP[] = {
38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355, 335, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168, 158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33, 31, 30, 28, 26, };

static void note_on(uint8_t k, uint8_t v);
static void note_off(uint8_t k);

#define MAX_NOTES 8
static uint8_t note_index[MAX_NOTES];
static uint8_t note_velocity[MAX_NOTES];

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

struct adsr_state {
    enum {
        ADSR_OFF,
        ADSR_ATTACK,
        ADSR_DECAY,
        ADSR_SUSTAIN,
        ADSR_RELEASE,
    } mode;
    int16_t initial_value;
    int16_t a_incr;
    int16_t a_value;
    int16_t d_incr;
    int16_t s_value;
    int16_t r_incr;
    int16_t final_value;
    int16_t output;
    uint8_t last_gate;
};

static void adsr_init(struct adsr_state *state, int8_t initial_value, int8_t a_time, int8_t a_value, int8_t d_time, int8_t s_value, int8_t r_time, int8_t final_value) {
    state->mode = ADSR_OFF;
    state->initial_value = initial_value;
    if (a_time <= 0) {
        a_time = 1;
    }
    state->a_incr = (a_value - initial_value) / a_time;
    if (state->a_incr == 0) {
        state->a_incr = a_value > initial_value ? 1 : -1;
    }
    state->a_value = a_value;
    if (d_time <= 0) {
        d_time = 1;
    }
    state->d_incr = (s_value - a_value) / d_time;
    if (state->d_incr == 0) {
        state->d_incr = s_value > a_value ? 1 : -1;
    }
    state->s_value = s_value;
    if (r_time <= 0) {
        r_time = 1;
    }
    state->r_incr = (final_value - s_value) / r_time;
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
    int16_t low;
    int16_t high;
    int16_t increment;
    int16_t output;
    uint8_t last_gate;
};

static void triangle_init(struct triangle_state *state, int16_t low, int16_t high, int16_t period) {
    state->low = low;
    state->high = high;
    if (period == 0) {
        period = 1;
    }
    state->increment = (high - low) / period;
    if (state->increment == 0) {
        state->increment = 1;
    }
    state->output = low;
    state->up = 1;
    state->last_gate = 0;
}

static int16_t triangle_update(struct triangle_state *state, uint8_t gate) {
    if (gate && gate != state->last_gate) {
        state->output = state->low;
        state->up = 1;
    } else {
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
    a = (a^c^x);       //note the mix of addition and XOR
    b = (b+a);         //And the use of very few instructions
    c = (c+(b>>1)^a);  //the right shift is to ensure that high-order bits from b can affect  
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
            check_notes();
            return;
        }
    }
}

static void note_off(uint8_t k) {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (note_index[i] == k && note_velocity[i] > 0) {
            note_velocity[i] = 0;
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

static struct triangle_state detune_lfo;

static void update_modulation();

int
main (void)
{
    // Set up external clock input
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA |= CLKCTRL_CLKSEL_EXTCLK_gc;
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    // Set up TCB0 as first voice oscillator
    TCB0.CCMP = 1250; // Just to get the audio loop going
    TCB0.INTCTRL |= TCB_CAPT_bm;
    TCB0.CTRLA |= TCB_ENABLE_bm;

    // Set up TCB1 as second voice oscillator
    TCB1.CCMP = 1250; // Just to get the audio loop going
    TCB1.INTCTRL |= TCB_CAPT_bm;
    TCB1.CTRLA |= TCB_ENABLE_bm;

    //TCB1.CCMP = 1250; // Just to get the audio loop going
    //TCB1.INTCTRL |= TCB_CAPT_bm;
    //TCB1.CTRLA |= TCB_ENABLE_bm;

    // Set up modulation timer

    TCA0.SINGLE.PER = 1250; // 8 KHz update rate
    //TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

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

    triangle_init(&detune_lfo, 0, 0, 0);

    sei();

    for(;;) {
        asm volatile("": : :"memory");
        if (TCA0.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm) {
            TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
            update_modulation();
        }
    }
}

uint8_t knob;

#define MIN_PER 10
static uint16_t per11 = 1250;
static uint16_t per12 = 1250;
static uint16_t per21 = 1250;
static uint16_t per22 = 1250;

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

static uint8_t output_amplitude1;
static uint8_t output_amplitude2;
static uint8_t out1;
static uint8_t out2;

// Update modulation
static void update_modulation() {
    ADC0.MUXPOS |= ADC_MUXPOS_AIN1_gc; // Read from PA1
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    knob = ADC0.RES >> 2;

    int16_t detune_amt = (knob < 12 ? knob : 12) << 10;
    detune_lfo.low = -detune_amt;
    detune_lfo.high = detune_amt;
    detune_lfo.increment = knob >> 1;

    uint8_t detune_lfo_value = 127 + (triangle_update(&detune_lfo, 1) >> 8);

    uint8_t triggered_note1 = note_index[0];
    uint8_t triggered_velocity1 = note_velocity[0];

    uint16_t output_period1 = period(triggered_note1 << 8);
    if (output_period1 < 2 * MIN_PER) {
        output_period1 = MIN_PER;
    }
    per11 = ((uint32_t)output_period1 * detune_lfo_value) >> 8;
    per12 = output_period1 - per11;
    if (per11 < MIN_PER) {
        per11 = MIN_PER;
        per12 = output_period1 - MIN_PER;
    } else if (per12 < MIN_PER) {
        per12 = MIN_PER;
        per11 = output_period1 - MIN_PER;
    }
    output_amplitude1 = triggered_velocity1;

    uint8_t triggered_note2 = note_index[1];
    uint8_t triggered_velocity2 = note_velocity[1];

    uint16_t output_period2 = period(triggered_note2 << 8);
    if (output_period2 < 2 * MIN_PER) {
        output_period2 = MIN_PER;
    }
    per21 = ((uint32_t)output_period2 * detune_lfo_value) >> 8;
    per22 = output_period2 - per21;
    if (per21 < MIN_PER) {
        per21 = MIN_PER;
        per22 = output_period2 - MIN_PER;
    } else if (per22 < MIN_PER) {
        per22 = MIN_PER;
        per21 = output_period2 - MIN_PER;
    }
    output_amplitude2 = triggered_velocity2;

    if (output_amplitude1 > 0 || output_amplitude2 > 0) {
        PORTB.OUT |= 1 << 0;
    } else {
        PORTB.OUT &= ~(1 << 0);
    }

}

ISR (TCB0_INT_vect) 
{
    static uint8_t prescaler;

    TCB0.INTFLAGS |= TCB_CAPT_bm;

    if (prescaler > 4) {
        prescaler = 0;
    } else {
        prescaler++;
        return;
    }

    static uint8_t t0;
    TCB0.CCMP = (t0 & 1) ? per12 : per11;
    out1 = (t0 & 1) ? output_amplitude1 : 0;
    DAC0.DATA = out1 + out2;
    t0++;
}

ISR (TCB1_INT_vect) 
{
    static uint8_t prescaler;

    TCB1.INTFLAGS |= TCB_CAPT_bm;

    if (prescaler > 4) {
        prescaler = 0;
    } else {
        prescaler++;
        return;
    }

    static uint8_t t0;
    TCB1.CCMP = (t0 & 1) ? per22 : per21;
    out2 = (t0 & 1) ? output_amplitude2 : 0;
    DAC0.DATA = out1 + out2;
    t0++;
}

//ISR (TCB1_INT_vect) 
//{
//    
//    t1++;
//    update();
//
//    uint8_t detune = ((uint16_t)knob * (uint16_t)rand()) >> 8;
//
//    if ((t1 & T_PERIOD_BM) == 0) {
//        TCB1.CCMP = (period(((int16_t)triggered_note << 8) + bend + detune) << 3);
//    }
//    TCB1.INTFLAGS |= TCB_CAPT_bm;
//}
