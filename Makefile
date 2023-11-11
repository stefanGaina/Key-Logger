#######################################################################################################
# Module history:                                                                                     #
#   Date:       Author:                    Reason:                                                    #
#   08.08.2023  Gaina Stefan               Initial version.                                           #
# Description: This Makefile is used to invoke the Makefiles in the subdirectories.                   #
#######################################################################################################

export CC  = gcc
export CXX = g++

export SRC := src
export OBJ := obj
export LIB := lib
export BIN := bin

export RM := del /f /q

export COVERAGE_REPORT := coverage_report

GENHTML       = vendor/lcov/genhtml.perl
GENHTML_FLAGS = --branch-coverage --num-spaces=4 --output-directory coverage_report/

INFO_FILES = $(COVERAGE_REPORT)/key-logger.info

### MAKE SUBDIRECTORIES ###
all:
	$(MAKE) -C key-logger
	$(MAKE) -C key-logger-mail
	$(MAKE) -C key-logger-installer

### CLEAN SUBDIRECTORIES ###
clean:
	$(MAKE) clean -C key-logger
	$(MAKE) clean -C key-logger-mail
	$(MAKE) clean -C key-logger-installer

### MAKE UNIT-TESTS ###
ut: create_dir
	$(MAKE) -C unit-tests
	$(MAKE) run_tests -C unit-tests
	perl $(GENHTML) $(INFO_FILES) $(GENHTML_FLAGS)

### CREATE DIRECTORY ###
create_dir:
	if not exist "$(COVERAGE_REPORT)" mkdir $(COVERAGE_REPORT)

### CLEAN UNIT-TESTS ###
ut-clean:
	$(RM) $(COVERAGE_REPORT)\*
	$(RM) $(COVERAGE_REPORT)\src\*
	rd /s /q $(COVERAGE_REPORT)\src
