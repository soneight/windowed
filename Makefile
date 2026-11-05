.PHONY: usage cmake ninja clean
# variables
BUILD := build
# commands
usage:
	@cat Makefile.usage.txt
cmake:
	@echo "configure project into build directory"
	@cmake -B $(BUILD) -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ninja:
	@echo "generates project into build directory"
	@ninja -C $(BUILD)
clean:
	@echo "remove force recursive build directory"
	@rm -fr $(BUILD)
	@git restore $(BUILD)
