#include <stdint.h>

#include "startup.h"

static void unexpected_interrupt(void);
void systick(void);

void (*nvic_table[])() __attribute__((section(".nvic_table"), used)) = {
	(void (*)()) &_estack,
        startup,                //-6: Reset
        unexpected_interrupt,   //-5: NMI
        unexpected_interrupt,   //-4: HardFault
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //-3: SVCall
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //XX: unused
        unexpected_interrupt,   //-2: PendSV
        systick,                //-1: SysTick
        unexpected_interrupt,   // 0: SPI0
        unexpected_interrupt,   // 1: SPI1
        unexpected_interrupt,   // 2: Reserved Interrupt
        unexpected_interrupt,   // 3: USART0
        unexpected_interrupt,   // 4: USART1
        unexpected_interrupt,   // 5: USART2
        unexpected_interrupt,   // 6: Reserved Interrupt
        unexpected_interrupt,   // 7: Reserved Interrupt
        unexpected_interrupt,   // 8: I2C
        unexpected_interrupt,   // 9: SCT
        unexpected_interrupt,   //10: MRT
        unexpected_interrupt,   //11: CMP
        unexpected_interrupt,   //12: WDT
        unexpected_interrupt,   //13: BOD
        unexpected_interrupt,   //14: Flash interrupt
        unexpected_interrupt,   //15: WKT Interrupt
        unexpected_interrupt,   //16: Reserved Interrupt
        unexpected_interrupt,   //17: Reserved Interrupt
        unexpected_interrupt,   //18: Reserved Interrupt
        unexpected_interrupt,   //19: Reserved Interrupt
        unexpected_interrupt,   //20: Reserved Interrupt
        unexpected_interrupt,   //21: Reserved Interrupt
        unexpected_interrupt,   //22: Reserved Interrupt
        unexpected_interrupt,   //23: Reserved Interrupt
        unexpected_interrupt,   //24: External Interrupt 0
        unexpected_interrupt,   //25: External Interrupt 1
        unexpected_interrupt,   //26: External Interrupt 2
        unexpected_interrupt,   //27: External Interrupt 3
        unexpected_interrupt,   //28: External Interrupt 4
        unexpected_interrupt,   //29: External Interrupt 5
        unexpected_interrupt,   //30: External Interrupt 6
        unexpected_interrupt,   //31: External Interrupt 7
};

static void __attribute__((interrupt("IRQ"))) unexpected_interrupt(void) {
	while(1);
}
