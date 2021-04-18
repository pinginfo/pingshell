CC = gcc
OBJS = main.o
COMPILE_OPTS = -g -Wall -c
OPTIM_OPTS = 
SANITIZERS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer

SRCDIR = src
OBJDIR = obj
BINDIR = bin
BINNAME = pingshell

SRC := $(shell find $(SRCDIR) -type f -name '*.c')
OBJ			:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRC:.c=.o))
DEP			:= $(OBJ:.o=.d)
BIN			:= $(BINDIR)/$(BINNAME)
-include $(DEP)

all: setup $(BIN)
setup: dir
remake: clean all

dir:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(OPTIM_OPTS) $(SANITIZERS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(COMPILE_OPTS) $(OPTIM_OPTS) $(SANITIZERS) -c -MMD -MP -o $@ $< 

.PHONY = clean

clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
	rmdir $(OBJDIR) $(BINDIR) 2> /dev/null; true

