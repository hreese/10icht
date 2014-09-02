#include "drv/chip.h"

#include "startup.h"

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

static void init_memory(void) {
	// copy data
	char *src = &_etext;
	char *dst = &_data;

	while(dst < &_edata)
		*dst++ = *src++;

	// clear bss
	for(dst = &_bstart; dst < &_bend; dst++)
		*dst = 0;
}

void main(void);

void startup(void) {
	init_memory();

	Chip_SystemInit();

	main();

	while(1);
}
