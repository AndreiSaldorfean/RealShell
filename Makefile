all: clean
	cmake -S . -B build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/StmToolchain.cmake
b:
	cmake --build build --target cc
ren:
	renode --console utils/stm_dev.resc
clean:
	rm -rf build
