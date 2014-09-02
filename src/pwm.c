#include <stdint.h>
#include "drv/chip.h"

#include "pwm.h"

/*
 * F_clk: 24 MHz
 * Target resolution: 10 bits -> Limit at 1024
 *  -> PWM-Frequency: 23 kHz
 *
 * EV0: Limit
 * EV1: Turn on output
 * EV2: Turn off output
 */

#define PWM_LIMIT 1024
#include "pwm_table.h"

static uint8_t pwm_value = 0;

void pwm_set(uint8_t val) {
	pwm_value = val;

	if(val >= PWM_MAX)
		pwm_set_raw(PWM_LIMIT + 1);
	else
		pwm_set_raw(pwm_table[val]);
}

uint8_t pwm_get(void) {
	return pwm_value;
}

void pwm_set_raw(uint16_t val) {
	LPC_SCT->MATCHREL[SCT_MATCH_2].L = val;
}

void pwm_init(void) {
	// configure GPIO
	Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, 0, 13, true);

	Chip_SWM_Init();
	Chip_SWM_MovablePinAssign(SWM_CTOUT_0_O, 13);
	Chip_SWM_Deinit();

	// configure SCT
	Chip_SCT_Init(LPC_SCT);

	// EV0
	LPC_SCT->MATCH[SCT_MATCH_0].L = PWM_LIMIT;
	LPC_SCT->MATCHREL[SCT_MATCH_0].L = PWM_LIMIT;
	LPC_SCT->EVENT[0].CTRL = 0x01 << 12 | // COMBMODE = MATCH
	                            0 <<  0;  // MATCHSEL = 0
	LPC_SCT->EVENT[0].STATE = 1;

	// EV1
	LPC_SCT->MATCH[SCT_MATCH_1].L = 0;
	LPC_SCT->MATCHREL[SCT_MATCH_1].L = 0;
	LPC_SCT->EVENT[1].CTRL = 0x01 << 12 | // COMBMODE = MATCH
	                            1 <<  0;  // MATCHSEL = 1
	LPC_SCT->EVENT[1].STATE = 1;

	// EV2
	LPC_SCT->MATCH[SCT_MATCH_2].L = 0;
	LPC_SCT->MATCHREL[SCT_MATCH_2].L = 0;
	LPC_SCT->EVENT[2].CTRL = 0x01 << 12 | // COMBMODE = MATCH
	                            2 <<  0;  // MATCHSEL = 2
	LPC_SCT->EVENT[2].STATE = 1;

	LPC_SCT->LIMIT_L = 1 << 0;
	LPC_SCT->OUT[0].SET = 1 << 1;
	LPC_SCT->OUT[0].CLR = 1 << 2;

	// Conflict resolution
	Chip_SCT_SetConflictResolution(LPC_SCT, 0, SCT_RES_CLEAR_OUTPUT);

	Chip_SCT_ClearControl(LPC_SCT, SCT_CTRL_HALT_L);
}
