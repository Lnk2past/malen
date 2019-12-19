// Malen.h
//
// Note: This must be included first! The Python.h header must come first within
// a translation unit!
//
#pragma once

#include <Python.h>

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "malen/py_conversions.h"
#include "malen/py_kwarg.h"

namespace malen
{
class Malen
{
public:
    Malen(const std::string &module_name, const std::vector<std::string> &additional_paths = std::vector<std::string>())
    {
        Py_Initialize();
        add_to_path(".");
        for (auto path : additional_paths)
        {
            add_to_path(path);
        }
        py_module = PyImport_ImportModule(module_name.c_str());
        if (!py_module)
        {
            PyErr_PrintEx(1);
            throw std::runtime_error("There were problems loading the " + module_name + " module.");
        }
    }

    ~Malen()
    {
        Py_Finalize();
    }

    template<typename... V>
    PyObject* operator()(const std::string &handle_name, PyObject *kwargs, V... v)
    {
        PyObject *args = set_arguments(nullptr, 0, v...);
        PyObject *pyRetval = PyObject_Call(get_python_method(handle_name), args, kwargs);
        if (!pyRetval)
        {
            PyErr_PrintEx(1);
            throw std::runtime_error("Failed to invoke the " +  handle_name + " handle!");
        }
        Py_INCREF(pyRetval);
        // Py_DECREF(args);
        // TODO: Determine issue with this Py_DECREF...
        return pyRetval;
    }

private:
    Malen(const Malen&) = delete;
    void operator=(const Malen&) = delete;

    inline void add_to_path(const std::string &path)
    {
        PyList_Append(PySys_GetObject("path"), convert_to_python(path));
    }

    inline PyObject* get_python_method(const std::string &method_name)
    {
        if (py_methods.find(method_name) == std::end(py_methods))
        {
            PyObject *handle = PyObject_GetAttrString(py_module, method_name.c_str());
            if (!handle)
            {
                throw std::runtime_error("There were problems loading the " + method_name + " method.");
            }
            py_methods[method_name] = handle;
        }
        return py_methods[method_name];
    }

    inline PyObject* make_py_tuple(std::size_t size)
    {
        PyObject *args = PyTuple_New(size);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }
        return args;
    }

    inline void set_arguments(PyObject *, std::size_t)
    {}

    template<typename T>
    inline void set_argument(PyObject *args, std::size_t idx, const T &t)
    {
        if (PyTuple_SetItem(args, idx, convert_to_python(t)))
        {
            throw std::runtime_error("Could not pack the argument at index " + std::to_string(idx) + " into the argument tuple!");
        }
    }

    template<typename T, typename... V>
    inline PyObject* set_arguments(PyObject *args, std::size_t idx, T t, V... v)
    {
        if (!args)
        {
            args = make_py_tuple(sizeof...(v) + 1);
        }
        set_argument(args, idx, t);
        set_arguments(args, idx+1, v...);
        return args;
    }

    PyObject* build_kwargs(PyObject *kwargs)
    {
        return kwargs;
    }

    template<typename K, typename... KW>
    PyObject* build_kwargs(PyObject *kwargs, const K &kw, KW... kws)
    {
        if (!kwargs)
        {
            kwargs = PyDict_New();
        }
        PyDict_SetItem(kwargs, kw.first, kw.second);
        build_kwargs(kwargs, kws...);
        return kwargs;
    }

    PyObject *py_module = nullptr;
    std::unordered_map<std::string, PyObject*> py_methods = {};
};
}
