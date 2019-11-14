// PythonVisualizer.h
//
// Note: This must be included first! The Python.h header must come first within
// a translation unit!
//
#pragma once

#include <Python.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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

template <typename KT, typename VT>
std::pair<PyObject*, PyObject*> kwarg(const KT &key, const VT &val)
{
    return {convert_to_python(key), convert_to_python(val)};
}

class PythonVisualizer
{
public:
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

        make_new_figure_handle = load_method("make_new_figure");
        plot_handle = load_method("plot");
        plot_color_handle = load_method("plot_color");
        image_handle = load_method("image");
        image_slider_handle = load_method("image_slider");
        generate_html_handle = load_method("generate_html");
    }

    ~PythonVisualizer()
    {
        Py_Finalize();
    }

    template <typename... KW>
    PyObject* make_new_figure(const std::string &title, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(make_new_figure_handle, __FUNCTION__ , kwargs, title);
    }

    template <template<typename...> class C1, template<typename...> class C2, typename T1, typename T2, typename... KW>
    PyObject* plot(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(plot_handle, __FUNCTION__, kwargs, figure, plot_type, datax, datay);
    }

    template <template<typename...> class C1, template<typename...> class C2, template<typename...> class C3, typename T1, typename T2, typename T3, typename... KW>
    PyObject* plot_color(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, const C3<T3> &datacolor, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(plot_color_handle, __FUNCTION__, kwargs, figure, plot_type, datax, datay, datacolor);
    }

    template <template<typename...> class C, typename T, typename... KW>
    PyObject* image(PyObject* figure, const C<C<T>> &image, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(image_handle, __FUNCTION__, kwargs, figure, image);
    }

    template <template<typename...> class C, typename T, typename... KW>
    PyObject* image_slider(PyObject* figure, const C<C<C<T>>> &images, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(image_slider_handle, __FUNCTION__, kwargs, figure, images);
    }

    template <typename... KW>
    void generate_html(PyObject *figure, const std::string &filename, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        py_call_object(generate_html_handle, __FUNCTION__, kwargs, figure, filename);
    }

private:
    PythonVisualizer(const PythonVisualizer&) = delete;
    void operator=(const PythonVisualizer&) = delete;

    inline void add_to_path(const std::string &path)
    {
        PyList_Append(PySys_GetObject("path"), convert_to_python(path));
    }

    inline PyObject* load_method(const std::string &method_name)
    {
        PyObject *handle = PyObject_GetAttrString(py_module, method_name.c_str());
        if (!handle)
        {
            throw std::runtime_error("There were problems loading the Visualizer." + method_name + " method.");
        }
        return handle;
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

    template<typename... V>
    inline PyObject* py_call_object(PyObject *handle, const std::string &handle_name, PyObject *kwargs, V... v)
    {
        PyObject *args = set_arguments(nullptr, 0, v...);
        PyObject *pyRetval = PyObject_Call(handle, args, kwargs);
        if (!pyRetval)
        {
            PyErr_PrintEx(1);
            throw std::runtime_error("Failed to invoke the " +  handle_name + " handle!");
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
