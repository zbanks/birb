#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIDI_CHANNEL 0

// For proper tuning, ensure PRESCALER * (2 ^ OCTAVE_OFFSET) = COUNTS_PER_PERIOD
// (COUNTS_PER_PERIOD = 16 for the lookup table below)

// This lookup table uses 16 counts per period
#define LOOKUP_COUNTS_PER_PERIOD 16
const uint16_t PERIOD_LOOKUP[] = {
38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355, 335, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168, 158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33, 31, 30, 28, 26, };

const uint8_t BASS[] = {21,47,180,101,89,225,106,57,149,167,78,234,131,62,116,44,86,86,196,86,224,69,165,78,101,47,182,197,54,117,156,196,15,248,129,19,248,209,17,255,129,191,0,244,11,248,133,253,63,209,31,252,63,2,44,128,2,0,208,85,255,203,191,251,180,1,208,123,16,63,193,255,96,7,0,5,119,17,0,182,255,255,127,172,83,95,1,208,203,255,0,247,255,58,1,0,0,1,2,224,255,255,255,202,255,0,8,64,221,4,254,167,126,130,224,6,224,4,246,255,255,127,118,4,0,0,248,197,253,255,254,31,56,1,0,0,0,18,250,255,255,255,207,19,0,0,64,223,244,255,219,47,0,40,0,80,45,240,255,255,255,111,21,0,0,0,246,246,255,255,127,3,1,0,0,136,106,219,251,255,236,70,64,45,0,124,210,255,105,255,137,85,0,0,1,154,208,255,255,247,255,173,8,0,0,128,87,234,254,255,255,143,74,4,0,0,208,186,118,255,95,17,108,37,16,104,181,189,93,181,255,84,73,144,45,0,160,212,170,182,255,127,244,159,80,137,0,86,128,91,177,255,87,255,141,42,65,72,0,176,21,218,247,182,147,180,183,0,54,149,171,214,255,85,117,74,82,1,160,4,88,142,212,255,183,251,183,242,32,120,0,232,5,97,251,187,171,237,47,85,34,32,141,0,84,173,123,185,111,45,73,138,170,164,169,221,41,253,31,213,115,84,5,146,5,80,137,96,165,235,187,248,255,96,255,3,248,1,224,4,240,13,231,170,239,63,244,31,162,78,2,152,32,140,82,223,113,127,41,253,18,170,18,228,52,29,223,244,239,152,46,16,163,32,148,40,78,85,255,116,93,43,223,17,84,43,138,42,75,138,127,217,227,63,112,225,7,136,151,0,94,28,248,207,242,255,128,30,58,208,18,130,174,233,56,63,30,215,15,105,195,168,19,197,139,161,216,57,170,109,201,174,45,202,170,4,85,105,218,58,189,82,85,109,84,41,173,42,166,87,216,118,18,250,85,252,56,138,11,227,212,131,186,240,30,149,62,82,37,85,84,21,135,109,164,214,149,220,39,84,159,212,170,37,240,145,44,39,85,211,94,177,211,89,178,108,69,106,37,57,77,212,171,169,186,138,181,18,181,149,154,74,181,170,170,149,234,54,85,147,74,182,18,165,173,42,187,74,213,86,150,170,168,106,165,180,150,37,245,170,21,167,163,152,116,140,71,89,172,43,135,115,90,165,174,104,85,150,149,202,74,213,218,162,220,138,234,168,42,156,197,203,82,237,168,181,56,181,150,106,85,82,149,170,169,82,117,98,154,170,78,101,139,151,216,85,117,44,181,137,219,168,83,220,84,92,5,215,98,227,42,245,152,42,29,101,197,169,218,160,173,28,171,74,167,212,226,37,86,77,197,75,242,100,75,174,106,203,150,218,202,74,91,81,149,28,86,162,99,37,117,69,87,149,220,10,189,84,219,74,173,84,149,85,85,181,42,181,139,236,10,118,146,180,73,211,138,105,83,217,170,100,171,146,171,138,181,210,170,150,101,165,149,85,169,77,43,43,117,216,113,226,216,81,92,154,170,177,197,70,157,106,113,105,58,139,19,151,78,78,23,174,22,211,133,29,135,142,163,108,81,86,43,122,104,58,53,241,226,40,125,232,161,243,248,176,14,226,139,227,199,81,98,58,54,188,56,126,20,197,135,13,199,54,92,56,142,11,199,143,227,184,112,252,56,14,28,60,79,28,58,126,140,56,78,29,59,46,28,199,241,225,56,142,146,199,14,158,135,142,227,232,113,28,60,142,13,23,142,31,27,78,28,63,14,28,58,117,92,56,126,28,184,116,252,56,116,28,27,46,28,61,142,226,56,126,28,135,116,242,56,122,188,56,116,28,56,142,243,248,113,232,56,196,227,195,241,227,192,193,227,195,113,240,56,113,225,248,193,227,216,49,225,71,89,30,170,142,158,56,46,233,248,112,224,90,113,156,90,241,225,56,142,30,7,94,28,199,142,23,199,133,227,194,142,23,199,129,227,26,78,158,60,46,92,56,126,28,248,113,226,194,113,244,56,112,252,56,118,28,58,122,240,56,120,92,197,113,225,56,62,28,60,46,28,63,120,92,195,133,179,106,121,28,45,143,227,196,137,163,195,142,11,199,113,108,170,142,227,194,241,225,56,118,28,199,240,227,232,113,144,165,142,227,152,116,28,31,142,227,196,113,226,248,113,108,56,142,233,224,177,227,194,113,225,196,241,227,224,113,226,56,142,139,199,113,236,208,113,236,56,142,163,227,112,92,21,143,131,199,177,19,31,142,163,7,79,28,39,142,31,199,161,227,184,176,227,224,209,30,135,142,227,197,113,236,56,134,22,47,248,226,168,112,252,56,142,31,199,113,232,56,46,28,39,142,227,248,112,156,56,142,15,71,135,99,21,55,156,170,122,224,248,113,180,56,142,227,199,33,177,56,206,30,71,139,227,232,81,14,199,143,131,199,209,226,56,54,28,199,209,227,56,116,14,199,113,28,47,142,227,196,113,28,61,142,15,199,113,28,58,142,78,120,113,92,56,142,171,199,138,139,135,142,163,197,197,13,199,197,13,39,143,86,248,112,92,56,126,236,24,120,92,58,142,23,199,137,22,59,142,233,56,113,28,61,142,19,219,112,226,184,120,92,90,57,92,236,112,241,56,78,28,199,143,227,24,120,28,199,201,227,56,86,30,199,193,227,56,116,28,151,120,236,232,113,224,248,113,225,56,112,28,199,241,227,184,112,252,56,120,28,58,142,131,199,37,28,231,139,227,194,139,227,196,113,14,199,177,227,24,199,13,199,161,227,56,142,15,199,161,227,197,177,227,56,122,28,135,142,227,224,113,244,56,137,227,216,113,92,197,177,227,216,113,244,56,112,236,216,113,224,56,113,252,232,209,227,224,113,232,184,168,227,196,209,67,199,113,234,56,142,147,108,85,156,60,142,30,135,170,30,199,137,19,61,14,29,199,135,227,88,113,28,199,142,169,58,116,252,56,112,92,56,123,28,42,143,241,56,120,28,58,62,156,56,142,227,197,113,188,56,86,236,56,112,188,60,142,233,184,112,124,184,112,226,210,177,227,216,113,172,152,116,225,248,112,124,56,78,28,59,142,30,99,113,188,56,118,232,232,112,236,56,166,233,208,113,241,24,117,156,211,138,227,194,241,225,88,113,114,234,88,170,210,89,118,56,142,30,35,55,156,26,87,28,59,142,142,101,113,150,45,142,142,197,113,242,56,58,108,85,197,99,195,113,226,120,113,92,60,90,86,58,86,86,220,112,236,152,86,105,106,89,156,58,198,13,167,166,86,154,198,149,229,88,106,21,143,149,101,165,177,149,54,150,101,165,83,60,54,150,149,78,156,90,169,234,216,112,234,152,166,86,218,177,226,104,169,113,195,201,169,195,165,163,197,165,105,101,169,169,165,142,19,199,139,99,85,85,105,197,90,170,101,165,169,101,165,233,232,112,113,90,89,150,90,89,113,234,112,105,154,166,149,197,165,163,101,89,106,149,90,86,154,90,105,106,169,106,149,86,142,149,166,149,149,170,170,170,166,150,101,89,234,216,112,106,165,170,169,165,166,149,85,165,150,149,166,149,90,165,170,85,85,86,154,90,86,106,89,105,154,90,105,85,169,169,85,85,169,165,170,169,101,165,170,90,89,85,90,89,106,170,86,169,90,89,86,85,89,105,106,89,150,170,166,170,85,85,86,85,89,85,165,90,169,170,166,86,85,165,86,149,170,106,149,170,170,170,166,170,85,89,149,170,86,85,85,85,105,85,165,85,149,90,86,106,89,105,106,85,85,85,165,85,165,170,106,170,170,86,90,85,85,85,165,170,170,170,170,170,90,106,85,85,85,85,169,85,106,85,170,85,85,149,85,85,105,106,85,85,154,86,149,165,170,106,106,85,150,149,86,150,170,90,169,170,166,170,154,166,86,165,170,149,170,90,170,85,165,85,154,90,85,85,85,85,170,90,169,170,170,169,165,90,86,170,90,85,101,165,85,149,86,85,165,90,86,106,85,105,165,170,149,85,169,85,165,170,86,149,84};

