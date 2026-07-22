CC = gcc
CFLAGS = -ansi -pedantic -Wall -I.h

SRC_DIR = .c

SOURCES = $(SRC_DIR)/errors.c $(SRC_DIR)/opcode_table.c $(SRC_DIR)/symbol_table.c $(SRC_DIR)/macro_table.c $(SRC_DIR)/num_utils.c $(SRC_DIR)/line_parser.c $(SRC_DIR)/memory_image.c $(SRC_DIR)/preassembler.c $(SRC_DIR)/first_pass.c $(SRC_DIR)/second_pass.c $(SRC_DIR)/output_writer.c $(SRC_DIR)/assembler.c

OBJECTS = $(SRC_DIR)/errors.o $(SRC_DIR)/opcode_table.o $(SRC_DIR)/symbol_table.o $(SRC_DIR)/macro_table.o $(SRC_DIR)/num_utils.o $(SRC_DIR)/line_parser.o $(SRC_DIR)/memory_image.o $(SRC_DIR)/preassembler.o $(SRC_DIR)/first_pass.o $(SRC_DIR)/second_pass.o $(SRC_DIR)/output_writer.o $(SRC_DIR)/assembler.o

TARGET = assembler

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
