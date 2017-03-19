/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "uart.h"
#include "lib.h"

#define ENTER_KEY 13
#define CMD_LEN 255
#define CMD_DESC 255

class console *c;

struct cmd_hash {
	char cmd_name[CMD_LEN];
	int (*cmd)(uint32_t);
	char cmd_desc[CMD_LEN];
};

int help(uint32_t arg);

struct cmd_hash cmds[] = {
	{"help", help, "prints this help information"}
};

int help(uint32_t arg)
{
	c->cprintf("List of functions:\n");
	for (int i = 0; i < (int) ARRAY_SIZE(cmds); i++) {
		c->cprintf("%s - %s\n", cmds[i].cmd_name, cmds[i].cmd_desc);
	}
	return 0;
}

void execute_cmd(char *cmd, uint32_t arg)
{
	for (int i = 0; i < (int) ARRAY_SIZE(cmds); i++) {
		if (!memcmp(cmd, cmds[i].cmd_name, strlen(cmds[i].cmd_name)))
			cmds[i].cmd(arg);
	}
}

static inline void delay(int32_t count)
{
	        asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
				                        : : [count]"r"(count) : "cc");
}

static void clock_setup(void)
{
	/* Enable GPIOD clock for LED & USARTs. */
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART2);
}

int main(void)
{
	clock_setup();
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	stm32_uart_device uart;
	uart.init(USART2, 115200, 8, USART_STOPBITS_1, USART_PARITY_NONE,
		USART_FLOWCONTROL_NONE);
	c = (class console *)&uart;

	while (1) {
		char cmd[CMD_LEN];
		int inc_c_idx = 0;
		unsigned char in_c;
		c->cprintf("> ");
		do {
			in_c = c->getc();
			gpio_toggle(GPIOD, GPIO12);
			delay(5000);
			gpio_toggle(GPIOD, GPIO12);
			cmd[inc_c_idx++] = in_c;
			c->putc(in_c);
		} while (in_c != ENTER_KEY);
		cmd[inc_c_idx - 1] = 0x0;
		execute_cmd(cmd, 0);
		c->cprintf("\n");
	}

	return 0;
}
