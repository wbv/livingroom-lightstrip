#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"

void main() {
	uint8_t status;
	register uint8_t on, off;

	cli();            // disable interrupts
	DDRB |= (1 << 5); // set port B5 to ouput

	status = PORTB;
	on  = status |  (1 << 5); // sets port B5 (LED light) high
	off = status & ~(1 << 5); // sets port B5 (LED light) Low

	while (1) {
		PORTB = on;
		_delay_ms(500);
		PORTB = off;
		_delay_ms(500);
	}

	return;
}
