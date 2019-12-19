#pragma once
#include <Python.h>
#include <utility>
#include "malen/py_conversions.h"

namespace malen
{
template <typename KT, typename VT>
inline std::pair<PyObject*, PyObject*> kwarg(const KT &key, const VT &val)
{
    return {convert_to_python(key), convert_to_python(val)};
}

template <typename KT, template<typename...> class VC, typename VT>
inline  std::pair<PyObject*, PyObject*> kwarg(const KT &key, const VC<VT> &val)
{
    return {convert_to_python(key), convert_to_python(val)};
}

inline PyObject* build_kwargs(PyObject *kwargs)
{
    return kwargs;
}

template<typename K, typename... KW>
inline PyObject* build_kwargs(PyObject *kwargs, const K &kw, KW... kws)
{
    if (!kwargs)
    {
        kwargs = PyDict_New();
    }
    PyDict_SetItem(kwargs, kw.first, kw.second);
    build_kwargs(kwargs, kws...);
    return kwargs;
}
}
