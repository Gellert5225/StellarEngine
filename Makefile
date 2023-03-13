BUILD = release
export

include $(wildcard ./PrettyPrint.inc)

all:
	@make --no-print-directory BUILD=$(BUILD) -C Stellar
	@make --no-print-directory BUILD=$(BUILD) -C StellarEditor

stellar:
	@make --no-print-directory -C Stellar

sandbox:
	@make --no-print-directory -C Sandbox

run:
	@make --no-print-directory -C StellarEditor run BUILD=$(BUILD)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
run_mac:
	@make --no-print-directory -C Sandbox run_mac
else

endif

clean:
	@echo Cleaning Stellar...
	@make --no-print-directory -C Stellar clean
	@echo Cleaning Sandbox...
	@make --no-print-directory -C Sandbox clean
	@echo Cleaning Editor...
	@make --no-print-directory -C StellarEditor clean
	@echo Done