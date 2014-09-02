#ifndef SYSTICK_H
#define SYSTICK_H

#define HZ 1000

#define time_before(x,y) (int32_t)((y) - (int32_t)(x) > 0)
#define time_after(x,y) time_before(y,x)

extern volatile uint32_t ticks;

void systick_init(void);
void delay_ticks(uint16_t cnt);

#endif
