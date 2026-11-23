CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
SAN_FLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -g -O1 -fsanitize=address,undefined
LDFLAGS := -lm

BUILD_DIR := build
APP_BIN := $(BUILD_DIR)/cesta_basica
TEST_BIN := $(BUILD_DIR)/test_cesta_basica
SAN_TEST_BIN := $(BUILD_DIR)/test_cesta_basica_san

.PHONY: all run demo test sanitize clean

all: $(APP_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP_BIN): src/main.c src/cesta_basica.c src/cesta_basica.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) src/main.c src/cesta_basica.c -o $@ $(LDFLAGS)

$(TEST_BIN): tests/test_cesta_basica.c src/cesta_basica.c src/cesta_basica.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_cesta_basica.c src/cesta_basica.c -o $@ $(LDFLAGS)

$(SAN_TEST_BIN): tests/test_cesta_basica.c src/cesta_basica.c src/cesta_basica.h | $(BUILD_DIR)
	$(CC) $(SAN_FLAGS) tests/test_cesta_basica.c src/cesta_basica.c -o $@ $(LDFLAGS)

run: $(APP_BIN)
	./$(APP_BIN)

demo: $(APP_BIN)
	./$(APP_BIN) --demo

test: $(APP_BIN) $(TEST_BIN)
	./$(TEST_BIN)
	bash tests/test_cli.sh ./$(APP_BIN)

sanitize: $(SAN_TEST_BIN)
	./$(SAN_TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
