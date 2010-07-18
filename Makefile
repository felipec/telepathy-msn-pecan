targets := pecan/libmsn-pecan.so \
	   tp-haze/telepathy-pecan

modules := purple pecan tp-haze
prefix := /usr

all: $(targets)

export PKG_CONFIG_PATH=$(PWD)

pecan/libmsn-pecan.so: purple/libpurple-mini-2.so
	$(MAKE) -C pecan

purple/libpurple-mini-2.so:
	$(MAKE) -C purple DESTDIR=$(PWD)/tmp install
	sed -e 's#@fake_prefix@#$(PWD)/tmp#' \
		-e 's#@prefix@#$(prefix)#' \
		purple-mini-2.pc.in > \
		purple-mini-2.pc

tp-haze/telepathy-pecan: purple/libpurple-mini-2.so
	$(MAKE) -C tp-haze

install: $(targets)
	for x in $(modules); do \
		$(MAKE) -C $$x $@; \
	done

.PHONY: $(targets) clean copy

clean:
	for x in $(modules); do \
		$(MAKE) -C $$x $@; \
	done

copy:
	for x in $(modules); do \
		$$x/copy $$x; \
	done
