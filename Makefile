.PHONY: tags docs all help travis clean

PUMBAA_VERSION ?= $(shell cat VERSION.txt)

BOARD ?= linux

ifeq ($(BOARD), linux)
TESTS = \
	tst/all
endif

ifeq ($(BOARD), nodemcu)
TESTS = \
	tst/kernel/timer
endif

ifeq ($(BOARD), nano32)
TESTS = \
	tst/all
endif

ifeq ($(BOARD), arduino_due)
TESTS = \
	tst/kernel/timer
endif

all: $(TESTS:%=%.all)

clean: $(TESTS:%=%.clean)

rerun:
	for test in $(TESTS) ; do \
	    $(MAKE) -C $$test run || exit 1 ; \
	done

# Depend on 'all' to build all applications (optinally with -j) before
# uploading and running them one at a time.
run: all
	for test in $(TESTS) ; do \
	    if [ ! -e $$test/.$(BOARD).passed ] ; then \
	        $(MAKE) -C $$test run || exit 1 ; \
	        touch $$test/.$(BOARD).passed ; \
	    else \
	        echo ; \
	        echo "$$test already passed." ; \
	        echo ; \
	    fi \
	done

report:
	@echo "================================================================================"
	@echo "Test report for $(BOARD)"
	@echo "================================================================================"
	@echo

	for test in $(TESTS) ; do \
	    $(MAKE) -C $(basename $$test) report ; \
	    echo ; \
	done

	@echo "================================================================================"
	@echo

test: run
	$(MAKE) report

travis:
	$(MAKE) test

release-test:
	+bin/release.py --test --version $(PUMBAA_VERSION)

release:
	+bin/release.py --package --version $(PUMBAA_VERSION)

clean-arduino-due:
	$(MAKE) BOARD=arduino_due SERIAL_PORT=/dev/simba-arduino_due clean

clean-nodemcu:
	$(MAKE) BOARD=nodemcu SERIAL_PORT=/dev/simba-nodemcuv3 clean

clean-nano32:
	$(MAKE) BOARD=nano32 SERIAL_PORT=/dev/simba-nano32 clean

test-arduino-due:
	@echo "Arduino Due"
	$(MAKE) BOARD=arduino_due SERIAL_PORT=/dev/simba-arduino_due test

test-nodemcu:
	@echo "NodeMCU"
	$(MAKE) BOARD=nodemcu SERIAL_PORT=/dev/simba-nodemcuv3 test

test-nano32:
	@echo "Nano32"
	$(MAKE) BOARD=nano32 SERIAL_PORT=/dev/simba-nano32 test

test-all-boards:
	$(MAKE) test-arduino-due
	$(MAKE) test-nodemcu
	$(MAKE) test-nano32

clean-all-boards:
	$(MAKE) clean-arduino-due
	$(MAKE) clean-nodemcu
	$(MAKE) clean-nano32

codecov-coverage: $(TESTS:%=%.ccc)

$(TESTS:%=%.all):
	$(MAKE) -C $(basename $@) all

$(TESTS:%=%.clean):
	$(MAKE) -C $(basename $@) clean

$(TESTS:%=%.ccc):
	$(MAKE) -C $(basename $@) codecov-coverage

docs:
	+bin/dbgen.py > database.json
	+bin/docgen.py database.json
	$(MAKE) -C docs sphinx

tags:
	echo "Creating tags file .TAGS"
	etags -o .TAGS --declarations $$(find . -name "*.[hci]" | xargs)

arduino:
	+make/arduino/arduino.py --remove-outdir --version $(PUMBAA_VERSION)

platformio:
	+make/platformio/platformio.py --version $(PUMBAA_VERSION)

mpy-cross:
	+cd bin && ./build.sh

help:
	@echo "--------------------------------------------------------------------------------"
	@echo "  target                      description"
	@echo "--------------------------------------------------------------------------------"
	@echo
