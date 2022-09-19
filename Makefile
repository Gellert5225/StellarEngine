all:
	$(MAKE) -C Stellar
	$(MAKE) -C Sandbox

debug: 
	$(MAKE) debug -C Stellar
	$(MAKE) debug -C Sandbox

release: 
	$(MAKE) release -C Stellar
	$(MAKE) release -C Sandbox

stellar:
	$(MAKE) -C Stellar

sandbox:
	$(MAKE) -C Sandbox

run-debug:
	$(MAKE) debug -C Sandbox run

run-release:
	$(MAKE) release -C Sandbox run

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
run_mac:
	$(MAKE) -C Sandbox run_mac
else

endif

clean:
	$(MAKE) -C Stellar clean
	$(MAKE) -C Sandbox clean