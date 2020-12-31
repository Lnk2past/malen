#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>

namespace malen
{
static inline void cpp_cast(PyObject*p, int &c)
{
    c = PyLong_AsLong(p);
}

static inline void cpp_cast(PyObject*p, unsigned int &c)
{
    c = PyLong_AsUnsignedLong(p);
}

static inline void cpp_cast(PyObject*p, long &c)
{
    c = PyLong_AsLong(p);
}

static inline void cpp_cast(PyObject*p, unsigned long &c)
{
    c = PyLong_AsUnsignedLong(p);
}

static inline void cpp_cast(PyObject*p, long long &c)
{
    c = PyLong_AsLongLong(p);
}

static inline void cpp_cast(PyObject*p, unsigned long long &c)
{
    c = PyLong_AsUnsignedLongLong(p);
}

static inline void cpp_cast(PyObject*p, std::string &c)
{
    c = PyUnicode_AsUTF8(p);
}

static inline void cpp_cast(PyObject*p, bool &c)
{
    c = static_cast<bool>(PyLong_AsLong(p));
}

template <template<typename...> class C, typename T>
static inline void cpp_cast(PyObject* p, C<T> &c)
{
    for (Py_ssize_t idx = 0; idx < PyList_Size(p); ++idx)
    {
        auto con = PyList_GetItem(p, idx);
        if (!con)
        {
            throw std::runtime_error("Could not get the data at index " + std::to_string(idx) + "!");
        }
        #if __cplusplus >= 201703L
        cpp_cast(con, *c.emplace_back());
        #else
        cpp_cast(con, *c.emplace(std::end(c)));
        #endif
    }
}

#ifdef _MALENABLE_NUMPY
extern "C"
{
#ifndef NPY_NO_DEPRECATED_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include <numpy/arrayobject.h>
}
#include <type_traits>
template <typename T, typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
static inline void cpp_cast(PyObject* p, std::vector<T> &c)
{
    auto npa = reinterpret_cast<PyArrayObject*>(p);
    void* data = PyArray_DATA(npa);
    c = std::vector<T>(static_cast<T*>(data), static_cast<T*>(data) + PyArray_SIZE(npa));
}

template <typename T, typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
static inline void cpp_cast(PyObject* p, std::vector<std::vector<T>> &c)
{
    auto npa = reinterpret_cast<PyArrayObject*>(p);
    void* data = PyArray_DATA(npa);
    auto dims = PyArray_DIMS(npa);
    c.clear();
    for (npy_intp ri = 0; ri < dims[0]; ++ri)
    {
        c.emplace_back(static_cast<T*>(data) + dims[1] * ri, static_cast<T*>(data) + dims[1] * (ri + 1));
    }
}

template <typename T, typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
static inline void cpp_cast(PyObject* p, std::vector<std::vector<std::vector<T>>> &c)
{
    auto npa = reinterpret_cast<PyArrayObject*>(p);
    void* data = PyArray_DATA(npa);
    auto dims = PyArray_DIMS(npa);
    c.clear();
    for (npy_intp di = 0; di < dims[2]; ++di)
    {
        c.emplace_back();
        for (npy_intp ri = 0; ri < dims[0]; ++ri)
        {
            auto soff = dims[1] * ri + di * dims[0] * dims[1];
            auto eoff = dims[1] * (ri + 1) + di * dims[0] * dims[1];
            c.back().emplace_back(static_cast<T*>(data) + soff, static_cast<T*>(data) + eoff);
        }
    }
}
#endif
}
