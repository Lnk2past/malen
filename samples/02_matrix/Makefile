CXX:=g++
STD:=-std=c++14
CFLAGS:=-Wall -Werror -Wextra -Weffc++ -pedantic
DEBUG_FLAGS:=-g

SRC:=$(wildcard *.cpp)
INC:=$(wildcard include/*.h)
OBJ:=$(SRC:%.cpp=obj/%.o)
DEP:=$(SRC:%.cpp=dep/%.d)
TRGT:=main.exe

INCLUDES:=-isysteminclude -I../../include
PYTHON_INCLUDES:=$(shell python3-config --includes)
PYTHON_LD_FLAGS:=$(shell python3-config --ldflags)

$(TRGT): setup $(OBJ)
	$(CXX) $(PYTHON_LD_FLAGS) $(OBJ) -o $@

setup:
	@mkdir -p obj
	@mkdir -p dep

-include $(DEP)

obj/%.o: %.cpp $(INC)
	$(CXX) $(STD) $(DEBUG_FLAGS) $(CFLAGS) $(INCLUDES) $(PYTHON_INCLUDES) -MF dep/$(<:.cpp=.d) -MMD -c $< -o $@

clean:
	rm -rf obj dep $(TRGT) __pycache__ test_output

test: $(TRGT)
	contest contest.yaml

.PHONY:all clean setup test
