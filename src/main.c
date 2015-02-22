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
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 10);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 11);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 16);
}

// ALPS notation
#define ROTARY_A 11
#define ROTARY_B 10
#define ROTARY_BUTTON 16

#define SENSITIVITY 50

static bool get_switch(void) {
    static unsigned int cnt = SENSITIVITY / 2;
    while(1) {
        if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, ROTARY_BUTTON)) {
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

void up(int* value, int limit) {
    if (*value < limit) {
        (*value)++;
    }
}

void down(int* value, int limit) {
    if (*value > limit) {
        (*value)--;
    }
}

#define DEBOUNCE 50
#define FADE_DELAY 1500

void main(void) {
	gpio_init();
	pwm_init();
	systick_init();

	pwm_set(0);
    // 0xa0000000

    /*
    int x = 0;
    while(1) {
        if (!Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, ROTARY_A)) {
            x++;   
        }
    }
    */

    int renc = DEBOUNCE;
    int fade_direction = 0;
    int pwm_bright = 0;
    int fade_delay_counter = 0;
	while(1) {
        if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, ROTARY_A)) {
            up(&renc, DEBOUNCE);
        } else {
            if (renc > 0) {
                --renc;
                // fallende Flanke
                if (renc == 0) {
                    if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, ROTARY_B)  == true) { // CW
                        up(&pwm_bright, PWM_MAX);
                    } else { // CCW
                        down(&pwm_bright, 0);
                    }
                    pwm_set(pwm_bright);
                }
            }
        }
        if (get_switch()) {
            switch (fade_direction) {
                case -1:
                    if (fade_delay_counter++ > FADE_DELAY) {
                        down(&pwm_bright, 0);
                        pwm_set(pwm_bright);
                        fade_delay_counter = 0;
                    }
                    break;
                case  0:
                    if (pwm_bright < PWM_MAX/2) {
                        fade_direction = 1;
                    } else {
                        fade_direction = -1;
                    }
                    break;
                case  1:
                    if (fade_delay_counter++ > FADE_DELAY) {
                        up(&pwm_bright, PWM_MAX);
                        pwm_set(pwm_bright);
                        fade_delay_counter = 0;
                    }
                    break;
            }
        } else {
            fade_direction = 0;
        }

    }
    /*
	while(1) {
    fade_to(PWM_MAX, 5);
    fade_to(0, 5);
	}
    */
}
