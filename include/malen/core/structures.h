#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <malen/core/convert_cpp_to_py.h>
#include <malen/core/convert_py_to_cpp.h>

namespace malen
{
static inline PyObject* make_py_tuple(std::size_t size)
{
    PyObject *py_tuple = PyTuple_New(size);
    if (!py_tuple)
    {
        throw std::runtime_error("Could not create a Python tuple!");
    }
    return py_tuple;
}

static inline PyObject* _tuple(PyObject *py_tuple, std::size_t)
{
    if (!py_tuple)
    {
        return PyTuple_New(0);
    }
    return py_tuple;
}

template<typename T>
static inline PyObject* _tuple(PyObject *py_tuple, std::size_t idx, const T &t)
{
    if (!py_tuple)
    {
        py_tuple = make_py_tuple(1);
    }
    if (PyTuple_SetItem(py_tuple, idx, py_cast(t)))
    {
        throw std::runtime_error("Could not pack the argument at index " + std::to_string(idx) + " into the argument tuple!");
    }
    return py_tuple;
}

template<typename T, typename... V>
static inline PyObject* _tuple(PyObject *py_tuple, std::size_t idx, T t, V... v)
{
    if (!py_tuple)
    {
        py_tuple = make_py_tuple(sizeof...(v) + 1);
    }
    _tuple(py_tuple, idx, t);
    _tuple(py_tuple, idx+1, v...);
    return py_tuple;
}

template<typename... V>
static inline PyObject* make_tuple(V... v)
{
    return _tuple(nullptr, 0, v...);
}

template <typename KT, typename VT>
static inline std::pair<PyObject*, PyObject*> item(const KT &key, const VT &val)
{
    return {py_cast(key), py_cast(val)};
}

template <typename KT, template<typename...> class VC, typename VT>
static inline std::pair<PyObject*, PyObject*> item(const KT &key, const VC<VT> &val)
{
    return {py_cast(key), py_cast(val)};
}

static inline PyObject* _dict(PyObject *py_dict)
{
    return py_dict;
}

template<typename... KW>
static inline PyObject* _dict(PyObject *py_dict, const std::pair<PyObject*, PyObject*> &kw, KW... kws)
{
    if (!py_dict)
    {
        py_dict = PyDict_New();
    }
    PyDict_SetItem(py_dict, kw.first, kw.second);
    _dict(py_dict, kws...);
    return py_dict;
}

template<typename VT, typename... KW>
static inline PyObject* _dict(PyObject *py_dict, const std::string &key, const VT &value, KW... kws)
{
    if (!py_dict)
    {
        py_dict = PyDict_New();
    }
    PyDict_SetItem(py_dict, py_cast(key), py_cast(value));
    _dict(py_dict, kws...);
    return py_dict;
}

template<typename... KW>
static inline PyObject* make_dict(KW... kws)
{
    return _dict(nullptr, kws...);
}

}