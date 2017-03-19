#ifndef UART_H
#define UART_H

#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include <libopencm3/stm32/usart.h>

#include "console.h"

class stm32_uart_device : public console {
public:
	stm32_uart_device();
        void putc(unsigned char c);
	unsigned char getc();
	void puts(const char* str);
	void init(uint32_t usart_id, uint32_t baudrate, uint32_t databits,
		uint32_t stopbits, uint32_t parity, uint32_t flow_control);
private:
	void arr_write(const unsigned char* buffer, size_t size);
	uint32_t uid; /*USART id */
};
#endif
