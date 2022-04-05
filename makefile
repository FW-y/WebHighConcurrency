CPPC = g++
CFLAG =  -O0 -g -lpthread
SRC = $(wildcard	./src/*.cpp)
OBJ = $(patsubst	%.cpp, %.o, $(SRC))
TARGET = main
BUILDDIR = ../build/

$(TARGET): $(OBJ)
	$(CPPC) $(OBJ) -o $@ $(CFLAG)


%.o: %.c
	$(CPPC) -c &< -o $@ $(CFLAG)
:wildcard