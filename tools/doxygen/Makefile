SHELL = bash
MK_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

all:
	@chmod +x $(MK_DIR)extract.sh
	@echo -n "Doxygen version: " && doxygen --version
	sh $(MK_DIR)extract.sh
	doxygen $(MK_DIR)Doxyfile
