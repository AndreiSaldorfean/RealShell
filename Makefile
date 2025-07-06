all: clean
	cmake -S . -B build -G "Unix Makefiles"
b:
	cmake --build build
clean:
	rm -rf build
flash:
	@if [ -d /media/rudy/RPI-RP2 ]; then \
		echo "Flashing..."; \
		cp ./build/RealShell.uf2 /media/rudy/RPI-RP2; \
	else \
		echo "RPI-RP2 not found. Please press BOOTSEL and connect the board."; \
	fi