static void note_on(uint8_t k, uint8_t v);
static void note_off(uint8_t k);

#define N_NOTES 5
#define N_OSC 3
#define N_SELECTOR_POSITIONS 12

#define NO_NOTE 255
#define NO_OSC 255

#define NO_GLIDE_INCREMENT 10000
#define WHITE_NOISE_PITCH 84

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

enum wave {
    WAVE_PULSE,
    WAVE_NOISE,
    WAVE_SAMPLE,
};

static uint8_t mod = 0;

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

static void adsr_configure_percussion(struct adsr_config *config, int16_t high_value, int16_t d_time, int16_t low_value) {
    config->initial_value = low_value;
    config->a_value = high_value;
    config->s_value = low_value;
    config->final_value = low_value;

    config->a_incr = high_value - low_value; // Instant attack
    config->d_incr = (low_value - high_value) / d_time;
    if (config->d_incr == 0) {
        config->d_incr = low_value > high_value ? 1 : -1;
    }
    config->r_incr = config->d_incr;
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
            state->output = config->initial_value;
            break;
        case ADSR_SUSTAIN:
            state->output = config->s_value;
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

static void triangle_configure_constant(struct triangle_config *config, int16_t value) {
    config->low = value;
    config->high = value;
    config->initial_value = value;
    config->increment = 0;
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

struct glide_config {
    int16_t increment;
};

struct glide_state {
    int16_t last_pitch;
    uint8_t last_gate;
};

static void glide_configure(struct glide_config *config, int16_t increment) {
    config->increment = increment;
}

static void glide_init(struct glide_state *state, const struct glide_config *config) {
    (void)config;
    state->last_gate = 0;
}

static int16_t glide_update(struct glide_state *state, const struct glide_config *config, int16_t target_pitch, uint8_t gate) {
    if (gate && gate != state->last_gate) {
        state->last_pitch = target_pitch;
    } else {
        if (state->last_pitch < target_pitch) {
            if (state->last_pitch > target_pitch - config->increment) {
                state->last_pitch = target_pitch;
            } else {
                state->last_pitch += config->increment;
            }
        } else {
            if (state->last_pitch - config->increment < target_pitch) {
                state->last_pitch = target_pitch;
            } else {
                state->last_pitch -= config->increment;
            }
        }
    }
    state->last_gate = gate;
    return state->last_pitch;
}

struct sample_config {
    const uint8_t *data;
    uint16_t length;
    uint16_t period;
    int8_t increment;
};

struct sample_state {
    int8_t out;
    uint16_t t;
};

static void sample_configure(struct sample_config *config, const uint8_t *data, uint16_t length, uint16_t period, int8_t increment) {
    config->data = data;
    config->length = length;
    config->period = period;
    config->increment = increment;
}

static void sample_init(struct sample_state *state, const struct sample_config *config) {
    (void)config;
    state->out = 0;
    state->t = 0;
}

static bool sample_finished(struct sample_state *state, const struct sample_config *config) {
    return state->t >= config->length;
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
    uint8_t t8;

    bool chromatic;
    uint8_t triggered_note;
    uint8_t triggered_velocity;

    enum wave wave;

    struct adsr_config amp_env_config;
    struct triangle_config amp_lfo_config;
    struct adsr_config pitch_env_config;
    struct triangle_config pitch_lfo_config;
    struct adsr_config pwm_env_config;
    struct triangle_config pwm_lfo_config;
    struct glide_config glide_config;
    struct sample_config sample_config;

    struct adsr_state amp_env;
    struct triangle_state amp_lfo;
    struct adsr_state pitch_env;
    struct triangle_state pitch_lfo;
    struct adsr_state pwm_env;
    struct triangle_state pwm_lfo;
    struct glide_state glide;
    struct sample_state sample;

    // Output stuff
    int8_t amplitude;
};

#define MIN_PER 300

static struct global_config {
    enum mode mode;
    struct arp_config arp_config;
    int16_t note_offset;
    uint8_t prescaler;
} global_config;

static struct global_state {
    struct osc_state osc[N_OSC];
    int8_t mixer_inputs[N_OSC];
    struct arp_state arp;
} global;

// Adjust the range of the instrument to include more low notes or more high notes
static void global_config_adjust_range(int8_t octaves) {
    global_config.note_offset = (int16_t)octaves * 12;

    if (octaves >= 0) {
        global_config.prescaler = LOOKUP_COUNTS_PER_PERIOD >> octaves;
    } else {
        global_config.prescaler = LOOKUP_COUNTS_PER_PERIOD << (-octaves);
    }
}

static bool osc_handle_timer_pulse(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // For a pulse wave, this function
    // returns the output signal level
    // and updates next_period with the time until this function should be called next

    state->prescaler++;
    if (state->prescaler >= global_config.prescaler) {
        state->prescaler = 0;
    } else {
        return false;
    }

    *next_period = (state->t8 & 1) ? state->timer_period_high : state->timer_period_low;
    *out = (state->t8 & 1) ? state->amplitude : -state->amplitude;
    state->t8++;
    return true;
}

static bool osc_handle_timer_noise(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // For a noise wave, this function
    // returns the output signal level
    // and updates next_period with the time until this function should be called next

    *next_period = (state->t8 & 1) ? state->timer_period_high : state->timer_period_low;
    *out = (rand8() & 1) ? state->amplitude : -state->amplitude;
    state->t8++;
    return true;
}

static bool osc_handle_timer_sample(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // For a pulse wave, this function
    // returns the output signal level
    // and updates next_period with the time until this function should be called next

    uint16_t byte_index = state->sample.t >> 3;
    uint16_t bit_index = state->sample.t & 7;

    if (state->sample.t >= state->sample_config.length) {
        *out = 0;
        *next_period = 1250;
        return true;
    }

    if (state->sample_config.data[byte_index] & (1 << bit_index)) {
        state->sample.out += state->sample_config.increment;
    } else {
        state->sample.out -= state->sample_config.increment;
    }

    state->sample.t++;

    *out = state->sample.out;
    *next_period = state->sample_config.period;
    return true;
}

static uint16_t period(int16_t note) {
    int16_t adj_note = note - (global_config.note_offset << 8);
    // Set osc as per new note
    if (adj_note < 0) {
        adj_note = 0;
    }
    if (adj_note > (72 << 8)) {
        adj_note = (72 << 8);
    }
    uint8_t ix = adj_note >> 8;
    uint16_t per_low = PERIOD_LOOKUP[ix];
    uint16_t per_high = PERIOD_LOOKUP[ix + 1];
    uint8_t frac = adj_note & 0xFF;
    uint16_t per = ((uint32_t)per_low * (256 - frac) + (uint32_t)per_high * frac) >> 8;
    return per;
}

static void global_osc_init() {
    for (int i = 0; i < N_OSC; i++) {
        global.osc[i].wave = WAVE_PULSE;
        global.osc[i].chromatic = true;
        global.osc[i].prescaler = 0;
        global.osc[i].timer_period_high = 1250;
        global.osc[i].timer_period_low = 1250;
        global.osc[i].t8 = 0;
        global.osc[i].amplitude = 0;
        adsr_init(&global.osc[i].amp_env, &global.osc[i].amp_env_config);
        triangle_init(&global.osc[i].amp_lfo, &global.osc[i].amp_lfo_config);
        adsr_init(&global.osc[i].pitch_env, &global.osc[i].pitch_env_config);
        triangle_init(&global.osc[i].pitch_lfo, &global.osc[i].pitch_lfo_config);
        adsr_init(&global.osc[i].pwm_env, &global.osc[i].pwm_env_config);
        triangle_init(&global.osc[i].pwm_lfo, &global.osc[i].pwm_lfo_config);
        glide_init(&global.osc[i].glide, &global.osc[i].glide_config);
        sample_init(&global.osc[i].sample, &global.osc[i].sample_config);
    }
}

static void patch_trigger();

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
    note_osc[0] = 0;

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

    // First, look for osctillators matching the note
    for (int i = 0; i < N_OSC; i++) {
        if (osc_note_index[i] == note_index[0]) {
            osc_index = i;
            goto found;
        }
    }

    // First, look for completely quiet oscillators
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
    // Find a suitable oscillator to use for this new note
    note_index[0] = k;
    note_velocity[0] = v;
    assign_osc_poly();

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

    patch_trigger(note_index[0], note_osc[0]);
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

    // Call this before enabling the timer interrupts
    // because otherwise the global prescaler will be 0
    // which will deadlock the timer ISRs
    update_modulation();

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

static void init_empty() {
    global_config.mode = MODE_POLY;

    global_config_adjust_range(2); // Two octaves up

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        osc->wave = WAVE_PULSE;

        glide_configure(&osc->glide_config, NO_GLIDE_INCREMENT);
        adsr_configure(&osc->pwm_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure_constant(&osc->pwm_lfo_config, 0);

        adsr_configure(&osc->amp_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure_constant(&osc->amp_lfo_config, 0);

        adsr_configure(&osc->pitch_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure_constant(&osc->pitch_lfo_config, 0);

        sample_configure(&osc->sample_config, NULL, 0, 0, 0);
    }

    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_empty() {
}

static void trigger_empty(uint8_t k, uint8_t osc_index) {
    (void)k;
    (void)osc_index;
}


static void init_basic() {
    global_config.mode = MODE_POLY;

    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 100, 0);
        osc->amp_env_config.s_value = 0;

        // Add some vibrato
        const int16_t et = 6000;
        const int16_t ev = 6000;
        adsr_configure(&osc->pitch_env_config, 0, et, ev, 1, ev, 1, 0);
        const int16_t ot = 400;
        const int16_t ov = 200;
        triangle_configure(&osc->pitch_lfo_config, -ov, ov, 0, ot);
    }
}

static void mod_basic() {
    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Decay based on depth knob
        osc->amp_env_config.d_incr = -(knobs.depth < 127 ? ((int16_t)(127 - knobs.depth) * (127 - knobs.depth) >> 6) + 16 : (255 - knobs.depth) >> 3);
        osc->amp_env_config.s_value = knobs.depth < 255 ? 0 : 64 << 8;

        // Vibrato speed and amount based on freq knob
        int16_t vibrato_amt = knobs.freq << 4;
        osc->pitch_lfo_config.low = -vibrato_amt;
        osc->pitch_lfo_config.high = vibrato_amt;
        osc->pitch_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 11;
    }
}

static void init_chorus() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Basic amplitude envelope
        adsr_configure(&osc->amp_env_config, 0, 1, 127 << 8, 1000, 63 << 8, 100, 0);
    }
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
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Constant PWM
        triangle_configure(&osc->pwm_lfo_config, -120 << 8, 120 << 8, -120 << 8, 800);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 100, 64 << 8, 1, 64 << 8, 100, 0);
    }
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
    global_config_adjust_range(1);
    global_config.mode = MODE_MONO;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 100, 0);

        // Glide set by freq knob
        glide_configure(&osc->glide_config, 1);
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Pulse width based on depth knob
        if (i == 0) {
            osc->pwm_env_config.initial_value = (int16_t)(255 - knobs.depth) << 7;
            osc->pwm_env_config.a_value = osc->pwm_env_config.initial_value;
            osc->pwm_env_config.s_value = osc->pwm_env_config.initial_value;
            osc->pwm_env_config.final_value = osc->pwm_env_config.initial_value;
        }

        osc->glide_config.increment = 64 - (knobs.freq >> 2);
    }
}

