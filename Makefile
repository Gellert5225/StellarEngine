BUILD = release
export

include $(wildcard ./PrettyPrint.inc)

all:
	@echo Building Stellar...
	@MAKE BUILD=$(BUILD) -C Stellar
	@echo Building Sandbox...
	@MAKE BUILD=$(BUILD) -C Sandbox

stellar:
	@MAKE -C Stellar

sandbox:
	@MAKE -C Sandbox

run:
	@MAKE -C Sandbox run BUILD=$(BUILD)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
run_mac:
	@MAKE -C Sandbox run_mac
else

endif

clean:
	@echo Cleaning Stellar...
	@MAKE -C Stellar clean
	@echo Cleaning Sandbox...
	@MAKE -C Sandbox clean