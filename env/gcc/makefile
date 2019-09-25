# makefile - MPF 2019

# compiler
CC = gcc

# compiler flags
CFLAGS += -Wall
#CFLAGS += -Dsymbol
CFLAGS += -I../../source/

# paths to search for source files
VPATH += ../../source/app
VPATH += ../../source/lib
VPATH += ../../source/hil/gcc

# objects
app = app.o \

lib = sch.o \
	comm.o \

hil = main.o \
	adc.o \
	i2c.o \
	io.o \
	pwm.o \
	sys.o \
	uart.o \

# binary
debug : $(app) $(lib) $(hil)
	$(CC) -o debug  $(app) $(lib) $(hil)

# source : prerequisite
%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

# clean
.PHONY: clean 
clean :
	rm debug  $(app) $(lib) $(hil)

