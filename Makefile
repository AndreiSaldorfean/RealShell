DEVICE := /dev/sdb1

all: clean
	cmake -S . -B build -G "Unix Makefiles"
b:
	cmake --build build
clean:
	rm -rf build
flash:

	@while [ ! -e $(DEVICE) ]; do \
		clear; \
		echo "Waiting for device"; \
		sleep 1; \
	done

	clear

	@if [ ! -d /media/rudy/RPI-RP2/ ]; then \
		sudo mkdir /media/rudy/RPI-RP2/; \
	fi

	@sudo mount $(DEVICE) /media/rudy/RPI-RP2

	@if [ -d /media/rudy/RPI-RP2 ]; then \
		echo "Flashing..."; \
		sudo cp ./build/RealShell.uf2 /media/rudy/RPI-RP2; \
	else \
		echo "RPI-RP2 not found. Please press BOOTSEL and connect the board."; \
	fi

	@sudo umount /media/rudy/RPI-RP2/

