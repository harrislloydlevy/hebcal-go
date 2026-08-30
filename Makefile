# Makefile for hebcal-c (desktop build: library, CLI, and tests)
# License: GPL-2.0-or-later

CC      ?= cc
CFLAGS  ?= -std=c99 -Wall -Wextra -O2 -Isrc
LDFLAGS ?= -lm

SRC_DIR = src
CLI_DIR = cli
TST_DIR = tests
TOL_DIR = tools

# Library sources (excluding CLI and tests)
LIB_SRCS = $(SRC_DIR)/greg.c $(SRC_DIR)/hdate.c \
           $(SRC_DIR)/anniversary.c $(SRC_DIR)/gematriya.c $(SRC_DIR)/molad.c \
           $(SRC_DIR)/locale.c $(SRC_DIR)/event.c \
           $(SRC_DIR)/sedra.c $(SRC_DIR)/omer.c \
           $(SRC_DIR)/noaa.c $(SRC_DIR)/zmanim.c \
           $(SRC_DIR)/holidays.c $(SRC_DIR)/hebcal.c

# Test sources
TST_SRCS = $(TST_DIR)/test_greg.c $(TST_DIR)/test_hdate.c \
           $(TST_DIR)/test_anniversary.c $(TST_DIR)/test_gematriya.c \
           $(TST_DIR)/test_molad.c $(TST_DIR)/test_sedra.c \
           $(TST_DIR)/test_omer.c $(TST_DIR)/test_event.c \
           $(TST_DIR)/test_noaa.c $(TST_DIR)/test_zmanim.c \
           $(TST_DIR)/test_holidays.c $(TST_DIR)/tests_main.c

# Object files
LIB_OBJS = $(LIB_SRCS:.c=.o)
TST_OBJS = $(TST_SRCS:.c=.o)

# Targets
TEST_BIN = $(TST_DIR)/hebcal_tests
CLI_BIN = hebcal-c
GEN_TABLES = $(TOL_DIR)/gen_tables

.PHONY: all test clean tables cli

all: tables $(TEST_BIN) $(CLI_BIN)

cli: $(CLI_BIN)

# CLI binary
CLI_SRCS = $(CLI_DIR)/hebcal_cli.c
CLI_OBJS = $(CLI_SRCS:.c=.o)

$(CLI_BIN): $(CLI_OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLI_DIR)/%.o: $(CLI_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Generate lookup tables
tables: $(GEN_TABLES)
	$(GEN_TABLES) > $(SRC_DIR)/tables_data.h

$(GEN_TABLES): $(TOL_DIR)/gen_tables.c
	$(CC) $(CFLAGS) -o $@ $<

# Compile library objects
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/tables_data.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile test objects
$(TST_DIR)/%.o: $(TST_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link test binary
$(TEST_BIN): $(TST_OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Run tests
test: $(TEST_BIN)
	$(TEST_BIN)

clean:
	rm -f $(LIB_OBJS) $(TST_OBJS) $(TEST_BIN) $(GEN_TABLES)
