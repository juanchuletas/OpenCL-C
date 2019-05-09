TARGET = openCL_project
SRC = src
INC = include
BIN = bin
RED = \033[1;31m
GREEN = \033[1;32m
BLUE = \033[1;34m
YELLOW = \033[1;33m
NC = \033[1;0m
UNAME := $(shell uname)

SOURCE = $(wildcard $(SRC)/*.c)

OBJECT = $(patsubst %,$(BIN)/%, $(notdir $(SOURCE:.c=.o)))

CC = gcc

ifeq ($(UNAME), Darwin)
SYSNAME:=$(shell sw_vers -productName)
SYSV:=$(shell sw_vers -productVersion)
CFLAGS = -I$(INC)
LIBS = -framework OpenCL

$(BIN)/$(TARGET) : $(OBJECT)
	@echo "$(RED)LINKING...$(NC)"
	$(CC) -o $@ $^ $(LIBS)
	@echo "$(YELLOW)FINISHED!!!$(NC)"
	@echo "$(YELLOW)SUCCESFULL!!!$(NC)"
	@echo "$(BLUE)OPERATING SISTEM: $(SYSNAME)!!!...$(NC)"

$(BIN)/%.o : $(SRC)/%.c
	@echo "$(GREEN)COMPILING...$(NC)"
	@echo  "\033[0;32m [OK] \033[0m       \033[0;33m Compiling:\033[0m" $<
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean :
	rm -f $(OBJECT) $(BIN)/$(TARGET)

