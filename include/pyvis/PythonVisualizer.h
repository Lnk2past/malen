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

namespace py
{
PyObject* convert_to_python(const int c)
{
    return PyLong_FromLong(c);
}

PyObject* convert_to_python(const double c)
{
    return PyFloat_FromDouble(c);
}

PyObject* convert_to_python(const std::string &c)
{
    return PyUnicode_FromString(c.c_str());
}

template <template<typename...> class C, typename T>
PyObject* convert_to_python(const C<T> &c)
{
    PyObject *py_list = PyList_New(c.size());
    for (size_t idx = 0; idx < c.size(); ++idx)
    {
        if (PyList_SetItem(py_list, idx, convert_to_python(c[idx])) != 0)
        {
            throw std::runtime_error("Could not set the data at index " + std::to_string(idx) + "!");
        }
    }
    return py_list;
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
        PyList_Append(PySys_GetObject("path"), PyUnicode_FromString("."));
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
        PyObject *args = PyTuple_New(1);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, PyUnicode_FromString(title.c_str())) != 0)
        {
            throw std::runtime_error("Could not pack the title into the argument tuple!");
        }

        PyObject *pyRetval = PyObject_CallObject(make_new_figure_handle, args);
        if (!pyRetval)
        {
            throw std::runtime_error("Failed to invoke the make_new_figure_handle!");
        }

        Py_INCREF(pyRetval);
        return pyRetval;
    }

    template <template<typename...> class C1, template<typename...> class C2, typename T1, typename T2>
    PyObject* plot(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, PyObject *kwargs=nullptr)
    {
        PyObject *args = PyTuple_New(4);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        if (PyTuple_SetItem(args, 1, PyUnicode_FromString(plot_type.c_str())) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the plot_type!");
        }

        if (PyTuple_SetItem(args, 2, convert_to_python(datax)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the x-data!");
        }

        if (PyTuple_SetItem(args, 3, convert_to_python(datay)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the y-data!");
        }

        PyObject *pyRetval = PyObject_Call(plot_handle, args, kwargs);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the plot_handle!");
        }

        Py_INCREF(pyRetval);
        Py_DECREF(args);
        return pyRetval;
    }

    template <template<typename...> class C1, template<typename...> class C2, template<typename...> class C3, typename T1, typename T2, typename T3>
    PyObject* plot_color(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, const C3<T3> &datacolor, PyObject *kwargs=nullptr)
    {
        PyObject *args = PyTuple_New(5);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        if (PyTuple_SetItem(args, 1, PyUnicode_FromString(plot_type.c_str())) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the plot_type!");
        }

        if (PyTuple_SetItem(args, 2, convert_to_python(datax)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the x-data!");
        }

        if (PyTuple_SetItem(args, 3, convert_to_python(datay)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the y-data!");
        }

        if (PyTuple_SetItem(args, 4, convert_to_python(datacolor)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the color-data!");
        }

        PyObject *pyRetval = PyObject_Call(plot_color_handle, args, kwargs);
        if (!pyRetval)
        {
            throw std::runtime_error("Failed to invoke the plot_color_handle!");
        }

        Py_INCREF(pyRetval);
        Py_DECREF(args);
        return pyRetval;
    }

    template <template<typename...> class C, typename T>
    PyObject* image(PyObject* figure, const C<C<T>> &image)
    {
        PyObject *args = PyTuple_New(2);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        if (PyTuple_SetItem(args, 1, convert_to_python(image)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the image!");
        }

        PyObject *pyRetval = PyObject_CallObject(image_handle, args);
        if (!pyRetval)
        {
            throw std::runtime_error("Failed to invoke the image_handle!");
        }

        Py_INCREF(pyRetval);
        Py_DECREF(args);
        return pyRetval;
    }

    template <template<typename...> class C, typename T>
    PyObject* image_slider(PyObject* figure, const C<C<C<T>>> &images)
    {
        PyObject *args = PyTuple_New(2);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        if (PyTuple_SetItem(args, 1, convert_to_python(images)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the image!");
        }

        PyObject *pyRetval = PyObject_CallObject(image_slider_handle, args);
        if (!pyRetval)
        {
            throw std::runtime_error("Failed to invoke the image_slider_handle!");
        }

        Py_INCREF(pyRetval);
        Py_DECREF(args);
        return pyRetval;
    }

    void generate_html(PyObject *figure, const std::string &filename)
    {
        PyObject *args = PyTuple_New(2);
        if (!args)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }
        if (PyTuple_SetItem(args, 1, PyUnicode_FromString(filename.c_str())) != 0)
        {
            throw std::runtime_error("Could not pack the filename into the argument tuple!");
        }

        PyObject *pyRetval = PyObject_CallObject(generate_html_handle, args);
        if (!pyRetval)
        {
            throw std::runtime_error("Failed to invoke the generate_html_handle!");
        }

        Py_DECREF(pyRetval);
    }

    template<typename KT, typename VT>
    PyObject* kwargs(const KT &key, const VT &val, PyObject *kw=nullptr)
    {
        if (!kw)
        {
            kw = PyDict_New();
        }
        if (PyDict_SetItem(kw, convert_to_python(key), convert_to_python(val)) != 0)
        {
            throw std::runtime_error("Failed add key/value pair to kwargs!");
        }
        return kw;
    }

    void add_to_path(const std::string &path)
    {
        PyList_Append(PySys_GetObject("path"), PyUnicode_FromString(path.c_str()));
    }

private:
    PyObject *py_module;
    PyObject *make_new_figure_handle;
    PyObject *plot_handle;
    PyObject *plot_color_handle;
    PyObject *image_handle;
    PyObject *image_slider_handle;
    PyObject *generate_html_handle;
};
}
