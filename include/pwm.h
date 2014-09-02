#ifndef PWM_H
#define PWM_H

#define PWM_MAX 63

void pwm_init(void);
void pwm_set(uint8_t val);
void pwm_set_raw(uint16_t val);
uint8_t pwm_get(void);

#endif
