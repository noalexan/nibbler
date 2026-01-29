NAME = nibbler

SRC = $(addprefix src/, main.cpp)
OBJ = $(SRC:.cpp=.o)

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(NAME) $(OBJ)

.PHONY: re
re: clean all

.NOT_PARALLEL: $(NAME) $(OBJ) clean
