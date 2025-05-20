BUILD_DIR := build
CMAKE_FLAGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

.PHONY: all debug test run_tests clean

all:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release $(CMAKE_FLAGS) -DCHX_ENABLE_TESTS=OFF
	cmake --build $(BUILD_DIR)

debug:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_FLAGS) -DCHX_ENABLE_TESTS=OFF
	cmake --build $(BUILD_DIR)

test:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_FLAGS) -DCHX_ENABLE_TESTS=ON
	cmake --build $(BUILD_DIR)

run_tests:
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

