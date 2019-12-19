#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>
#include "malen/convert_cpp_to_py.h"

namespace malen
{
inline PyObject* make_py_tuple(std::size_t size)
{
    PyObject *args = PyTuple_New(size);
    if (!args)
    {
        throw std::runtime_error("Could not create a Python tuple!");
    }
    return args;
}

inline PyObject* set_arguments(PyObject *args, std::size_t)
{
    if (!args)
    {
        return PyTuple_New(0);
    }
    return args;
}

template<typename T>
inline void set_argument(PyObject *args, std::size_t idx, const T &t)
{
    if (PyTuple_SetItem(args, idx, convert_to_python(t)))
    {
        throw std::runtime_error("Could not pack the argument at index " + std::to_string(idx) + " into the argument tuple!");
    }
}

template<typename T, typename... V>
inline PyObject* set_arguments(PyObject *args, std::size_t idx, T t, V... v)
{
    if (!args)
    {
        args = make_py_tuple(sizeof...(v) + 1);
    }
    set_argument(args, idx, t);
    set_arguments(args, idx+1, v...);
    return args;
}
}