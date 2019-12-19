#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>

namespace malen
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

inline void convert_from_python(PyObject*p, int &c)
{
    c = PyLong_AsLong(p);
}

template <template<typename...> class C, typename T>
inline void convert_from_python(PyObject* p, C<T> &c)
{
    for (Py_ssize_t idx = 0; idx < PyList_Size(p); ++idx)
    {
        auto con = PyList_GetItem(p, idx);
        if (!con)
        {
            throw std::runtime_error("Could not get the data at index " + std::to_string(idx) + "!");
        }
        convert_from_python(con, *c.emplace(std::end(c)));
    }
}

}
