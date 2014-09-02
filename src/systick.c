#include <stdint.h>
#include "drv/chip.h"

#include "systick.h"

volatile uint32_t ticks = 0;

void systick_init(void) {
	SysTick_Config(24 * 1000 * 1000 / HZ);
}

void delay_ticks(uint16_t cnt) {
        uint32_t timeout = ticks + cnt;

        while(time_before(ticks, timeout))
                ;
}

void __attribute__((interrupt("IRQ"))) systick(void) {
	ticks++;
}
