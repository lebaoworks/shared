# Compile macros
CC := g++
CCFLAGS := -Wall
LDFLAGS := -pthread -lgtest

# File macros
CPP_SRCS := BufferQueue.cpp Event.cpp
TARGET_SRC := main.cpp
TEST_SRC := test.cpp

TARGET_NAME := output
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif

# default rule
default: all

.PHONY: all
all:
	$(CC) $(CCFLAGS) $(TARGET_SRC) $(CPP_SRCS) -o $(TARGET_NAME) $(LDFLAGS)

.PHONY: test
test:
	$(CC) $(CCFLAGS) $(TEST_SRC) $(CPP_SRCS) -o $(TARGET_NAME) $(LDFLAGS)
	./$(TARGET_NAME)
	$(MAKE) clean

.PHONY: clean
clean:
	@rm -rf $(TARGET_NAME)
