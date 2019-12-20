#pragma once
#include <Python.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "malen/utilities/convert_cpp_to_py.h"
#include "malen/utilities/convert_py_to_cpp.h"
#include "malen/utilities/py_args.h"
#include "malen/utilities/py_kwargs.h"

namespace malen
{
class Malen
{
public:
    Malen(const std::string &module_name, const std::vector<std::string> &additional_paths = std::vector<std::string>())
    {
        Py_Initialize();
        for (auto path : additional_paths)
        {
            PyList_Append(PySys_GetObject("path"), convert_to_python(path));
        }
        py_module = PyImport_ImportModule(module_name.c_str());
        if (!py_module)
        {
            PyErr_PrintEx(1);
            throw std::runtime_error("There were problems loading the " + module_name + " module.");
        }
    }

    virtual ~Malen()
    {
        if (Py_IsInitialized() != 0)
        {
            Py_Finalize();
        }
    }

    PyObject* invoke(const std::string &handle_name, PyObject *py_args, PyObject *py_kwargs = nullptr)
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

protected:
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

    PyObject *py_module = nullptr;
    std::unordered_map<std::string, PyObject*> py_methods = {};

private:
    Malen(const Malen&) = delete;
    void operator=(const Malen&) = delete;
};
}
