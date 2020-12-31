#pragma once
#include <Python.h>
#include <map>
#include <string>
#include <vector>

#ifdef _MALENABLE_NUMPY
#ifndef NPY_NO_DEPRECATED_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#endif

#include <malen/core/loaders.h>
#include <malen/core/convert_cpp_to_py.h>

namespace malen
{
static inline void add_to_path(const std::vector<std::string> &additional_paths)
{
    for (auto path : additional_paths)
    {
        PyList_Append(PySys_GetObject("path"), py_cast(path));
    }
}

static inline void initialize(const std::vector<std::string> &additional_paths = std::vector<std::string>())
{
    Py_Initialize();
    #ifdef _MALENABLE_NUMPY
    _import_array();
    #endif

    add_to_path(additional_paths);
}

static inline void initialize(const std::string &module_name, const std::vector<std::string> &additional_paths = std::vector<std::string>())
{
    initialize(additional_paths);
    load_python_module(module_name);
}

static inline void initialize(const std::map<std::string, std::vector<std::string>> &modules_and_methods, const std::vector<std::string> &additional_paths = std::vector<std::string>())
{
    initialize(additional_paths);

    for (auto module_details : modules_and_methods)
    {
        load_python_module(module_details.first);
        for (auto &method_name: module_details.second)
        {
            load_python_attribute(module_details.first, method_name);
        }
    }
}

static inline void finalize()
{
    if (Py_IsInitialized() != 0)
    {
        Py_Finalize();
    }
}

}
