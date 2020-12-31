#pragma once
#include <Python.h>
#include <map>
#include <string>
#include <vector>
#include <malen/core/structure.h>

namespace malen
{

static PyObject* invoke(PyObject *object, PyObject *py_args, PyObject *py_kwargs = nullptr)
{
    if (!py_args)
    {
        py_args = make_tuple();
    }
    PyObject *pyRetval = PyObject_Call(object, py_args, py_kwargs);
    if (!pyRetval)
    {
        PyErr_PrintEx(1);
        throw std::runtime_error("Failed to invoke the handle!");
    }
    Py_INCREF(pyRetval);
    // Py_DECREF(args);
    // TODO: Determine issue with this Py_DECREF...
    return pyRetval;
}

static PyObject* invoke(PyObject *object, const std::string &method_name, PyObject *py_args, PyObject *py_kwargs = nullptr)
{
    return invoke(load_python_method(object, method_name), py_args, py_kwargs);
}

static PyObject* invoke(const std::string &module_name, const std::string &method_name, PyObject *py_args, PyObject *py_kwargs = nullptr)
{
    return invoke(load_python_method(module_name, method_name), py_args, py_kwargs);
}

}
