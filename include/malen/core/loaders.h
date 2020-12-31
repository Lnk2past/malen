#pragma once
#include <Python.h>
#include <map>
#include <stdexcept>
#include <string>

namespace malen
{

static inline PyObject* load_python_module(const std::string &module_name)
{
    if (py_modules.find(module_name) == std::end(py_modules))
    {
        PyObject *py_module = PyImport_ImportModule(module_name.c_str());
        if (!py_module)
        {
            PyErr_PrintEx(1);
            throw std::runtime_error("There were problems loading the " + module_name + " module.");
        }
        py_modules[module_name].module = py_module;
    }
    return py_modules[module_name].module;
}

static inline PyObject* load_python_attribute(PyObject *object, const std::string &attribute_name)
{
    PyObject *handle = PyObject_GetAttrString(object, attribute_name.c_str());
    if (!handle)
    {
        throw std::runtime_error("There were problems loading the " + attribute_name + " attribute.");
    }
    return handle;
}

static inline PyObject* load_python_attribute(const std::string &module_name, const std::string &attribute_name)
{
    return load_python_attribute(load_python_module(module_name), attribute_name);
}

static inline PyObject* load_python_method(PyObject *object, const std::string &method_name)
{
    auto handle = load_python_attribute(object, method_name);
    if (!handle)
    {
        throw std::runtime_error("There were problems loading the " + method_name + " method.");
    }
    return handle;
}

static inline PyObject* load_python_method(const std::string &module_name, const std::string &method_name)
{
    return load_python_method(load_python_module(module_name), method_name);
}

static inline void load_python_module_and_methods(const std::map<std::string, std::vector<std::string>> &modules_and_methods)
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

static inline PyObject* get_python_module(const std::string &module_name)
{
    if (py_modules.find(module_name) == std::end(py_modules))
    {
        throw std::runtime_error("No module " + module_name + " loaded.");
    }
    return py_modules[module_name].module;
}

}
