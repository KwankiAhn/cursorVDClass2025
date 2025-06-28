CC=gcc
CFLAGS=-Wall -Wextra -std=c99
TARGET=log_analyzer
SOURCE=log_analyzer.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET) ../sample-logs/sample.log

.PHONY: all clean test 