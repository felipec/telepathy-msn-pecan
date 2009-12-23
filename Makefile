targets := pecan/libmsn-pecan.so \
	   tp-haze/telepathy-pecan

modules := purple pecan tp-haze

all: $(targets)

export PKG_CONFIG_PATH=$(PWD)

pecan/libmsn-pecan.so: purple/libpurple-mini-2.so
	$(MAKE) -C pecan

purple/libpurple-mini-2.so:
	$(MAKE) -C purple DESTDIR=$(PWD)/tmp install
	sed -e 's#@fake_prefix@#$(PWD)/tmp/usr#' \
		purple-mini-2.pc.in > \
		purple-mini-2.pc

tp-haze/telepathy-pecan: purple/libpurple-mini-2.so
	$(MAKE) -C tp-haze

install: $(targets)
	for x in $(modules); do \
		$(MAKE) -C $$x $@; \
	done

.PHONY: $(targets) clean

clean:
	for x in $(modules); do \
		$(MAKE) -C $$x $@; \
	done
