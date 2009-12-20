CC := $(CROSS_COMPILE)gcc

PURPLE_CFLAGS := $(shell pkg-config --cflags purple-mini-2)
PURPLE_LIBS := $(shell pkg-config --libs purple-mini-2)

TP_CFLAGS := $(shell pkg-config --cflags telepathy-glib)
TP_LIBS := $(shell pkg-config --libs telepathy-glib)

GOBJECT_CFLAGS := $(shell pkg-config --cflags gobject-2.0)
GOBJECT_LIBS := $(shell pkg-config --libs gobject-2.0)

DBUS_GLIB_CFLAGS := $(shell pkg-config --cflags dbus-glib-1)
DBUS_GLIB_LIBS := $(shell pkg-config --libs dbus-glib-1)

CFLAGS := -O2 -ggdb -Wall

prefix := /usr

datadir := $(prefix)/share
libexecdir := $(prefix)/libexec

objects := connection-aliasing.o \
           connection-avatars.o \
           connection.o \
           connection-presence.o \
           connection-manager.o \
           contact-list.o \
           contact-list-channel.o \
           debug.o \
           im-channel.o \
           im-channel-factory.o \
           main.o \
           notify.o \
           request.o

app := telepathy-haze
service := org.freedesktop.Telepathy.ConnectionManager.haze.service

.PHONY: clean

all:

# pretty print
V = @
Q = $(V:y=)
QUIET_CC    = $(Q:@=@echo '   CC         '$@;)
QUIET_LINK  = $(Q:@=@echo '   LINK       '$@;)
QUIET_CLEAN = $(Q:@=@echo '   CLEAN      '$@;)

D = $(DESTDIR)

$(app): $(objects)
$(app): CFLAGS := $(CFLAGS) $(PURPLE_CFLAGS) $(TP_CFLAGS) $(GOBJECT_CFLAGS) $(DBUS_GLIB_CFLAGS) \
	-DG_LOG_DOMAIN='"haze"' -DPACKAGE_VERSION='"0.3.2"'
$(app): LIBS := $(LIBS) $(PURPLE_LIBS) $(TP_LIBS) $(GOBJECT_LIBS) $(DBUS_GLIB_LIBS)
$(app): LDFLAGS := $(LDFLAGS) -Wl,--no-undefined

all: $(app)

$(app):
	$(QUIET_LINK)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o:: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -MMD -o $@ -c $<

clean:
	$(QUIET_CLEAN)$(RM) $(app) data/$(service) *.o *.d

data/$(service): data/$(service).in
	sed -e 's#@libexecdir@#$(libexecdir)#g' -e 's#@app@#$(app)#g' $< > $@

install: $(app) data/$(service)
	mkdir -p $(D)/$(datadir)/dbus-1/services
	install -m 664 data/$(service) $(D)/$(datadir)/dbus-1/services
	mkdir -p $(D)/$(libexecdir)
	install -m 755 $(app) $(D)/$(libexecdir)

uninstall:
	$(RM) $(D)/$(datadir)/dbus-1/services/$(service)
	$(RM) $(D)/$(libexecdir)/$(app)

-include *.d
