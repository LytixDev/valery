# Nicolai Brand (lytix.dev) 2022
# See LICENSE for license info

OBJDIR = .obj
SRC = src
DIRS := $(shell find $(SRC) -type d)
SRCS := $(shell find $(SRC) -type f -name "*.c")
OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)

CC = gcc
CFLAGS = -I include -Wall -Wpedantic -Wextra -Wshadow -std=c99

.PHONY: clean $(OBJDIR)
TARGET = valery


$(OBJDIR)/%.o: %.c Makefile | $(OBJDIR)
	@echo [CC] $@
	@$(CC) -c $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@echo [LD] $@
	@$(CC) -o $@ $^

debug: CFLAGS += -g -DDEBUG
debug: clean
debug: $(TARGET)

clean:
	@rm -rf $(OBJDIR) $(TARGET)

$(OBJDIR):
	$(foreach dir, $(DIRS), $(shell mkdir -p $(OBJDIR)/$(dir)))
