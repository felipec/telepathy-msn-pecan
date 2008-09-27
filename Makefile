CC := gcc
RM := rm -f

TP_CFLAGS := $(shell pkg-config --cflags telepathy-glib)
TP_LIBS := $(shell pkg-config --libs telepathy-glib)

CFLAGS += -Wall

objects := main.o \
	   manager.o \
	   connection.o \
	   factory.o

sources := $(objects:.o=.c)
deps := $(objects:.o=.d)

.PHONY: clean

version := 0.1

# Default target:
all:

# from Lauri Leukkunen's build system
ifdef V
Q = 
P = @printf "" # <- space before hash is important!!!
else
P = @printf "[%s] $@\n" # <- space before hash is important!!!
Q = @
endif

target := telepathy-msn-pecan
$(target): $(objects) libmsn-pecan/libmsn-pecan.a
$(target): CFLAGS := $(CFLAGS) $(TP_CFLAGS) -D VERSION='"$(version)"'
$(target): LIBS := $(TP_LIBS)

all: $(target)

libmsn-pecan/libmsn-pecan.a:
	$(P)SUBDIR
	$(Q)$(MAKE) -C libmsn-pecan

$(target):
	$(P)LINK
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.a::
	$(P)ARCHIVE
	$(Q)$(AR) rcs $@ $^

%.o:: %.c
	$(P)CC
	$(Q)$(CC) $(CFLAGS) -MMD -o $@ -c $<

clean:
	@printf "[CLEAN]\n"
	$(Q)$(RM) $(target) $(objects) $(deps)

-include $(deps)
