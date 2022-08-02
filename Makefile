DIRS := src src/valery src/lib src/builtins
#SRCS := $(wildcard **/*.c) $(wildcard *.c)
SRCS := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJDIR = obj
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

$(OBJDIR): $(DIRS)
	$(foreach dir, $(DIRS), @mkdir -p $(OBJDIR)/$(dir))
