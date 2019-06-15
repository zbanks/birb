#ifndef __BIRB_H__
#define __BIRB_H__

#ifndef BIRB_STACK_SIZE
#define BIRB_STACK_SIZE 64
#endif

// Right now only unsigned integer types are supported
typedef unsigned int birb_t;

enum {
    BIRB_T = 0x10,
    BIRB_DIG,
    BIRB_SWP,
    BIRB_SHR,
    BIRB_AND,
    BIRB_OR,
    BIRB_XOR,
    BIRB_ADD,
    BIRB_SUB,
    BIRB_MUL,
    BIRB_DIV,
    BIRB_MOD,
    BIRB_DUP,
    //BIRB_ SHL? (y << x)
    //BIRB_ LTE? (y <= x)
    //BIRB_
    BIRB_END = 0xFF,
};


static inline birb_t
birb_eval(const unsigned char * p, birb_t t) {
    static birb_t stack[BIRB_STACK_SIZE];
    int si = 0; // stack index

#define PUSH(v) ({ \
        stack[si++] = (v); \
        si = (si >= BIRB_STACK_SIZE) ? BIRB_STACK_SIZE : si; \
    })
#define POP() ({ \
        int _i = (si > 0) ? (si - 1) : 0; \
        stack[si = _i]; \
    })
    
    birb_t x, y;
    for (; *p != BIRB_END; p++) {
        switch (*p & 0x1F) {
        case BIRB_T: PUSH(t); break;
        case BIRB_DIG: x = POP(); y = POP(); PUSH((x << 4) | y); break;
        case BIRB_SWP: x = POP(); y = POP(); PUSH(x); PUSH(y); break;
        //case BIRB_SHL: x = POP(); y = POP(); PUSH(y << x); break;
        case BIRB_SHR: x = POP(); y = POP(); PUSH(y >> x); break;
        case BIRB_AND: x = POP(); y = POP(); PUSH(y & x); break;
        case BIRB_OR:  x = POP(); y = POP(); PUSH(y | x); break;
        case BIRB_XOR: x = POP(); y = POP(); PUSH(y ^ x); break;
        case BIRB_ADD: x = POP(); y = POP(); PUSH(y + x); break;
        case BIRB_SUB: x = POP(); y = POP(); PUSH(y - x); break;
        case BIRB_MUL: x = POP(); y = POP(); PUSH(y * x); break;
        case BIRB_DIV: x = POP(); y = POP(); PUSH(x == 0 ? 0 : y / x); break;
        case BIRB_MOD: x = POP(); y = POP(); PUSH(x == 0 ? 0 : y % x); break;
        case BIRB_DUP: x = POP(); PUSH(x); PUSH(x); break;
        default: PUSH(*p & 0xF); break;
        }
    }

    return POP();

#undef POP
#undef PUSH
}

#ifdef BIRB_SHORTHAND

#define T   BIRB_T
#define DIG BIRB_DIG
#define SWP BIRB_SWP
#define SHL BIRB_SHL
#define SHR BIRB_SHR
#define AND BIRB_AND
#define OR  BIRB_OR
#define XOR BIRB_XOR
#define ADD BIRB_ADD
#define SUB BIRB_SUB
#define MUL BIRB_MUL
#define DIV BIRB_DIV
#define MOD BIRB_MOD
#define END BIRB_END
#define A 0xA
#define B 0xB
#define C 0xC
#define D 0xD
#define E 0xE
#define F 0xF

#endif

#endif
