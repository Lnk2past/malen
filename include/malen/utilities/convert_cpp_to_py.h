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

inline PyObject* convert_to_python(const long c)
{
    return PyLong_FromLong(c);
}

inline PyObject* convert_to_python(const unsigned long c)
{
    return PyLong_FromUnsignedLong(c);
}

inline PyObject* convert_to_python(const long long c)
{
    return PyLong_FromLongLong(c);
}

inline PyObject* convert_to_python(const unsigned long long c)
{
    return PyLong_FromUnsignedLongLong(c);
}

inline PyObject* convert_to_python(const double c)
{
    return PyFloat_FromDouble(c);
}

inline PyObject* convert_to_python(const std::string &c)
{
    return PyUnicode_FromString(c.c_str());
}

inline PyObject* convert_to_python(const char *c)
{
    return PyUnicode_FromString(c);
}

inline PyObject* convert_to_python(const bool c)
{
    return PyBool_FromLong(static_cast<long>(c));
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

#ifdef _MALENABLE_NUMPY
extern "C"
{
#ifndef NPY_NO_DEPRECATED_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include <numpy/arrayobject.h>
}
#include <cstdint>
#include <type_traits>

template <typename T>
struct numpy_type
{};

template <>
struct numpy_type<std::int8_t>
{
    static constexpr int typenum = NPY_INT8;
};

template <>
struct numpy_type<std::int16_t>
{
    static constexpr int typenum = NPY_INT16;
};

template <>
struct numpy_type<std::int32_t>
{
    static constexpr int typenum = NPY_INT32;
};

template <>
struct numpy_type<std::int64_t>
{
    static constexpr int typenum = NPY_INT64;
};

template <>
struct numpy_type<std::uint8_t>
{
    static constexpr int typenum = NPY_UINT8;
};

template <>
struct numpy_type<std::uint16_t>
{
    static constexpr int typenum = NPY_UINT16;
};

template <>
struct numpy_type<std::uint32_t>
{
    static constexpr int typenum = NPY_UINT32;
};

template <>
struct numpy_type<std::uint64_t>
{
    static constexpr int typenum = NPY_UINT64;
};

template <>
struct numpy_type<float>
{
    static constexpr int typenum = NPY_FLOAT;
};

template <>
struct numpy_type<double>
{
    static constexpr int typenum = NPY_DOUBLE;
};

template<class T>
inline static constexpr int numpy_type_v = numpy_type<T>::typenum;

template <typename T>
inline PyObject* convert_to_python(const std::vector<T> &c)
{
    npy_intp dims[1] = {static_cast<npy_intp>(c.size())};
    PyObject *np_array = PyArray_SimpleNewFromData(
        1,
        dims,
        numpy_type_v<T>,
        static_cast<void*>(const_cast<T*>(c.data()))
    );
    return np_array;
}

template <typename T>
inline PyObject* convert_to_python(const std::vector<std::vector<T>> &c)
{
    if (c.size() == 0)
    {
        npy_intp dims[2] = {0, 0};
        return PyArray_EMPTY(2, dims, numpy_type_v<T>, 0);
    }

    PyObject *npas = PyTuple_New(c.size());
    for (std::size_t i = 0; i < c.size(); ++i)
    {
        PyTuple_SET_ITEM(npas, i, convert_to_python(c[i]));
    }
    PyObject *np_arrays = PyArray_Concatenate(npas, 0);

    npy_intp _new_dims[2] = {static_cast<npy_intp>(c.size()), static_cast<npy_intp>(c[0].size())};
    PyArray_Dims new_dims {_new_dims, 2};

    PyArray_Resize(reinterpret_cast<PyArrayObject*>(np_arrays), &new_dims, 0, NPY_CORDER);

    return np_arrays;
}

template <typename T>
inline PyObject* convert_to_python(const std::vector<std::vector<std::vector<T>>> &c)
{
    if (c.size() == 0)
    {
        npy_intp dims[3] = {0, 0, 0};
        return PyArray_EMPTY(3, dims, numpy_type_v<T>, 0);
    }

    PyObject *npas = PyTuple_New(c.size());
    for (std::size_t i = 0; i < c.size(); ++i)
    {
        PyTuple_SET_ITEM(npas, i, convert_to_python(c[i]));
    }
    PyObject *np_arrays = PyArray_Concatenate(npas, 0);

    npy_intp _new_dims[3] = {static_cast<npy_intp>(c.size()), static_cast<npy_intp>(c[0].size()), static_cast<npy_intp>(c[0][0].size())};
    PyArray_Dims new_dims {_new_dims, 3};

    PyArray_Resize(reinterpret_cast<PyArrayObject*>(np_arrays), &new_dims, 0, NPY_CORDER);

    return np_arrays;
}
#endif
}
