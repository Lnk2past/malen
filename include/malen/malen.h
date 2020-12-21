#pragma once
#include <Python.h>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "malen/utilities/convert_cpp_to_py.h"
#include "malen/utilities/convert_py_to_cpp.h"
#include "malen/utilities/py_args.h"
#include "malen/utilities/py_kwargs.h"

#ifdef _MALENABLE_NUMPY
#ifndef NPY_NO_DEPRECATED_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#endif

namespace malen
{
class Malen
{
public:
    Malen(const std::vector<std::string> &additional_paths = std::vector<std::string>())
    {
        Py_Initialize();
        #ifdef _MALENABLE_NUMPY
        _initialize_numpy();
        #endif

        add_to_path(additional_paths);
    }

    Malen(const std::string &module_name, const std::vector<std::string> &additional_paths = std::vector<std::string>()):
        Malen(additional_paths)
    {
        load_python_module(module_name);
        if (!py_modules[module_name])
        {
            PyErr_PrintEx(1);
            throw std::runtime_error("There were problems loading the " + module_name + " module.");
        }
    }

    Malen(const std::map<std::string, std::vector<std::string>> &modules_and_methods, const std::vector<std::string> &additional_paths = std::vector<std::string>()):
        Malen(additional_paths)
    {
        for (auto module_details : modules_and_methods)
        {
            load_python_module(module_details.first);
            for (auto &method_name: module_details.second)
            {
                load_python_method(module_details.first, method_name);
            }
        }
    }

    virtual ~Malen()
    {
        if (Py_IsInitialized() != 0)
        {
            Py_Finalize();
        }
    }

    inline void add_to_path(const std::vector<std::string> &additional_paths)
    {
        for (auto path : additional_paths)
        {
            PyList_Append(PySys_GetObject("path"), convert_to_python(path));
        }
    }

    inline PyObject* load_python_module(const std::string &module_name)
    {
        if (py_modules.find(module_name) == std::end(py_modules))
        {
            PyObject *py_module = PyImport_ImportModule(module_name.c_str());
            if (!py_module)
            {
                PyErr_PrintEx(1);
                throw std::runtime_error("There were problems loading the " + module_name + " module.");
            }
            py_modules[module_name] = py_module;
        }
        return py_modules[module_name];
    }

    inline PyObject* load_python_method(const std::string &module_name, const std::string &method_name)
    {
        if (py_methods.find(method_name) == std::end(py_methods))
        {
            PyObject *handle = PyObject_GetAttrString(load_python_module(module_name), method_name.c_str());
            if (!handle)
            {
                throw std::runtime_error("There were problems loading the " + method_name + " method.");
            }
            py_methods[method_name] = handle;
        }
        return py_methods[method_name];
    }

    inline PyObject* get_python_method(const std::string &method_name)
    {
        if (py_methods.find(method_name) == std::end(py_methods))
        {
            throw std::runtime_error("No method " + method_name + " loaded.");
        }
        return py_methods[method_name];
    }

    inline PyObject* invoke(const std::string &handle_name, PyObject *py_args, PyObject *py_kwargs = nullptr)
    {
        if (!py_args)
        {
            py_args = args();
        }
        PyObject *pyRetval = PyObject_Call(get_python_method(handle_name), py_args, py_kwargs);
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

    inline PyObject* invoke(const std::string &module_name, const std::string &handle_name, PyObject *py_args, PyObject *py_kwargs = nullptr)
    {
        load_python_method(module_name, handle_name);
        return invoke(handle_name, py_args, py_kwargs);
    }

protected:
    std::unordered_map<std::string, PyObject*> py_modules = {};
    std::unordered_map<std::string, PyObject*> py_methods = {};

private:
    #ifdef _MALENABLE_NUMPY
    int _initialize_numpy() {
        return _import_array();
    };
    #endif

    Malen(const Malen&) = delete;
    void operator=(const Malen&) = delete;
};
}
