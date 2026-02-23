CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
DEPS = file_util.h scanner.h
OBJ = main.o file_util.o scanner.o
TARGET = Program2

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
