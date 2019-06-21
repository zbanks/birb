# birb
smol stack synthesizer for embedded systems, single-header, based on work by viznut et al.

# Running the demo

    $ make music

# Documentation

`birb` programs are arrays of values between `0x00` and `0x1F` (5 bits), terminated with `0xFF` (`BIRB_END`). 

Programs operate on a stack, making the programs very RPN-like. Popping a value from an empty stack yields `0`, and pushing a value onto a full stack overwrites the top value. The default maximum stack size is `64` but can be customized with `BIRB_STACK_SIZE`.

When evaluating programs, there is a special `T` value set which can be referenced from the birb program. This is intended to be used to produce time-varying output for synthesizing sounds.

After hitting the end of the program, top value on the stack is popped and returned from `birb_eval` (or `0` if the stack was empty).

## Opcode Reference

Most opcodes pop 2 values from the stack (`x` then `y`) and push a single value back:
- `BIRB_SHR`: `y >> x`
- `BIRB_SHL`: `y << x`
- `BIRB_AND`: `y & x`
- `BIRB_OR`: `y | x`
- `BIRB_XOR`: `y ^ x`
- `BIRB_ADD`: `y + x`
- `BIRB_SUB`: `y - x`
- `BIRB_MUL`: `y * x`
- `BIRB_DIV`: `y / x`, or `0` if `x == 0`
- `BIRB_MOD`: `y % x`, or `0` if `x == 0`
- `BIRB_DIG`: `(x << 4) | y`. Useful for constructing larger constants, e.g. `4 3 2 1 DIG DIG DIG` yields `0x1234`

Other opcodes that do not follow the above pattern:
- `0x0` - `0xF`: Push the given opcode onto the stack
- `BIRB_T`: Push the special variable `T` onto the stack
- `BIRB_U`: Push the special variable `U` onto the stack
- `BIRB_SWP`: Swap the position of the top two values on the stack
- `BIRB_DUP`: Duplicate the top value on the stack
- `BIRB_RND`: Pop `x` from the stack, then push `hash(x)` back
