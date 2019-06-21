#include <stdio.h>
#include <stdlib.h>

#define BIRB_SHORTHAND
#include "birb.h"

// C Operator Precedence:
// x++ [] ()
// ++x -x ! ~
// * / %
// + -
// << >>
// < <= => >
// == !=
// &
// ^
// |

// Credits to https://www.youtube.com/watch?v=tCRPUv8V22o
static const struct program {
    const char *name;
    const unsigned char data[256];
} programs[] = {
    {
        .name = "rand(t)|(t*5&t>>5)",
        .data = {
            T, RND,
            T, 5, MUL,
            T, 5, SHR,
                AND,
                    OR,
            END,
        },
    },
    {
        .name = "(t*5&t>>7)|(t*3&t>>10)",
        .data = {
            T, 5, MUL,
            T, 7, SHR,
                AND,
            T, 3, MUL,
            T, A, SHR,
                AND,
                    OR,
            END,
        },
    },
    {
        .name = "(t*9&t>>4|t*5&t>>7|t*3&t/1024)-1",
        .data = {
            T, 9, MUL,
            T, 4, SHR,
                AND,
            T, 5, MUL,
            T, 7, SHR,
                AND,
            T, 3, MUL,
            T, A, SHR,
                AND,
            OR, OR, 1, SUB,
            END,
        },
    },
    {
        .name = "(t&t%255)-(t*3&t>>13&t>>6)",
        .data = {
            T, T, F, F, DIG, MOD, AND,
            T, 3, MUL,
            T, D, SHR,
            T, 6, SHR,
                AND, AND,
                    SUB,
            END,
        },
    },
    {
        .name = "(t/1e7*t*t+t)%127 |t>>4|t>>5|t%127+(t>>16)|t",
        .data = {
            T, T, T, T, // Not faithful without floating point
                MUL, MUL, MUL,
            0, 8, 6, 9, 8, 9, // 0x989680 == 1e7
                DIG, DIG, DIG, DIG, DIG,
                    DIV,
            0xf, 7, DIG, // 127
                MOD,
            T, 4,
                SHR, OR,
            T, 5,
                SHR, OR,
            T, 0xf, 7, DIG, MOD,
            T, 0, 1, DIG, SHR,
                ADD,
                OR,
            T,
                OR,
            END,
        },
    },
    /* this one is broken 
    {
        .name = "((t/2*(15&(0x234568a0>>(t>>8^28))))|t/2>>(t>>11)^t>>12)+(t/16&t&24)",
        .data = {
            0, A, 8, 6, 5, 4, 3, 2,
                DIG, DIG, DIG, DIG, DIG, DIG, DIG,
            T, 8, SHR,
                C, 1, DIG, XOR,
                    SHR,
                        F, AND,
            T, 2, DIV,
                MUL,
            T, 2, DIV,
            T, B, SHR,
                SHR,
            T, C, SHR,
                XOR,
                    OR,
            T, 0, 1, DIG, DIV,
            T, AND, 8, 1, DIG, AND,
                ADD,
            END,
        },
    }, */
    { 0 },
};

#define RANDOM_LEN 32
unsigned char *random_program(unsigned int seed) {
    // These usually aren't that good...
    static unsigned char data[RANDOM_LEN+1];
    srand(seed);
    while (1) {
        for (int i = 0; i < RANDOM_LEN; i++) {
            data[i] = (unsigned char) (rand() & 0x1F);
        }
        data[RANDOM_LEN] = END;
        // Require time dependence
        if (birb_eval(data, (birb_t) rand(), 0) != birb_eval(data, (birb_t) rand(), 0)) {
            return data;
        }
    }
}

int main() {
    // viznut
    for (const struct program * program = programs; program->name != NULL; program++) {
        fprintf(stderr, "%s\n", program->name);
        for (birb_t t = 0; t < 8196 * 10; t++) { // 10 seconds-ish
            unsigned char val = (unsigned char) birb_eval(program->data, t, 0);
            putchar(val);
        }
        for (birb_t t = 0; t < 4096; t++) putchar(0);
    }

    for (unsigned int i = 0; i < 10; i++) {
        unsigned char *data = random_program(i);
        for (birb_t t = 0; t < 8196 * 2; t++) { // 2 seconds-ish
            unsigned char val = (unsigned char) birb_eval(data, t, 0);
            putchar(val);
        }
        for (birb_t t = 0; t < 4096; t++) putchar(0);
    }
    return 0;
}
