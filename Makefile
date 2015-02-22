OBJS = src/startup.o \
       src/irq.o \
       src/main.o \
       src/pwm.o \
       src/systick.o \
       src/drv/sysinit.o \
       src/drv/syscon.o \
       src/drv/clock.o \
       src/drv/gpio.o \
       src/drv/sct.o \
       src/drv/swm.o \
       src/drv/iocon.o

COMPILER_PREFIX=arm-none-eabi

CFLAGS=-mthumb -mcpu=cortex-m0plus -O0 -g -flto -fno-common -ffreestanding -Wall -Wno-unused-function -Wextra -Werror -std=c99
LDFLAGS=-Wl,--gc-sections -O0 -flto -g -nostartfiles -mthumb -mcpu=cortex-m0plus

CC=$(COMPILER_PREFIX)-gcc
LD=$(COMPILER_PREFIX)-ld
OBJCOPY=$(COMPILER_PREFIX)-objcopy
SIZE=$(COMPILER_PREFIX)-size
GDB=$(COMPILER_PREFIX)-gdb

all: licht.bin

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ -Iinclude/ $<

licht.elf: $(OBJS)
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $(OBJS) -lgcc
	$(SIZE) $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) licht.elf licht.bin