static void init_wobble_bass() {
    global_config_adjust_range(1);
    global_config.mode = MODE_OCTAVE;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // No glide
        glide_configure(&osc->glide_config, NO_GLIDE_INCREMENT);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 300, 0);

        // No noise channel
        if (i == 2) {
            adsr_configure(&osc->amp_env_config, 0, 1, 0, 1, 0, 1, 0);
        }
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_wobble_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // No glide
        glide_configure(&osc->glide_config, NO_GLIDE_INCREMENT);

        if (i == 0 || i == 1) {
            // Vibrato speed and amount based on freq knob
            int16_t vibrato_amt = knobs.freq << 4;
            osc->pitch_lfo_config.low = -vibrato_amt;
            osc->pitch_lfo_config.high = vibrato_amt;
            osc->pitch_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 11;
        }

        if (i == 0) {
            // PWM based on depth knob
            int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
            osc->pwm_lfo_config.low = -pwm_amt;
            osc->pwm_lfo_config.high = pwm_amt;
            osc->pwm_lfo_config.increment = knobs.depth >> 1;
        }

        if (i == 1) {
            // Bring in lower octave with depth knob
            uint16_t vol = knobs.depth < 64 ? knobs.depth : 64;
            osc->amp_env_config.a_value = vol << 8;
            osc->amp_env_config.s_value = vol << 8;
        }

        // Tremolo (wobble)
        int16_t tremolo_amt = (knobs.freq < 127 ? knobs.freq : 127) << 7;
        osc->amp_lfo_config.low = -tremolo_amt;
        osc->amp_lfo_config.high = tremolo_amt;
        osc->amp_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 8;

        if (i == 2) {
            // Noise channel
        }
    }
}

