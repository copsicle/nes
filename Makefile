CC = gcc
CFLAGS = -g -Wall

BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/main.exe

all: $(TARGET)

$(TARGET): $(OBJS)
  @mkdir -p $(BIN_DIR)
  $(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: ./%.c
  @mkdir -p $(BUILD_DIR)
  $(CC) $(CFLAGS) -c $> -o $@

clean:
  @rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean