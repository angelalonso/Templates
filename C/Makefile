CC = gcc
CFLAGS = -Wall -Wextra -O2 `pkg-config --cflags gtk+-3.0 webkit2gtk-4.1`
LIBS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.1`
TARGET = config_editor
SRCDIR = src
OBJDIR = obj
SRCS = $(SRCDIR)/main.c $(SRCDIR)/config.c
OBJS = $(OBJDIR)/main.o $(OBJDIR)/config.o

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
