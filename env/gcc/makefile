# ------------------------------------------------------------------------------
# makefile - MPF 04/2020
# ------------------------------------------------------------------------------

# compiler
CC = gcc

# compiler flags:
# .. all warnings
CFLAGS += -Wall
# .. generate dependency files *.d
CFLAGS += -MMD -MP
# .. #define symbols
CFLAGS += -DSYMBOL
# .. paths to search for header files
CFLAGS += -I../../source/

# .. paths to search for source files
VPATH += ../../source/app
VPATH += ../../source/lib
VPATH += ../../source/hil/gcc

# linked libraries:
# .. path to libraries (for non standard libraries)
# LDFLAGS += -L./
# .. libraries to link
# LIBS += -llib

# executable name
EXEC = debug

# object folder
OBJDIR = ./obj

# source files
SRC = app.c \
	sch.c \
	comm.c \
	main.c \
	adc.c \
	i2c.c \
	io.c \
	pwm.c \
	sys.c \
	uart.c \

# to execute under windows/linux
ifdef OS
	FixPath = $(subst /,\,$1)
	MKDIR = if not exist $(call FixPath,$(OBJDIR)) mkdir $(call FixPath,$(OBJDIR)) 
	RM = del /Q
else
	ifeq ($(shell uname), Linux)
		MKDIR = mkdir -p $(OBJDIR)
		RM = rm -f
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
	$(RM) debug
	$(RM) $(call FixPath,$(SRC:%.c=$(OBJDIR)/%.o))
	$(RM) $(call FixPath,$(SRC:%.c=$(OBJDIR)/%.d))


