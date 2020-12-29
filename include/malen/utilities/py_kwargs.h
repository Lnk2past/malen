#pragma once
#include <Python.h>
#include <utility>
#include "malen/utilities/convert_cpp_to_py.h"

namespace malen
{
template <typename KT, typename VT>
inline std::pair<PyObject*, PyObject*> kwarg(const KT &key, const VT &val)
{
    return {py_cast(key), py_cast(val)};
}

template <typename KT, template<typename...> class VC, typename VT>
inline std::pair<PyObject*, PyObject*> kwarg(const KT &key, const VC<VT> &val)
{
    return {py_cast(key), py_cast(val)};
}

inline PyObject* _kwargs(PyObject *py_kwargs)
{
    return py_kwargs;
}

template<typename... KW>
inline PyObject* _kwargs(PyObject *py_kwargs, const std::pair<PyObject*, PyObject*> &kw, KW... kws)
{
    if (!py_kwargs)
    {
        py_kwargs = PyDict_New();
    }
    PyDict_SetItem(py_kwargs, kw.first, kw.second);
    _kwargs(py_kwargs, kws...);
    return py_kwargs;
}

template<typename VT, typename... KW>
inline PyObject* _kwargs(PyObject *py_kwargs, const std::string &key, const VT &value, KW... kws)
{
    if (!py_kwargs)
    {
        py_kwargs = PyDict_New();
    }
    PyDict_SetItem(py_kwargs, py_cast(key), py_cast(value));
    _kwargs(py_kwargs, kws...);
    return py_kwargs;
}

template<typename... KW>
inline PyObject* kwargs(KW... kws)
{
    return _kwargs(nullptr, kws...);
}
}
