# Nicolai Brand (lytix.dev) 2022
# See LICENSE for license info

OBJDIR = .obj
SRC = src
DIRS := $(shell find $(SRC) -type d)
SRCS := $(shell find $(SRC) -type f -name "*.c")
OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)

CC = gcc
CFLAGS = -I include -Wall -Wpedantic -Wextra -Wshadow -std=c99

.PHONY: clean
TARGET = valery


$(OBJDIR)/%.o: %.c | $(OBJDIR)
	@echo [CC] $@
	@$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	@echo [LD] $@
	@$(CC) -o $@ $^

clean:
	@rm -rf $(OBJDIR) $(TARGET)

$(OBJDIR):
	$(foreach dir, $(DIRS), $(shell mkdir -p $(OBJDIR)/$(dir)))
