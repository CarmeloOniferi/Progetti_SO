CC = gcc
CFLAGS = -I../include -Wall -Wextra -Werror

SRCDIR = src
OBJDIR = obj
INCLUDEDIR = include

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

TARGET = client

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJDIR) $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(OBJDIR)

.PHONY: all clean
