CC=gcc

GOBJECT_CFLAGS=`pkg-config --cflags gobject-2.0`
GOBJECT_LIBS=`pkg-config --libs gobject-2.0`

TP_CFLAGS=`pkg-config --cflags telepathy-glib`
TP_LIBS=`pkg-config --libs telepathy-glib`

CFLAGS+=-Wall

objects = main.o \
	  manager.o \
	  connection.o \
	  factory.o

sources = $(patsubst %.o,%.c,$(objects))

.PHONY: clean

target = "telepathy-msn-pecan"

all: $(target)

version := 0.1

# from Lauri Leukkunen's build system
ifdef V
Q = 
P = @printf "" # <- space before hash is important!!!
else
P = @printf "[%s] $@\n" # <- space before hash is important!!!
Q = @
endif

$(target): $(objects)
$(target): CFLAGS := $(CFLAGS) $(TP_CFLAGS) -D VERSION='"$(version)"'
$(target): LIBS := $(TP_LIBS)

$(target):
	$(P)LINK
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o:: %.c
	$(P)CC
	$(Q)$(CC) $(CFLAGS) -Wp,-MMD,$(dir $@).$(notdir $@).d -o $@ -c $<

clean:
	find -name '.*.d' -delete
	rm -f $(target) $(objects)
