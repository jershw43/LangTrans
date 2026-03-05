CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
DEPS = file_util.h scanner.h parser.h
OBJ = main.o file_util.o scanner.o parser.o
TARGET = Program3

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	@del /Q $(OBJ) $(TARGET) 2>nul || true
	for %f in (*.txt *.lis *.bak *.tmp) do if /i not "%f"=="input.txt" del "%f"
else
	@rm -f $(OBJ) $(TARGET)
	@rm -f $(filter-out input.txt,$(wildcard *.txt)) *.lis *.bak *.tmp
endif

.PHONY: all clean
