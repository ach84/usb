sinclude .crosscompile
sinclude version

ifeq ($(ARCH),)
ARCH := i386
endif

export DEB_HOST_ARCH=$(ARCH)
export DEB_BUILD_OPTIONS=noddebs
export VERSION
export RELEASE

APP1=usbctl

CFLAGS=-Wall

$(APP1): $(APP1).c
	$(CROSS_COMPILE)gcc $(CFLAGS) $(EXTRA_INCLUDE) $(APP1).c -o $@	

all: $(APP1)

clean:
	rm -f $(APP1) *~

re:
	make clean
	make all

debu:
	fakeroot debian/rules binary

debc:
	fakeroot debian/rules clean
