// PythonVisualizer.h
//
// Note: This must be included first! The Python.h header must come first within
// a translation unit!
//
#pragma once

#include <Python.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <stdexcept>
#include <vector>

#define _M_QUOTE(seq) "\""#seq"\""
#define _M_PY_SET_ARGUMENT(args, idx, obj) set_argument(args, idx, obj, _M_QUOTE(obj))
#define _M_PY_CALL_OBJECT_(handle, args) py_call_object(handle, args, nullptr, _M_QUOTE(handle))
#define _M_PY_CALL_OBJECT_KW(handle, args, kwargs) py_call_object(handle, args, kwargs, _M_QUOTE(handle))

#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define _M_PY_CALL_OBJECT(...) GET_MACRO(__VA_ARGS__, _M_PY_CALL_OBJECT_KW, _M_PY_CALL_OBJECT_, )(__VA_ARGS__)

namespace py
{
namespace
{
inline PyObject* convert_to_python(PyObject* c)
{
    return c;
}

inline PyObject* convert_to_python(const int c)
{
    return PyLong_FromLong(c);
}

inline PyObject* convert_to_python(const double c)
{
    return PyFloat_FromDouble(c);
}

inline PyObject* convert_to_python(const std::string &c)
{
    return PyUnicode_FromString(c.c_str());
}

template <template<typename...> class C, typename T>
inline PyObject* convert_to_python(const C<T> &c)
{
    PyObject *py_list = PyList_New(c.size());
    for (size_t idx = 0; idx < c.size(); ++idx)
    {
        if (PyList_SetItem(py_list, idx, convert_to_python(c[idx])))
        {
            throw std::runtime_error("Could not set the data at index " + std::to_string(idx) + "!");
        }
    }
    return py_list;
}
}

class PythonVisualizer
{
public:
    PythonVisualizer(const PythonVisualizer&) = delete;
    void operator=(const PythonVisualizer&) = delete;

    PythonVisualizer(const std::vector<std::string> &additional_paths = std::vector<std::string>()) :
        py_module(nullptr),
        make_new_figure_handle(nullptr),
        plot_handle(nullptr),
        plot_color_handle(nullptr),
        image_handle(nullptr),
        image_slider_handle(nullptr),
        generate_html_handle(nullptr)
    {
        Py_Initialize();
        add_to_path(".");
        for (auto path : additional_paths)
        {
            add_to_path(path);
        }

        py_module = PyImport_ImportModule("Visualizer");
        if (!py_module)
        {
            throw std::runtime_error("There were problems loading the Visualizer module.");
        }

        make_new_figure_handle = PyObject_GetAttrString(py_module, "make_new_figure");
        if (!make_new_figure_handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer.make_new_figure method.");
        }

        plot_handle = PyObject_GetAttrString(py_module, "plot");
        if (!plot_handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer.plot method.");
        }

        plot_color_handle = PyObject_GetAttrString(py_module, "plot_color");
        if (!plot_color_handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer.plot_color method.");
        }

        image_handle = PyObject_GetAttrString(py_module, "image");
        if (!image_handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer.image method.");
        }

        image_slider_handle = PyObject_GetAttrString(py_module, "image_slider");
        if (!image_slider_handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer.image_slider method.");
        }

        generate_html_handle = PyObject_GetAttrString(py_module, "generate_html");
        if (!generate_html_handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer.generate_html method.");
        }
    }

    ~PythonVisualizer()
    {
        Py_Finalize();
    }

    PyObject* make_new_figure(const std::string &title)
    {
        PyObject *args = make_py_tuple(1);
        _M_PY_SET_ARGUMENT(args, 0, title);
        return _M_PY_CALL_OBJECT(make_new_figure_handle, args);
    }

    template <template<typename...> class C1, template<typename...> class C2, typename T1, typename T2>
    PyObject* plot(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, PyObject *kwargs=nullptr)
    {
        PyObject *args = make_py_tuple(4);
        _M_PY_SET_ARGUMENT(args, 0, figure);
        _M_PY_SET_ARGUMENT(args, 1, plot_type);
        _M_PY_SET_ARGUMENT(args, 2, datax);
        _M_PY_SET_ARGUMENT(args, 3, datay);
        return _M_PY_CALL_OBJECT(plot_handle, args, kwargs);
    }

    template <template<typename...> class C1, template<typename...> class C2, template<typename...> class C3, typename T1, typename T2, typename T3>
    PyObject* plot_color(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, const C3<T3> &datacolor, PyObject *kwargs=nullptr)
    {
        PyObject *args = make_py_tuple(5);
        _M_PY_SET_ARGUMENT(args, 0, figure);
        _M_PY_SET_ARGUMENT(args, 1, plot_type);
        _M_PY_SET_ARGUMENT(args, 2, datax);
        _M_PY_SET_ARGUMENT(args, 3, datay);
        _M_PY_SET_ARGUMENT(args, 4, datacolor);
        return _M_PY_CALL_OBJECT(plot_color_handle, args, kwargs);
    }

    template <template<typename...> class C, typename T>
    PyObject* image(PyObject* figure, const C<C<T>> &image)
    {
        PyObject *args = make_py_tuple(2);
        _M_PY_SET_ARGUMENT(args, 0, figure);
        _M_PY_SET_ARGUMENT(args, 1, image);
        return _M_PY_CALL_OBJECT(image_handle, args);
    }

    template <template<typename...> class C, typename T>
    PyObject* image_slider(PyObject* figure, const C<C<C<T>>> &images)
    {
        PyObject *args = make_py_tuple(2);
        _M_PY_SET_ARGUMENT(args, 0, figure);
        _M_PY_SET_ARGUMENT(args, 1, images);
        return _M_PY_CALL_OBJECT(image_slider_handle, args);
    }

    void generate_html(PyObject *figure, const std::string &filename)
    {
        PyObject *args = make_py_tuple(2);
        _M_PY_SET_ARGUMENT(args, 0, figure);
        _M_PY_SET_ARGUMENT(args, 1, filename);
        _M_PY_CALL_OBJECT(generate_html_handle, args);
    }

    template<typename KT, typename VT>
    PyObject* make_kwarg(const KT &key, const VT &val, PyObject *kw=nullptr)
    {
        if (!kw)
        {
            kw = PyDict_New();
        }
        if (PyDict_SetItem(kw, convert_to_python(key), convert_to_python(val)))
        {
            throw std::runtime_error("Failed add key/value pair to kwargs!");
        }
        return kw;
    }

private:
    inline void add_to_path(const std::string &path)
    {
        PyList_Append(PySys_GetObject("path"), convert_to_python(path));
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

    template<typename T>
    inline void set_argument(PyObject *args, std::size_t idx, const T &t, const std::string &obj_name)
    {
        if (PyTuple_SetItem(args, idx, convert_to_python(t)))
        {
            throw std::runtime_error("Could not pack the " + obj_name + " into the argument tuple!");
        }
    }

    inline PyObject* py_call_object(PyObject *handle, PyObject *args, PyObject *kwargs, const std::string &handle_name)
    {
        PyObject *pyRetval = PyObject_Call(handle, args, kwargs);
        if (!pyRetval)
        {
            throw std::runtime_error("Failed to invoke the " +  handle_name + "!");
        }
        Py_INCREF(pyRetval);
        Py_DECREF(args);
        return pyRetval;
    }

    PyObject *py_module;
    PyObject *make_new_figure_handle;
    PyObject *plot_handle;
    PyObject *plot_color_handle;
    PyObject *image_handle;
    PyObject *image_slider_handle;
    PyObject *generate_html_handle;
};
}
