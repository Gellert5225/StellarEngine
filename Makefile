all:
	$(MAKE) -C Stellar
	$(MAKE) -C Sandbox

stellar:
	$(MAKE) -C Stellar

sandbox:
	$(MAKE) -C Sandbox

run:
	$(MAKE) -C Sandbox run

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
run_mac:
	$(MAKE) -C Sandbox run_mac
else

endif

clean:
	$(MAKE) -C Stellar clean
	$(MAKE) -C Sandbox clean