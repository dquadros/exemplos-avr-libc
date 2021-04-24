# Makefile para gerar HEX usando o WinAVR
# Deve funcionar tambem sob Linux
# TODO: Formatar como os outros Makefiles

PROGNAME=i2c_display

AVRDUDEFLAGS=-c usbtiny -p attiny85

GCCFLAGS=-g -Os -Wall -mcall-prologues -mmcu=attiny85 -DF_CPU=16500000
LINKFLAGS=-Wl,-Map,${PROGNAME}.map

LFUSE = 0xe1
HFUSE = 0xdd
EFUSE = 0xfe


all:	${PROGNAME}.hex

program: ${PROGNAME}-upload

clean:
	rm -f ${PROGNAME}.o ${PROGNAME}.hex ${PROGNAME}.map

fuse:
	avrdude ${AVRDUDEFLAGS} -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m \
    -U efuse:w:$(EFUSE):m

I2c.o: ../LibFranzininho/Driver/I2c.h ../LibFranzininho/Driver/I2c.c
	avr-gcc ${GCCFLAGS} -c -o I2c.o ../LibFranzininho/Driver/I2c.c

${PROGNAME}.hex: ${PROGNAME}.o
	avr-objcopy -R .eeprom -O ihex ${PROGNAME}.o ${PROGNAME}.hex

${PROGNAME}.o: main.c i2c.o
	avr-gcc ${GCCFLAGS} ${LINKFLAGS} -o ${PROGNAME}.o main.c i2c.o
   
${PROGNAME}-upload:	${PROGNAME}.hex
	avrdude ${AVRDUDEFLAGS} -U flash:w:${PROGNAME}.hex:i