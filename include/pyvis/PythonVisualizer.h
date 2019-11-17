// PythonVisualizer.h
//
// Note: This must be included first! The Python.h header must come first within
// a translation unit!
//
#pragma once

#include <Python.h>

#include <stdexcept>
#include <string>
#include <utility>
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

inline PyObject* convert_to_python(const unsigned int c)
{
    return PyLong_FromUnsignedLong(c);
}

inline PyObject* convert_to_python(const unsigned long c)
{
    return PyLong_FromUnsignedLong(c);
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

template <typename KT, template<typename...> class VC, typename VT>
std::pair<PyObject*, PyObject*> kwarg(const KT &key, const VC<VT> &val)
{
    return {convert_to_python(key), convert_to_python(val)};
}

class PythonVisualizer
{
public:
    PythonVisualizer(const std::vector<std::string> &additional_paths = std::vector<std::string>())
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
            PyErr_PrintEx(1);
            throw std::runtime_error("There were problems loading the Visualizer module.");
        }

        make_new_figure_handle = load_method("make_new_figure");
        plot_handle = load_method("plot");
        image_handle = load_method("image");
        slider_handle = load_method("slider");
        ticker_handle = load_method("ticker");
        color_mapper_handle = load_method("color_mapper");
        color_bar_handle = load_method("color_bar");
        layout_handle = load_method("layout");
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

    template <template<typename...> class C, typename T, typename... KW>
    PyObject* image(PyObject* figure, const C<C<T>> &image, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(image_handle, __FUNCTION__, kwargs, figure, image);
    }

    template <typename... KW>
    PyObject* slider(PyObject* renderer, const std::string &title, const std::size_t start, const std::size_t end, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(slider_handle, __FUNCTION__, kwargs, renderer, title, start, end);
    }

    template <typename... KW>
    PyObject* ticker(const std::string &ticker_type, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(ticker_handle, __FUNCTION__, kwargs, ticker_type);
    }

    template <typename... KW>
    PyObject* color_mapper(const std::string &mapper_type, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        return py_call_object(color_mapper_handle, __FUNCTION__, kwargs, mapper_type);
    }

    template <typename... KW>
    void color_bar(PyObject *figure, KW... kw)
    {
        auto kwargs = build_kwargs(nullptr, kw...);
        py_call_object(color_bar_handle, __FUNCTION__, kwargs, figure);
    }

    PyObject* layout(PyObject* obj1, PyObject *obj2)
    {
        return py_call_object(layout_handle, __FUNCTION__, nullptr, obj1, obj2);
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
        // Py_DECREF(args);
        // TODO: Determine issue with this Py_DECREF...
        return pyRetval;
    }

    PyObject *py_module = nullptr;
    PyObject *make_new_figure_handle = nullptr;
    PyObject *plot_handle = nullptr;
    PyObject *image_handle = nullptr;
    PyObject *slider_handle = nullptr;
    PyObject *ticker_handle = nullptr;
    PyObject *color_mapper_handle = nullptr;
    PyObject *color_bar_handle = nullptr;
    PyObject *layout_handle = nullptr;
    PyObject *generate_html_handle = nullptr;
};
}
