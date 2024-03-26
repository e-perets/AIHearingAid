# Makefile for hsl, the High Score List program
# with automatic dependency scanning

PROGRAM := record+play.out
SRCS := record+play.c
CFLAGS := -Wall
LIB_LINK := -lasound

COMPILE.cc = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH)

.PHONY: all clean
all: $(PROGRAM)

clean:
	$(RM) $(PROGRAM)

$(PROGRAM): $(SRCS)
	$(COMPILE.cc) $^ $(LIB_LINK) -o $@
