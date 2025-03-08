CC := gcc
TARGET := shooty_game

SRC_DIR := ./src
INC_DIR := ./include
BUILD_DIR := ./out

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

CFLAGS := -I$(INC_DIR) -Wall -Wextra
LDFLAGS := -lraylib -lm

ifeq ($(DEBUG), 1)
	CFLAGS += -g -Og -DDEBUG
else
	CFLAGS += -O2 -DRELEASE
endif

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET)

# Link object files and create the binary.
$(BUILD_DIR)/$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile object files from .c files.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

debug:
	$(MAKE) DEBUG=1

clean:
	rm -r $(BUILD_DIR)
