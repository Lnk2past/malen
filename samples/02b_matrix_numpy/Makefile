CXX:=g++
STD:=-std=c++17
CFLAGS:=-Wall -Werror -Wextra -Weffc++ -pedantic -D_MALENABLE_NUMPY
DEBUG_FLAGS:=-g

SRC:=$(wildcard *.cpp)
INC:=$(wildcard include/*.h)
OBJ:=$(SRC:%.cpp=obj/%.o)
DEP:=$(SRC:%.cpp=dep/%.d)
TRGT:=main.exe

INCLUDES:=-I../../include

PYVERSION:=$(shell python3 --version)
PY8_REGEX:=^Python [0-9]+\.8\.[0-9]+$

PYTHON_INCLUDES:=$(shell python3-config --includes) -isystem $(shell python -c "import numpy; print(numpy.get_include())")
PYTHON_LIB_DIR:=$(shell python3-config --prefix)/lib
ifeq ($(shell echo ${PYVERSION} | egrep "${PY8_REGEX}"),)
	PYTHON_LIBS:=$(shell python3-config --libs)
else
	PYTHON_LIBS:=$(shell python3-config --libs --embed)
endif
PYTHON_LD_FLAGS:=-Wl,-rpath,$(PYTHON_LIB_DIR) -L$(PYTHON_LIB_DIR) $(PYTHON_LIBS)

$(info $(PYVERSION))
$(info $(PY8_REGEX))
$(info $(PYTHON_LIBS))

$(TRGT): setup $(OBJ)
	$(CXX) $(OBJ) $(PYTHON_LD_FLAGS) -o $@

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
