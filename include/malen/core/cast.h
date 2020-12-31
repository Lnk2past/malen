#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>

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
