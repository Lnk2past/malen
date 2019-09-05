# Basic make file
#

CXX:=g++
STD:=-std=c++14
CFLAGS:=-Werror -Wall -Wextra -Weffc++ -Wpedantic
PYVIS_INCLUDE_DIR:=../include
INCLUDES:=-I$(PYVIS_INCLUDE_DIR)

SRC:=$(wildcard *.cpp)
OBJ:=$(SRC:.cpp=.o)
TRGT:=main.exe

PYTHON_INCLUDES:=$(shell python3-config --includes)
PYTHON_LIB_DIR:=$(shell python3-config --prefix)/lib
PYTHON_LD_FLAGS:=-Wl,-rpath,$(PYTHON_LIB_DIR) -L$(PYTHON_LIB_DIR) -lpython3.6m

$(TRGT): $(OBJ)
	$(CXX) $(PYTHON_LD_FLAGS) $^ -o $@

clean:
	rm -rf $(OBJ) $(TRGT) __pycache__

%.o: %.cpp $(PYVIS_INCLUDE_DIR)/pyvis/PythonVisualizer.h
	$(CXX) $(STD) $(CFLAGS) $(INCLUDES) $(PYTHON_INCLUDES) -c $< -o $@

.PHONY:all clean

