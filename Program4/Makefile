CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

SRCS = main.c file_util.c scanner.c parser.c action.c
DEPS = file_util.h scanner.h parser.h action.h
INPUTS = example.in example2.in
OBJS = $(SRCS:.c=.o)
TARGET = Program4
KEEP = $(SRCS) $(DEPS) $(INPUTS) Makefile .vscode

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	@for %%f in (*) do @(echo $(KEEP) | findstr /I /C:"%%f" >nul || del "%%f")
else
	@find . -maxdepth 1 -type f ! -name '.' $(foreach f,$(KEEP),! -name '$(f)') -delete
endif

.PHONY: all clean
