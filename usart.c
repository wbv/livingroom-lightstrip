#include <stdint.h>
#include <avr/io.h>

#include "usart.h"

#if 0
// register layout + configuration for the USART.
// see ATmega328P datasheet chapter 19: USART0 for details.
static struct usart_cfg {
	// UCSR0A: USART Control and Status Register A
	union {
		uint8_t reg;
		struct {
			uint8_t mpcm  : 1; // Multi-processor Comm. Mode
			uint8_t u2x   : 1; // USART 2x Asynchronous Mode
			uint8_t upe   : 1; // (read-only) USART Parity Error
			uint8_t dor   : 1; // (read-only) Data Overrun
			uint8_t fe    : 1; // (read-only) Frame Error
			uint8_t udre  : 1; // (read-only) USART Data Register Empty
			uint8_t txc   : 1; // (read-only) USART TX Complete
			uint8_t rxc   : 1; // (read-only) USART RX Complete
		} bits;
	} ucsra;

	// UCSR0B: USART Control and Status Register B
	union {
		uint8_t reg;
		struct {
			uint8_t txb8  : 1; // Transmit Data Bit 8
			uint8_t rxb8  : 1; // (read-only) Receive Data Bit 8
			uint8_t ucsz2 : 1; // Character Size bit 2
			uint8_t txen  : 1; // Transmitter Enable
			uint8_t rxen  : 1; // Receiver Enable
			uint8_t udrie : 1; // USART Data Register Empty Interrupt Enable
			uint8_t txcie : 1; // TX Complete Interrupt Enable
			uint8_t rxcie : 1; // RX Complete Interrupt Enable
		} bits;
	} ucsrb;

	// UCSR0B: USART Control and Status Register C
	union {
		uint8_t reg;
		struct {
			uint8_t ucpol : 1; // Clock Polarity
			uint8_t ucsz  : 2; // Character Size bits 0:1
			uint8_t usbs  : 1; // Stop Bit Select
			uint8_t upm   : 2; // Parity Mode
			uint8_t umsel : 2; // USART Mode Select
		} bits;
	} ucsrc;

	// UBRR: USART Baud Rate Register
	// 
	//   UBRR = f_osc / (16 * baudrate) - 1
	// 
	// so for a baud rate of 9600, and our f_osc of 16 MHz,
	//
	//   UBRR = 16e6 / (16 * 9600) - 1 = 103 (ish)
	//
	struct {
		uint8_t lo : 8; // UBRR Low bits
		uint8_t hi : 4; // UBRR High bits (only four count)
	} ubrr;
} cfg = {0};
#endif

// initializes the ATmega328p's USART0 device with the baud rate register
// setting corresponding to `ubrr` and enables the receiever in asynchronous
// normal mode.
void m328p_usart_init_(uint8_t ubrr_lo) {
#if 0
	cfg.ucsra.bits.u2x = 1;
	cfg.ucsrb.bits.rxen = 1;
	cfg.ucsrc.bits.ucsz = 2;
	cfg.ubrr.lo = ubrr_lo;
	cfg.ubrr.hi = 0;

	UCSR0A = cfg.ucsra.reg;
	UCSR0B = cfg.ucsrb.reg;
	UCSR0C = cfg.ucsrc.reg;
	UBRR0H = cfg.ubrr.hi;
	UBRR0H = cfg.ubrr.lo;
#endif
#if 1
	UBRR0H = 0;
	UBRR0L = ubrr_lo;

	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0B = (1 << RXEN0);
#endif
#if 0
	uint8_t ucsr_a = 0;
	ucsr_a |= (0 << MPCM0);
	ucsr_a |= (0 << U2X0);
	ucsr_a |= (0 << UPE0);
	ucsr_a |= (1 << DOR0);
	ucsr_a |= (0 << FE0);
	ucsr_a |= (0 << UDRE0);
	ucsr_a |= (0 << TXC0);
	ucsr_a |= (0 << RXC0);

	uint8_t ucsr_b = 0;
	ucsr_b |= (0 << TXB80);  // Transmit Data Bit 8
	ucsr_b |= (0 << RXB80);  // Receive Data Bit 8
	ucsr_b |= (0 << UCSZ02); // Character Size Bit 2
	ucsr_b |= (0 << TXEN0);  // Transmitter Enable
	ucsr_b |= (1 << RXEN0);  // Receiver Enable
	ucsr_b |= (0 << UDRIE0); // USART Data Register Empty Interrupt Enable
	ucsr_b |= (0 << TXCIE0); // TX Complete Interrupt Enable
	ucsr_b |= (0 << RXCIE0); // RX Complete Interrupt Enable

	uint8_t ucsr_c = 0;
	ucsr_c |= (0 << UCPOL0);  // Clock Polarity
	ucsr_c |= (1 << UCSZ00);  // Character Size bit 0
	ucsr_c |= (1 << UCSZ01);  // Character Size bit 1
	ucsr_c |= (0 << USBS0);   // Stop Bit Select
	ucsr_c |= (0 << UPM00);   // Parity Mode bit 0
	ucsr_c |= (0 << UPM01);   // Parity Mode bit 1
	ucsr_c |= (0 << UMSEL00); // USART Mode Select bit 0
	ucsr_c |= (0 << UMSEL01); // USART Mode Select bit 1


	// UBRR: USART Baud Rate Register
	// we never run at a baud rate below 9600, so assume UBRR < 256 and
	// leave the high word of UBRR at 0.
	UBRR0H = 0;
	UBRR0L = ubrr_lo;

	UCSR0A = ucsr_a;
	UCSR0B = ucsr_b;
	UCSR0C = ucsr_c;
#endif
}
