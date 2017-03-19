#include <libopencm3/stm32/gpio.h>
#include "uart.h"
#include "lib.h"

struct gpio_usart_map {
	uint32_t gpio_subsys;
	uint32_t gpio_tx;
	uint32_t gpio_rx;
	uint32_t gpio_af;
};
struct gpio_usart_map usart_map[] = {
	{0, 0, 0, 0}, /*START */
	{0, 0, 0, 0}, /*USART 1*/
	{GPIOA, GPIO2, GPIO3, GPIO_AF7}, /*USART 2*/
	{0, 0, 0, 0}, /*USART 3*/
	{0, 0, 0, 0}, /*USART 4*/
};

stm32_uart_device::stm32_uart_device() : uid(0) {};

void stm32_uart_device::init(uint32_t usart_id, uint32_t baudrate, uint32_t databits,
	uint32_t stopbits, uint32_t parity, uint32_t flow_control)
{
	uid = usart_id;
	if (uid == USART2) {
		/* Setup GPIO pins for transmit. */
		gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
		/* Setup GPIO pins for recieve. */
		gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
		/* Setup TX pin as alternate function. */
		gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
		/* Setup RX pin as alternate function. */
		gpio_set_af(GPIOA, GPIO_AF7, GPIO3);

	}
	usart_set_baudrate(uid, baudrate);
	usart_set_databits(uid, databits);
	usart_set_stopbits(uid, stopbits);
	usart_set_mode(uid, USART_MODE_TX_RX);
	usart_set_parity(uid, parity);
	usart_set_flow_control(uid, flow_control);

	/* Finally enable the USART. */
	usart_enable(uid);

}

void stm32_uart_device::putc(unsigned char c)
{
	usart_send_blocking(uid, c);
}

unsigned char stm32_uart_device::getc()
{
	return (char) usart_recv_blocking(uid);
}

void stm32_uart_device::arr_write(const unsigned char* buffer, size_t size)
{
	for (size_t i = 0; i < size; i++ ) {
		if (buffer[i] == '\n')
			putc('\r');
		putc(buffer[i]);
	}
}

void stm32_uart_device::puts(const char* str)
{
	arr_write((const unsigned char*) str, strlen(str));
}