static void init_pluck_bass() {
    global_config_adjust_range(1);
    global_config.mode = MODE_OCTAVE;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 127 << 8, 300, 64 << 8, 100, 0);
        osc->amp_env_config.s_value = 0;

        // Pluck PWM
        if (i == 0) {
            adsr_configure(&osc->pwm_env_config, 127 << 8, 1, 127 << 8, 300, 64 << 8, 100, 0);
            osc->pwm_env_config.s_value = 0;
        }
    }
}

static void mod_pluck_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // PWM depth based on depth knob
        if (i == 0) {
            osc->pwm_env_config.initial_value = (int16_t)(knobs.depth) << 7;
            osc->pwm_env_config.a_value = osc->pwm_env_config.initial_value;
        }

        // Decay based on freq knob
        uint16_t base_amt = 255 - knobs.freq;
        uint16_t amp_amt = (base_amt * 255) >> 10;
        uint16_t pwm_amt = (base_amt * knobs.depth) >> 10;
        osc->amp_env_config.d_incr = -(int16_t)amp_amt - 1;
        osc->amp_env_config.r_incr = -(int16_t)amp_amt - 1;
        osc->pwm_env_config.d_incr = -(int16_t)pwm_amt - 1;
        osc->pwm_env_config.r_incr = -(int16_t)pwm_amt - 1;
    }
}

