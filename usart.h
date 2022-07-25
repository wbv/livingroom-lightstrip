#include <stdint.h>

// initializes the ATmega328p's USART0 device with the baud rate register
// setting corresponding to `ubrr` and enables the receiever in asynchronous
// normal mode. see ATmega328P datasheet chapter on USART0 for details.
void m328p_usart_init_rx_only(uint8_t ubrr_lo);
