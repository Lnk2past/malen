#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>

namespace py
{
inline PyObject* convert_to_python(PyObject* c)
{
    return c;
}

inline PyObject* convert_to_python(const int c)
{
    return PyLong_FromLong(c);
}

inline PyObject* convert_to_python(const unsigned int c)
{
    return PyLong_FromUnsignedLong(c);
}

inline PyObject* convert_to_python(const unsigned long c)
{
    return PyLong_FromUnsignedLong(c);
}

inline PyObject* convert_to_python(const double c)
{
    return PyFloat_FromDouble(c);
}

inline PyObject* convert_to_python(const std::string &c)
{
    return PyUnicode_FromString(c.c_str());
}

template <template<typename...> class C, typename T>
inline PyObject* convert_to_python(const C<T> &c)
{
    PyObject *py_list = PyList_New(c.size());
    for (std::size_t idx = 0; idx < c.size(); ++idx)
    {
        if (PyList_SetItem(py_list, idx, convert_to_python(c[idx])))
        {
            throw std::runtime_error("Could not set the data at index " + std::to_string(idx) + "!");
        }
    }
    return py_list;
}
}
