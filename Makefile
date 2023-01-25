all:
	# noop

setup:
	cp lib/config/example_config.h lib/config/config.h
	# Please edit config.h in lib/config

.PHONY: all setup