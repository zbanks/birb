#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MIDI_CHANNEL 0

//const uint16_t PERIOD_LKUP[] = {
//30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, };

//const uint8_t INCREMENT_BITS_LKUP[] = {
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, };

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
static uint16_t velocity_change_rate;
static uint16_t real_velocity;

static uint16_t arp_counter;
static uint8_t arp_index;
static uint8_t decay_counter;

static uint16_t frac_note;
static uint8_t frac_note_counter;

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

    if ((byte & 0xF8) == 0xF8) {
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

int
main (void)
{
    // Set up external clock input
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA |= CLKCTRL_CLKSEL_EXTCLK_gc;
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PEN_bm;

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

    sei();
    //uint8_t a = 0;

    uint8_t last_a = 0;

    for(;;);

    //while(1) 
    //{
    //    ADC0.MUXPOS &= ~ADC_MUXPOS_AIN4_gc;
    //    ADC0.MUXPOS |= ADC_MUXPOS_AIN2_gc; // Read from PA2
    //    ADC0.COMMAND |= ADC_STCONV_bm;
    //    while (ADC0.COMMAND & ADC_STCONV_bm);
    //    uint8_t knob = ADC0.RES >> 2;
    //    ADC0.MUXPOS &= ~ADC_MUXPOS_AIN2_gc;
    //    ADC0.MUXPOS |= ADC_MUXPOS_AIN4_gc; // Read from PA4
    //    ADC0.COMMAND |= ADC_STCONV_bm;
    //    while (ADC0.COMMAND & ADC_STCONV_bm);
    //    uint8_t inp = ADC0.RES >> 2;

    //    //uint8_t a = t*((t>>12 | t>>8)&63&t>>4);
    //    //uint8_t a = (t*5&t>>7)|(t*3&t>>10);
    //    //uint8_t a = t << 3;
    //    //uint8_t a = (t * 9 & t >> 4 | t * 5 & t >> 7 | t * 30 & t / 1024) - 1;
    //    //DAC0.DATA = inp & knob;

    //    if (knob > 127) {
    //        // Square
    //        uint8_t t_byte = (uint8_t)(t >> 8);
    //        DAC0.DATA = t_byte > knob ? velocity << 1 : 0;
    //    } else {
    //        // Saw
    //        uint8_t t_byte = (uint8_t)(t >> 8);
    //        uint8_t saw = t_byte < (knob << 1) ? t_byte : 0;
    //        DAC0.DATA = velocity ? saw : 0;
    //    }

    //    //DAC0.DATA = t >> 4;
    //    t += increment;
    //    if (knob == 0) t = 0;
    //    while (!(TCA0.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm));
    //    TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
    //}
}

// Update modulation
ISR (TCB0_INT_vect)
{
    //// Find last held note
    //if (note_velocity[i] != 0) {
    //    break;
    //}
    //new_velocity = note_velocity[i];
    //new_note = note_index[i];

    //if (new_velocity > 0 && (new_velocity != velocity || new_note != note)) {
    //    // Note on!
    //    note = new_note;
    //    velocity = new_velocity;

    //    // New note this frame
    //    t = 0;
    //    decay_counter = 0;

    //    TCA0.SINGLE.CNT = 0;
    //    PORTA.OUT |= 1 << 1;
    //} else if (new_velocity == 0 && velocity > 0) {
    //    // Note off!
    //    velocity = 0;
    //    PORTA.OUT &= ~(1 << 1);
    //}

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
        frac_note = 0;
    }

    if (arp_counter > (knob << 3)) {
        arp_counter = 0;
        arp_index++;
        if (arp_index >= MAX_NOTES || note_velocity[arp_index] == 0) {
            arp_index = 0;
        }
    }
    arp_counter++;

    if (frac_note < (30 << 8) && frac_note_counter > 4) {
        frac_note++;
        frac_note_counter = 0;
    }
    frac_note_counter++;

    triggered_note = note_index[arp_index];
    triggered_velocity = note_velocity[arp_index];

    if (triggered_velocity > 0) {
        target_velocity = triggered_velocity + 128; // Only use upper half of volume range
        velocity_change_rate = 10;
    } else {
        target_velocity = 0;
        velocity_change_rate = 10;
    }

    if (real_velocity != target_velocity) {
        if (decay_counter > velocity_change_rate) {
            //int16_t error = (int16_t)target_velocity - real_velocity;
            //error = (error * 255) / 256;
            //real_velocity = (int16_t)target_velocity - error;
            if(real_velocity < target_velocity) real_velocity++;
            if(real_velocity > target_velocity) real_velocity--;
            decay_counter = 0;
        }
        decay_counter++;
    }
    note_playing = real_velocity > 0;

    if (note_playing) {
        PORTA.OUT |= 1 << 1;
    } else {
        PORTA.OUT &= ~(1 << 1);
    }
    TCB0.INTFLAGS |= TCB_CAPT_bm;
}

// Update DAC
ISR (TCA0_OVF_vect) 
{
    static uint8_t last_note;
    static uint8_t last_velocity;

    //ADC0.MUXPOS &= ~ADC_MUXPOS_AIN2_gc;
    //ADC0.MUXPOS |= ADC_MUXPOS_AIN4_gc; // Read from PA4
    //ADC0.COMMAND |= ADC_STCONV_bm;
    //while (ADC0.COMMAND & ADC_STCONV_bm);
    //uint8_t inp = ADC0.RES >> 2;

    //uint8_t a = t*((t>>12 | t>>8)&63&t>>4);
    //uint8_t a = (t*5&t>>7)|(t*3&t>>10);
    //uint8_t a = t << 3;
    //uint8_t a = (t * 9 & t >> 4 | t * 5 & t >> 7 | t * 30 & t / 1024) - 1;
    //DAC0.DATA = inp & knob;


    // Square
    #define T_PERIOD_BM 0xF
    #define T_PERIOD_BITS 4

    uint8_t t_byte = (uint8_t)(t & T_PERIOD_BM);
    DAC0.DATA = t_byte > 0 ? 0 : last_velocity;
    //DAC0.DATA = (t_byte * last_velocity) >> 8;
    //} else {
    //    // Saw
    //    uint8_t t_byte = (uint8_t)(t >> 8);
    //    uint8_t saw = t_byte < (knob << 1) ? t_byte : 0;
    //    DAC0.DATA = velocity ? saw : 0;
    //}

    //DAC0.DATA = t >> 4;
    t++;
    //if (knob == 0) t = 0;

    if ((t & T_PERIOD_BM) == 0 && (triggered_note != last_note || real_velocity != last_velocity || 1)) {
        // Set osc as per new note
        uint8_t ix = triggered_note + (frac_note >> 8);
        if (ix > 100) { // Highest note to avoid deadlock
            ix = 100;
        }
        uint16_t per_low = PERIOD_LKUP[ix];
        uint16_t per_high = PERIOD_LKUP[ix + 1];
        uint8_t frac = frac_note & 0xFF;
        //frac = 0;
        uint16_t per = ((uint32_t)per_low * (256 - frac) + (uint32_t)per_high * frac) >> 8;
        TCA0.SINGLE.PER = per;
        last_note = triggered_note;
        last_velocity = real_velocity;
    }
    TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
}