static void init_drums() {
    global_config.mode = MODE_MONO; // Not enough CPU to do poly drums (yet?)
    global_config_adjust_range(2);

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        osc->wave = WAVE_NOISE;
        osc->chromatic = false;

        // Turn on the pitch triangle wave at a constant 127
        // since this is multiplied by the pitch envelope.
        // This allows the pitch envelope to do its thing
        triangle_configure_constant(&osc->pitch_lfo_config, 127 << 8);
    }
}

static void trigger_drums(uint8_t k, uint8_t osc_index) {
    struct osc_state *osc = &global.osc[osc_index];

    switch (k) {
        case 35:
        case 36:
            // KICK DRUM
            // Basic amplitude envelope
            // and weep down the pitch of the noise very quickly
            adsr_configure_percussion(&osc->amp_env_config, 127 << 8, 400, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 100 << 8, 50, 0);
            break;
        case 38:
        case 40:
            // SNARE DRUM
            // Basic amplitude envelope
            // and sweep down the pitch
            adsr_configure_percussion(&osc->amp_env_config, 127 << 8, 400, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 100 << 8, 50, 75 << 8);
            break;
        case 42:
            // CLOSED HI-HAT
            // Basic amplitude envelope
            adsr_configure_percussion(&osc->amp_env_config, 127 << 8, 30, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 100 << 8, 1, 100 << 8);
            break;
        default:
            // No sound
            adsr_configure_percussion(&osc->amp_env_config, 0, 1, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 0, 1, 0);
            break;
    }

    adsr_init(&osc->amp_env, &osc->amp_env_config);
    adsr_init(&osc->pitch_env, &osc->pitch_env_config);
}

