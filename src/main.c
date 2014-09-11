#include "drv/chip.h"
#include "drv/clock.h"
#include "drv/iocon.h"

#include "systick.h"
#include "pwm.h"

static void fade_to(uint8_t target, uint8_t delay) {
	uint8_t cur = pwm_get();
	int8_t delta = (target > cur) ? 1 : -1;

	for(; cur != target; cur += delta) {
		pwm_set(cur);
		delay_ticks(delay);
	}

	pwm_set(target);
}

#define WOBBLE_DELTA 10
#define WOBBLE_DELAY 20
static void wobble(void) {
	uint8_t cur = pwm_get();
	int8_t delta = (cur > WOBBLE_DELTA) ? (-WOBBLE_DELTA) : WOBBLE_DELTA;

	for(uint8_t i = 0; i < 3; i++) {
		fade_to(cur + delta, WOBBLE_DELAY);
		fade_to(cur, WOBBLE_DELAY);
	}
}

static void toggle(void) {
	if(pwm_get())
		pwm_set(0);
	else
		pwm_set(PWM_MAX);
}

static void strobe(void) {
	for(uint8_t i = 0; i < 8; i++) {
		toggle();
		delay_ticks(60);
	}
}

static void gpio_init(void) {
	Chip_GPIO_Init(LPC_GPIO_PORT);

	// 0_4 -> Wake
	// 0_16 -> Misc

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	Chip_IOCON_PinSetMode(LPC_IOCON, IOCON_PIO4, PIN_MODE_PULLUP);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 4);
}

#define SENSITIVITY 50000
static bool get_switch(void) {
	static unsigned int cnt = SENSITIVITY / 2;
	while(1) {
		if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 4)) {
			if(cnt > 0)
				cnt--;
		} else if(cnt < SENSITIVITY) {
			cnt++;
		}

		if(cnt == 0)
			return 0;
		else if(cnt >= SENSITIVITY) {
			cnt = SENSITIVITY;
			return 1;
		}
	}
}

// Time to give the user to release the key
#define RELEASE_TIME 300

void main(void) {
	uint32_t limit;

	gpio_init();
	pwm_init();
	systick_init();

	pwm_set(0);

	while(1) {
wait:
		limit = 5;
		while(!get_switch())
			;

		fade_to(PWM_MAX, 5);
		delay_ticks(RELEASE_TIME);

start:
		for(uint32_t i = 0; i < limit * 60 * 100; i++) {
			uint32_t cnt = 0;
			while(get_switch()) {
				cnt++;

				if(cnt > 300) {
					limit = 20;
					strobe();

					delay_ticks(2*RELEASE_TIME);
					goto start;
				}

				delay_ticks(10);
			}

			if(cnt) {
				fade_to(0, 5);
				goto wait;
			}

			delay_ticks(10);
		}

		wobble();

		for(uint32_t i = 0; i < 30 * 100; i++) {
			uint32_t cnt = 0;
			while(get_switch()) {
				cnt++;

				if(cnt > 300) {
					limit = 20;
					strobe();

					delay_ticks(2*RELEASE_TIME);
					goto start;
				}

				delay_ticks(10);
			}

			if(cnt) {
				delay_ticks(1000);
				goto start;
			}

			delay_ticks(10);
		}

		fade_to(0, 30);
	}
}
