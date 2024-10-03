
ARM64 = aarch64-elf
COMPILEFLAGS = -g -c -O0 -Wall

all: clean kernel8.img

kernel8.img: start.o main.o uart.o mbox.o timer.o i2c.o bmi270.o
	$(ARM64)-ld -nostdlib -nostartfiles start.o main.o uart.o mbox.o timer.o i2c.o bmi270.o -T link.ld -o kernel8.elf

start.o: $(SOURCE)start.S
	aarch64-elf-gcc -c $(SOURCE)start.S -o $(BUILD)start.o


main.o: main.c
	$(ARM64)-gcc $(COMPILEFLAGS) -o main.o main.c

uart.o: uart.c uart.h
	$(ARM64)-gcc $(COMPILEFLAGS) -o uart.o uart.c

mbox.o: mbox.c mbox.h
	$(ARM64)-gcc $(COMPILEFLAGS) -o mbox.o mbox.c

timer.o: timer.c timer.h
	$(ARM64)-gcc $(COMPILEFLAGS) -o timer.o timer.c

i2c.o: i2c.c i2c.h
	$(ARM64)-gcc $(COMPILEFLAGS) -o i2c.o i2c.c

bmi270.o: bmi270.c bmi270.h
	$(ARM64)-gcc $(COMPILEFLAGS) -o bmi270.o bmi270.c

clean:
	rm -f *.o *.elf *.img

	
	