static void init_sample() {
    global_config.mode = MODE_MONO; // Not enough CPU to do poly samples

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        osc->wave = WAVE_SAMPLE;
    }
}

static void osc_sample_play(struct osc_state *state, const uint8_t *data, uint16_t length, uint16_t period, int8_t increment) {
    sample_configure(&state->sample_config, data, length, period, increment);
    sample_init(&state->sample, &state->sample_config);
    state->amplitude = 1; // Mark sample as playing
}

static void trigger_sample(uint8_t k, uint8_t osc_index) {
    struct osc_state *osc = &global.osc[osc_index];

    if (k >= 36 && k <= 48) {
        uint16_t period = 350 + (48 - k) * 10;
        osc_sample_play(osc, BASS, sizeof (BASS) * 8, period, 4);
    }
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
    void (*trigger_fn)(uint8_t k, uint8_t osc_index);
} patches[N_SELECTOR_POSITIONS] = {
    {
        .init_fn = init_basic,
        .mod_fn = mod_basic,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_chorus,
        .mod_fn = mod_chorus,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_wave,
        .mod_fn = mod_wave,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_bass,
        .mod_fn = mod_bass,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_wobble_bass,
        .mod_fn = mod_wobble_bass,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_empty, // XXX hardware bug: skip this slot
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_pluck_bass,
        .mod_fn = mod_pluck_bass,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_drums,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_drums,
    },
    {
        .init_fn = init_sample,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_sample,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
};

static void patch_init() {
    init_empty();
    patches[knobs.select].init_fn();
}

static void patch_mod() {
    patches[knobs.select].mod_fn();
}

static void patch_trigger() {
    patches[knobs.select].trigger_fn(note_index[0], note_osc[0]);
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
        uint8_t note = osc->chromatic ? osc_note_index[i] : 0;
        uint8_t velocity = osc_note_velocity[i];

        // Store triggered velocity
        uint8_t gate = 0;
        if (velocity > 0) {
            osc->triggered_note = note;
            osc->triggered_velocity = velocity;
            gate = note + 1;
        }

        if (osc->wave == WAVE_SAMPLE) {
            if (sample_finished(&osc->sample, &osc->sample_config)) {
                // Mark sample as finished
                osc->amplitude = 0;
            }
        } else {
            // Advance pwm LFO
            uint16_t pwm_lfo_value = triangle_update(&osc->pwm_lfo, &global.osc[i].pwm_lfo_config, notes_held);

            // Advance amplitude LFO
            uint16_t amp_lfo_value = 32768 + (uint16_t)triangle_update(&osc->amp_lfo, &global.osc[i].amp_lfo_config, notes_held);

            // Advance PWM envelope generator
            int16_t pwm_env_value = adsr_update(&osc->pwm_env, &osc->pwm_env_config, gate);

            // Advance amplitude envelope generator
            int16_t amp_env_value = adsr_update(&osc->amp_env, &osc->amp_env_config, gate);

            // Advance pitch LFO
            int16_t pitch_lfo_value = triangle_update(&osc->pitch_lfo, &global.osc[i].pitch_lfo_config, notes_held);

            // Advance pitch envelope generator
            int16_t pitch_env_value = adsr_update(&osc->pitch_env, &osc->pitch_env_config, gate);

            // Apply pitch glide
            int16_t glided_pitch = glide_update(&osc->glide, &osc->glide_config, (osc->triggered_note + arp_note) << 8, notes_held);
            int16_t pitch_adjust = ((int32_t)pitch_env_value * pitch_lfo_value) >> 15;

            // Calculate timer periods
            uint16_t output_period = period(glided_pitch + pitch_adjust);

            if (output_period < 2 * MIN_PER) {
                output_period = 2 * MIN_PER;
            }
            uint16_t pulse_width_multiplier = 32768 + (uint16_t)(pwm_env_value + pwm_lfo_value);
            uint16_t per1 = ((uint32_t)output_period * pulse_width_multiplier) >> 16;
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
    switch (global.osc[0].wave) {
        case WAVE_PULSE:
            if (osc_handle_timer_pulse(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
                update_output();
            }
            break;
        case WAVE_NOISE:
            if (osc_handle_timer_noise(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
                update_output();
            }
            break;
        case WAVE_SAMPLE:
            if (osc_handle_timer_sample(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
                update_output();
            }
            break;
    }
}

ISR (TCB0_INT_vect) 
{
    TCB0.INTFLAGS |= TCB_CAPT_bm;
    switch (global.osc[1].wave) {
        case WAVE_PULSE:
            if (osc_handle_timer_pulse(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
                update_output();
            }
            break;
        case WAVE_NOISE:
            if (osc_handle_timer_noise(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
                update_output();
            }
            break;
        case WAVE_SAMPLE:
            if (osc_handle_timer_sample(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
                update_output();
            }
            break;
    }
}

ISR (TCB1_INT_vect) 
{
    TCB1.INTFLAGS |= TCB_CAPT_bm;
    switch (global.osc[2].wave) {
        case WAVE_PULSE:
            if (osc_handle_timer_pulse(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
                update_output();
            }
            break;
        case WAVE_NOISE:
            if (osc_handle_timer_noise(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
                update_output();
            }
            break;
        case WAVE_SAMPLE:
            if (osc_handle_timer_sample(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
                update_output();
            }
            break;
    }
}
