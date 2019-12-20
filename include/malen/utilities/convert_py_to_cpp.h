#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>

namespace malen
{
inline void convert_to_cpp(PyObject*p, int &c)
{
    c = PyLong_AsLong(p);
}

inline void convert_to_cpp(PyObject*p, unsigned int &c)
{
    c = PyLong_AsUnsignedLong(p);
}

inline void convert_to_cpp(PyObject*p, long &c)
{
    c = PyLong_AsLong(p);
}

inline void convert_to_cpp(PyObject*p, unsigned long &c)
{
    c = PyLong_AsUnsignedLong(p);
}

inline void convert_to_cpp(PyObject*p, long long &c)
{
    c = PyLong_AsLongLong(p);
}

inline void convert_to_cpp(PyObject*p, unsigned long long &c)
{
    c = PyLong_AsUnsignedLongLong(p);
}

inline void convert_to_cpp(PyObject*p, std::string &c)
{
    c = PyUnicode_AsUTF8(p);
}

template <template<typename...> class C, typename T>
inline void convert_to_cpp(PyObject* p, C<T> &c)
{
    for (Py_ssize_t idx = 0; idx < PyList_Size(p); ++idx)
    {
        auto con = PyList_GetItem(p, idx);
        if (!con)
        {
            throw std::runtime_error("Could not get the data at index " + std::to_string(idx) + "!");
        }
        #if __cplusplus >= 201703L
        convert_to_cpp(con, *c.emplace_back());
        #else
        convert_to_cpp(con, *c.emplace(std::end(c)));
        #endif
    }
}

}
