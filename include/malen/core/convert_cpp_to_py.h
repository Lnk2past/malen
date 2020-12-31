#pragma once
#include <Python.h>
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace malen
{
static inline PyObject* py_cast(PyObject* c)
{
    return c;
}

static inline PyObject* py_cast(const int c)
{
    return PyLong_FromLong(c);
}

static inline PyObject* py_cast(const unsigned int c)
{
    return PyLong_FromUnsignedLong(c);
}

static inline PyObject* py_cast(const long c)
{
    return PyLong_FromLong(c);
}

static inline PyObject* py_cast(const unsigned long c)
{
    return PyLong_FromUnsignedLong(c);
}

static inline PyObject* py_cast(const long long c)
{
    return PyLong_FromLongLong(c);
}

static inline PyObject* py_cast(const unsigned long long c)
{
    return PyLong_FromUnsignedLongLong(c);
}

static inline PyObject* py_cast(const double c)
{
    return PyFloat_FromDouble(c);
}

static inline PyObject* py_cast(const bool c)
{
    return PyBool_FromLong(static_cast<long>(c));
}

static inline PyObject* py_cast(const char *c)
{
    return PyUnicode_FromString(c);
}

static inline PyObject* py_cast(const std::string &c)
{
    return PyUnicode_FromString(c.c_str());
}

template <typename T, typename std::enable_if_t<std::is_pointer<T>::value>* = nullptr>
static inline PyObject* py_cast(const T c)
{
    return PyLong_FromLong(reinterpret_cast<std::intptr_t>(c));
}

template <template<typename...> class C, typename T>
static inline PyObject* py_cast(const C<T> &c)
{
    PyObject *py_list = PyList_New(c.size());
    for (std::size_t idx = 0; idx < c.size(); ++idx)
    {
        if (PyList_SetItem(py_list, idx, py_cast(c[idx])))
        {
            throw std::runtime_error("Could not set the data at index " + std::to_string(idx) + "!");
        }
    }
    return py_list;
}

template <typename KT, typename VT>
static inline PyObject* py_cast(const std::map<KT, VT> &c)
{
    PyObject *py_dict = PyDict_New();
    for (const auto &p : c)
    {
        if (PyDict_SetItem(py_dict, py_cast(p.first), py_cast(p.second)))
        {
            throw std::runtime_error("Could not set the data at key!");
        }
    }
    return py_dict;
}

#ifdef _MALENABLE_NUMPY
extern "C"
{
#ifndef NPY_NO_DEPRECATED_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include <numpy/arrayobject.h>
}

template <typename T>
struct numpy_type
{};

template <>
struct numpy_type<std::int8_t>
{
    static inline constexpr int typenum = NPY_INT8;
};

template <>
struct numpy_type<std::int16_t>
{
    static inline constexpr int typenum = NPY_INT16;
};

template <>
struct numpy_type<std::int32_t>
{
    static inline constexpr int typenum = NPY_INT32;
};

template <>
struct numpy_type<std::int64_t>
{
    static inline constexpr int typenum = NPY_INT64;
};

template <>
struct numpy_type<std::uint8_t>
{
    static inline constexpr int typenum = NPY_UINT8;
};

template <>
struct numpy_type<std::uint16_t>
{
    static inline constexpr int typenum = NPY_UINT16;
};

template <>
struct numpy_type<std::uint32_t>
{
    static inline constexpr int typenum = NPY_UINT32;
};

template <>
struct numpy_type<std::uint64_t>
{
    static inline constexpr int typenum = NPY_UINT64;
};

template <>
struct numpy_type<float>
{
    static inline constexpr int typenum = NPY_FLOAT;
};

template <>
struct numpy_type<double>
{
    static inline constexpr int typenum = NPY_DOUBLE;
};

template<class T>
static inline constexpr int numpy_type_v = numpy_type<T>::typenum;

template <typename T>
static inline PyObject* py_cast(const std::vector<T> &c)
{
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    npy_intp dims[1] = {static_cast<npy_intp>(c.size())};
    PyObject *np_array = PyArray_SimpleNewFromData(
        1,
        dims,
        numpy_type_v<T>,
        static_cast<void*>(const_cast<T*>(c.data()))
    );
    
    PyGILState_Release(gstate);

    return np_array;
}

template <typename T>
static inline PyObject* py_cast(const std::vector<std::vector<T>> &c)
{
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    if (c.size() == 0)
    {
        npy_intp dims[2] = {0, 0};
        return PyArray_EMPTY(2, dims, numpy_type_v<T>, 0);
    }

    PyObject *npas = PyTuple_New(c.size());
    for (std::size_t i = 0; i < c.size(); ++i)
    {
        PyTuple_SET_ITEM(npas, i, py_cast(c[i]));
    }
    PyObject *np_arrays = PyArray_Concatenate(npas, 0);

    npy_intp _new_dims[2] = {static_cast<npy_intp>(c.size()), static_cast<npy_intp>(c[0].size())};
    PyArray_Dims new_dims {_new_dims, 2};

    PyArray_Resize(reinterpret_cast<PyArrayObject*>(np_arrays), &new_dims, 0, NPY_CORDER);

    PyGILState_Release(gstate);

    return np_arrays;
}

template <typename T>
static inline PyObject* py_cast(const std::vector<std::vector<std::vector<T>>> &c)
{
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    if (c.size() == 0)
    {
        npy_intp dims[3] = {0, 0, 0};
        return PyArray_EMPTY(3, dims, numpy_type_v<T>, 0);
    }

    PyObject *npas = PyTuple_New(c.size());
    for (std::size_t i = 0; i < c.size(); ++i)
    {
        PyTuple_SET_ITEM(npas, i, py_cast(c[i]));
    }
    PyObject *np_arrays = PyArray_Concatenate(npas, 0);

    npy_intp _new_dims[3] = {static_cast<npy_intp>(c.size()), static_cast<npy_intp>(c[0].size()), static_cast<npy_intp>(c[0][0].size())};
    PyArray_Dims new_dims {_new_dims, 3};

    PyArray_Resize(reinterpret_cast<PyArrayObject*>(np_arrays), &new_dims, 0, NPY_CORDER);

    PyGILState_Release(gstate);

    return np_arrays;
}
#endif

}
