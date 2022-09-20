BUILD = release
export

include $(wildcard ./PrettyPrint.inc)

all:
	@echo Building Stellar...
	@make BUILD=$(BUILD) -C Stellar
	@echo Building Sandbox...
	@make BUILD=$(BUILD) -C Sandbox

stellar:
	@make -C Stellar

sandbox:
	@make -C Sandbox

run:
	@make -C Sandbox run BUILD=$(BUILD)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
run_mac:
	@make -C Sandbox run_mac
else

endif

clean:
	@echo Cleaning Stellar...
	@make -C Stellar clean
	@echo Cleaning Sandbox...
	@make -C Sandbox clean
	@echo Done