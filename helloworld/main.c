#include "stm32f407xx.h"

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

/* Loop <delay> times in a way that the compiler won't optimize away. */
static inline void delay(int32_t count)
{
	asm volatile("mov r0, %[count]; __delay_%=: subs r0, r0, #1; bne __delay_%=\n"
			: : [count]"r"(count) : "cc");
}

void work()
{
	uint32_t timeout = 0xf0000;
	/* Enable clock for IO port D */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	/* Configuring LED ports as output IO ports */
	GPIOD->MODER |= (GPIO_MODER_MODE15_0 | GPIO_MODER_MODE14_0
			| GPIO_MODER_MODE13_0 | GPIO_MODER_MODE12_0);
	while (1) {
		GPIOD->ODR = (GPIO_ODR_OD15 | GPIO_ODR_OD14);
		delay(timeout);
		GPIOD->ODR = (GPIO_ODR_OD13 | GPIO_ODR_OD12);
		delay(timeout);
	}

}

void reset(void)
{
	/* Use symbols provided by linker to get data contents from nonvolatile memory (at end of .text) and copy to .data in RAM */
	uint32_t* nvdata = &_etext;
	uint32_t* data =  &_sdata;
	while(data < &_edata)
		*(data++) = *(nvdata++);
	/* Now zero .bss, for all global/static stuff that has no initial value */
	data = &_sbss;
	while(data < &_ebss)
		*(data++) = 0;
	work();
}

void(*vector_table[])(void) __attribute__((section(".vectors"))) = {
	(void(*)(void))&_estack,            /* stack top address */
	reset,              /* 1 Reset */
	0x0,               /* 2 NMI */
	0x0,               /* 3 HardFault */
	0x0,               /* 4 MemManage */
	0x0,               /* 5 BusFault */
	0x0,               /* 6 UsageFault */
	0x0,               /* 7 RESERVED */
	0x0,               /* 8 RESERVED */
	0x0,               /* 9 RESERVED*/
	0x0,               /* 10 RESERVED */
	0x0,               /* 11 SVCall */
	0x0,               /* 12 Debug Monitor */
	0x0,               /* 13 RESERVED */
	0x0,               /* 14 PendSV */
	0x0,		/* 15 SysTick */
	0x0,               /* 16 External Interrupt(0) */
	0x0,               /* 17 External Interrupt(1) */
	0x0,               /* 18 External Interrupt(2) */
	0x0                /* 19 */
};
