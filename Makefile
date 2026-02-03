NAME = nibbler

SRC = $(addprefix src/, main.cpp Board.cpp Snake.cpp)
OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -Iinclude/
LDFLAGS  = -rdynamic

.PHONY: all
all: $(NAME) frontends/SDL2/build frontends/OpenGL/build
	cmake --build frontends/SDL2/build
	cmake --build frontends/OpenGL/build

frontends/SDL2/build:
	cmake -B frontends/SDL2/build -S frontends/SDL2

frontends/OpenGL/build:
	cmake -B frontends/OpenGL/build -S frontends/OpenGL

src/Board.o: src/Board.cpp include/nibbler/Board.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

src/Snake.o: src/Snake.cpp include/nibbler/Board.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

src/main.o: src/main.cpp include/nibbler/init.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(NAME): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(NAME) $(OBJ)
	$(RM) -r frontends/SDL2/build frontends/SDL2/.cache
	$(RM) -r frontends/OpenGL/build frontends/OpenGL/.cache

.PHONY: re
re: clean all

.NOTPARALLEL: all clean
