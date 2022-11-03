NAME = interpreter
SRCS = interpreter.cpp
CXX = g++

.PHONY: all
all: $(NAME)
	./$(NAME)

$(NAME):
	$(CXX) -o $(NAME) $(SRCS)

.PHONY: clean
clean:
	rm -f $(NAME)

TESTDIR = ./test
TESTSRCS = $(shell find $(TESTDIR) )

.PHONY: test
test: $(NAME)
	@echo "test: $(TESTSRCS)" 
	./$(NAME) < $(TESTSRCS)

