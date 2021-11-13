#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BOOTEND_FUSE 0x08 // How many 256-byte pages the BL takes (0x08 = 2KB)
#define APP_START (BOOTEND_FUSE * 256) // Memory address of application, in bytes

typedef __attribute__((noreturn)) void (*const app_t)(void);
const app_t branch_to_app = (app_t)(APP_START / sizeof (app_t));

FUSES = {
    .WDTCFG = 0x00,
    .BODCFG = 0x00,
    .OSCCFG = 0x02,
    .TCD0CFG = 0x00,
    .SYSCFG0 = 0xC4,
    .SYSCFG1 = 0x07,
    .APPEND = 0x00,
    .BOOTEND = BOOTEND_FUSE,
};

#define MIDI_CHANNEL 0

static volatile uint8_t stay_in_bootloader = 0;

static void rx(uint8_t byte) {
    static enum {
        UNKNOWN,
        WAIT_MANUFACTURER_ID,
        WAIT_CUSTOM_ID_1,
        WAIT_CUSTOM_ID_2,
        WAIT_COMMAND,
        WAIT_WRITE_ADDRESS_1,
        WAIT_WRITE_ADDRESS_2,
        WAIT_WRITE_BYTE_1,
        WAIT_WRITE_BYTE_2,
        WAIT_WRITE_BYTE_3,
        WAIT_WRITE_BYTE_4,
        WAIT_WRITE_BYTE_5,
    } state = UNKNOWN;

    static uint16_t write_memory_address;
    static uint8_t bytes_written;

    if (byte == 0xFF) {
        // "Reset" message: Issue system reset
        _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
        for(;;);
    } else if (byte == 0xF0) { // Start sysex
        state = WAIT_MANUFACTURER_ID;
    } else if (byte == 0xF7) { // End sysex
        if (state == WAIT_WRITE_BYTE_1 && bytes_written == 64) { // Page size is 64 bytes
            _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
            while(NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
            PORTA.OUTSET |= PIN1_bm;
        }
        state = UNKNOWN;
    } else if (byte & 0x80) { // Unknown status message
        state = UNKNOWN;
    } else {
        // Data bytes
        switch (state) {
            case UNKNOWN:
                break;
            case WAIT_MANUFACTURER_ID:
                if (byte == 0x7D) { // Custom / prototype manufacturer ID
                    state = WAIT_CUSTOM_ID_1;
                } else {
                    state = UNKNOWN;
                }
                break;
            case WAIT_CUSTOM_ID_1:
                if (byte == 0x61) { // First byte of our custom ID
                    state = WAIT_CUSTOM_ID_2;
                } else {
                    state = UNKNOWN;
                }
                break;
            case WAIT_CUSTOM_ID_2:
                if (byte == 0x26) { // Second byte of our custom ID
                    stay_in_bootloader = 1;
                    state = WAIT_COMMAND;
                } else {
                    state = UNKNOWN;
                }
                break;
            case WAIT_COMMAND:
                switch (byte) {
                    case 0x01: // Write mem
                        state = WAIT_WRITE_ADDRESS_1;
                        break;
                    default: // 0x00 = NOOP, 0x03 = reset into BL (ignore in BL)
                        state = UNKNOWN;
                        break;
                }
                break;
            case WAIT_WRITE_ADDRESS_1:
                // First data byte is used as most significant byte of address
                // The most significant bit is assumed to be 1
                // since the flash starts at 0x8000
                write_memory_address = 0x8000 | (byte << 8);
                state = WAIT_WRITE_ADDRESS_2;
                break;
            case WAIT_WRITE_ADDRESS_2:
                // Second data byte is used as least significant byte of address
                // (after shifting left by 1 position.)
                // The least significant bit is assumed to be 0
                // since the flash is word-aligned (2-byte words)
                write_memory_address |= byte << 1;
                state = WAIT_WRITE_BYTE_1;
                bytes_written = 0;
                break;
            case WAIT_WRITE_BYTE_1:
                *(uint8_t *)write_memory_address = 0x80 | byte;
                state = WAIT_WRITE_BYTE_2;
                break;
            case WAIT_WRITE_BYTE_2:
                *(uint8_t *)write_memory_address = 0x7F | (byte << 7);
                write_memory_address++;
                *(uint8_t *)write_memory_address = 0xC0 | (byte >> 1);
                state = WAIT_WRITE_BYTE_3;
                break;
            case WAIT_WRITE_BYTE_3:
                *(uint8_t *)write_memory_address = 0x3F | (byte << 6);
                write_memory_address++;
                *(uint8_t *)write_memory_address = 0xE0 | (byte >> 2);
                state = WAIT_WRITE_BYTE_4;
                break;
            case WAIT_WRITE_BYTE_4:
                *(uint8_t *)write_memory_address = 0x1F | (byte << 5);
                write_memory_address++;
                *(uint8_t *)write_memory_address = 0xF0 | (byte >> 3);
                state = WAIT_WRITE_BYTE_5;
                break;
            case WAIT_WRITE_BYTE_5:
                *(uint8_t *)write_memory_address = 0x0F | (byte << 4);
                write_memory_address++;
                bytes_written += 4;
                state = WAIT_WRITE_BYTE_1;
                break;
        }
    }
}

ISR (USART0_RXC_vect) 
{
    //PORTA.OUTSET |= PIN1_bm;
    rx(USART0.RXDATAL);
    //PORTA.OUTCLR |= PIN1_bm;
}

static void prepare_app() {
    // Disable interrupts
    cli();    

    // Remap interrupts to application
    CCP = CCP_IOREG_gc;
    CPUINT.CTRLA = 0;

    // Unconfigure PB2
    PORTB.DIRSET = 0;
    PORTA.OUT = 0;
    PORTA.DIRSET = 0;

    // Leave PB3 configured (INVEN)
    // so that application code is agnostic to
    // transceiver polarity

    // Unconfigure USART
    USART0.BAUD = 0;
    USART0.CTRLA = 0;
    USART0.CTRLB = 0;

    // Set clock frequency back to startup default
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB = CLKCTRL_PEN_bm;
}

int
main (void)
{
    // Set internal 20 MHz clock divided by two (PDIV=0, PEN=1)
    // for a result of 10 MHz
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PDIV_gm;

    // Remap interrupts to bootloader
    CCP = CCP_IOREG_gc;
    CPUINT.CTRLA |= CPUINT_IVSEL_bm;

    // Configure USART
    // Set PB2 as output
	PORTB.DIRSET = PIN2_bm;

    // Invert PB3 input because this transceiver is weird
    PORTB.PIN3CTRL |= PORT_INVEN_bm;

    // Set PB3 pullup
    PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;

    USART0.BAUD = 1280; // 31250 baud @ 10 MHz clock
    USART0.CTRLA |= USART_RXCIE_bm; // Receive data interrupt enable
	USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;	// Enable rx and tx
    // End Configure USART

    sei();

    // Wait about half a second, blinking the light
    PORTA.DIRSET = PIN1_bm; // Set PA1 as output
    for (int i = 0; i < 5; i++) {
        PORTA.OUTSET |= PIN1_bm;
        _delay_ms(20);
        if (stay_in_bootloader) break;
        PORTA.OUTCLR |= PIN1_bm;
        _delay_ms(20);
        if (stay_in_bootloader) break;
    }

    if (stay_in_bootloader) {
        for (;;) {
            PORTA.OUTSET |= PIN1_bm;
            _delay_ms(20);
            PORTA.OUTCLR |= PIN1_bm;
            _delay_ms(20);
        }
    } else {
        prepare_app();
        branch_to_app();
    }
}
