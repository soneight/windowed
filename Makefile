.PHONY: usage cmake ninja clean build allin
# commands
usage:
	@cat Makefile.usage.txt
cmake:
	@echo "configuring project"
	@cmake -B build/ -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ninja:
	@echo "generating project"
	@ninja -C build/
clean:
	@echo "cleaning build directory"
	@rm -rf build/
	@git restore build/
build:
	@echo "configuring and generating project"
	@make cmake
	@make ninja
allin:
	@echo "cleaning build directory and then configuring and generating project"
	@make clean
	@make build
