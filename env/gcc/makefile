# ------------------------------------------------------------------------------
# makefile - MPF 04/2020
# ------------------------------------------------------------------------------

# compiler
CC = gcc

# compiler flags:
# .. all warnings
CFLAGS += -Wall
CFLAGS += -Wundef
# .. ignore unknown pragmas
CFLAGS += -Wno-unknown-pragmas
# .. generate dependency files *.d
CFLAGS += -MMD -MP
# .. #define symbols
CFLAGS += -DSIMULATION
# .. paths to search for header files
CFLAGS += -I../../source/app
CFLAGS += -I../../source/hal
CFLAGS += -I../../source/lib

# .. paths to search for source files
VPATH += ../../source/app
VPATH += ../../source/lib
VPATH += ../../source/hil/gcc

# linked libraries:
# .. path to libraries (for non standard libraries)
# LDFLAGS += -L./
# .. libraries to link
# LIBS += -llib
LIBS += -lm

# executable name
EXEC = debug

# object folder
OBJDIR = ./obj

# source files
SRC = app.c \
	sch.c \
	comm.c \
	main.c \
	io.c \
	sys.c \
	uart.c \
	pwm.c \
	adc.c \
	dac.c \
	error.c \
	mathlib.c \
	dac.c \
	i2c.c \
	fb.c

# to execute under windows/linux
ifdef OS
	FixPath = $(subst /,\,$1)
	MKDIR = if not exist $(call FixPath,$(OBJDIR)) mkdir $(call FixPath,$(OBJDIR)) 
	RM = del /Q
	EXT = .exe
else
	ifeq ($(shell uname), Linux)
		MKDIR = mkdir -p $(OBJDIR)
		RM = rm -f
		FixPath = $1
   endif
endif

# build target - linker command
$(EXEC) : $(SRC:%.c=$(OBJDIR)/%.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# compile source files
$(OBJDIR)/%.o : %.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# auto dependencies - recompile if a header file changed
-include $(SRC:%.c=$(OBJDIR)/%.d)

# clean
.PHONY: clean 
clean :
	$(RM) $(EXEC)$(EXT)
	$(RM) $(call FixPath,$(SRC:%.c=$(OBJDIR)/%.o))
	$(RM) $(call FixPath,$(SRC:%.c=$(OBJDIR)/%.d))


