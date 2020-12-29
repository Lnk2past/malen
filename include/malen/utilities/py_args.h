#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>
#include "malen/utilities/convert_cpp_to_py.h"

namespace malen
{
inline PyObject* make_py_tuple(std::size_t size)
{
    PyObject *py_args = PyTuple_New(size);
    if (!py_args)
    {
        throw std::runtime_error("Could not create a Python tuple!");
    }
    return py_args;
}

inline PyObject* _args(PyObject *py_args, std::size_t)
{
    if (!py_args)
    {
        return PyTuple_New(0);
    }
    return py_args;
}

template<typename T>
inline PyObject* _args(PyObject *py_args, std::size_t idx, const T &t)
{
    if (!py_args)
    {
        py_args = make_py_tuple(1);
    }
    if (PyTuple_SetItem(py_args, idx, py_cast(t)))
    {
        throw std::runtime_error("Could not pack the argument at index " + std::to_string(idx) + " into the argument tuple!");
    }
    return py_args;
}

template<typename T, typename... V>
inline PyObject* _args(PyObject *py_args, std::size_t idx, T t, V... v)
{
    if (!py_args)
    {
        py_args = make_py_tuple(sizeof...(v) + 1);
    }
    _args(py_args, idx, t);
    _args(py_args, idx+1, v...);
    return py_args;
}

template<typename... V>
inline PyObject* args(V... v)
{
    return _args(nullptr, 0, v...);
}
}