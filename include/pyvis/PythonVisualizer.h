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

class PythonVisualizer
{
public:
    PythonVisualizer(const PythonVisualizer&) = delete;
    void operator=(const PythonVisualizer&) = delete;

    PythonVisualizer(const std::vector<std::string> &additional_paths = std::vector<std::string>()) :
        py_module(nullptr),
        make_new_figure_handle(nullptr),
        line_handle(nullptr),
        circle_handle(nullptr),
        vbar_handle(nullptr),
        image_handle(nullptr),
        generate_html_handle(nullptr)
    {
        Py_Initialize();
        PyList_Append(PySys_GetObject("path"), PyUnicode_FromString("."));
        for (auto path : additional_paths)
        {
            add_to_path(path);
        }

        py_module = PyImport_ImportModule("Visualizer");
        if (py_module == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer module.");
        }

        make_new_figure_handle = PyObject_GetAttrString(py_module, "make_new_figure");
        if (make_new_figure_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.make_new_figure method.");
        }

        line_handle = PyObject_GetAttrString(py_module, "line");
        if (line_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.line method.");
        }

        circle_handle = PyObject_GetAttrString(py_module, "circle");
        if (circle_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.circle method.");
        }

        vbar_handle = PyObject_GetAttrString(py_module, "vbar");
        if (vbar_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.vbar method.");
        }

        image_handle = PyObject_GetAttrString(py_module, "image");
        if (image_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.image method.");
        }

        generate_html_handle = PyObject_GetAttrString(py_module, "generate_html");
        if (generate_html_handle == nullptr)
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
        if (args == nullptr)
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
    void line(PyObject* figure, const C1<T1> &datax, const C2<T2> &datay, const std::string &legend = "")
    {
        PyObject *args = PyTuple_New(3);
        if (args == nullptr)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        set_data(args, datax, datay);

        PyObject *kwargs = nullptr;
        if (!legend.empty())
        {
            kwargs = PyDict_New();
            PyDict_SetItemString(kwargs, "legend", PyUnicode_FromString(legend.c_str()));
        }

        PyObject *pyRetval = PyObject_Call(line_handle, args, kwargs);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the line_handle!");
        }

        Py_DECREF(pyRetval);
    }

    template <template<typename...> class C1, template<typename...> class C2, typename T1, typename T2>
    void circle(PyObject* figure, const C1<T1> &datax, const C2<T2> &datay, const std::string &legend = "")
    {
        PyObject *args = PyTuple_New(3);
        if (args == nullptr)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        set_data(args, datax, datay);

        PyObject *pyRetval = PyObject_CallObject(circle_handle, args);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the circle_handle!");
        }

        Py_DECREF(pyRetval);
        Py_DECREF(args);
    }

    template <template<typename...> class C1, template<typename...> class C2, typename T1, typename T2, typename T3>
    void vbar(PyObject* figure, const C1<T1> &datax, const C2<T2> &datatops, const T3 width, const std::string &legend = "")
    {
        PyObject *args = PyTuple_New(4);
        if (args == nullptr)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        set_data(args, datax, datatops);
        if (PyTuple_SetItem(args, 3, convert_to_python(width)) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the width!");
        }

        PyObject *pyRetval = PyObject_CallObject(vbar_handle, args);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the vbar_handle!");
        }

        Py_DECREF(pyRetval);
        Py_DECREF(args);
    }

    template <template<typename...> class C, typename T>
    void image(PyObject* figure, const C<C<C<T>>> &images)
    {
        PyObject *args = PyTuple_New(2);
        if (args == nullptr)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

        PyObject *py_images = PyList_New(images.size());
        for (size_t image_index = 0; image_index < images.size(); ++image_index)
        {
            auto &image = images[image_index];

            PyObject *py_image = PyList_New(image.size());
            for (size_t point_index = 0; point_index < image.size(); ++point_index)
            {
                auto &r = image[point_index];
                PyObject *py_row = PyList_New(r.size());
                if (py_row == nullptr)
                {
                    throw std::runtime_error("Could not create a Python list for the image!");
                }

                for (size_t row_index = 0; row_index < r.size(); ++row_index)
                {
                    if (PyList_SetItem(py_row, row_index, convert_to_python(r[row_index])) != 0)
                    {
                        throw std::runtime_error("Could not set the pixel for point <" + std::to_string(point_index) + ", " + std::to_string(row_index) + ">!");
                    }
                }
                
                if (PyList_SetItem(py_image, point_index, py_row) != 0)
                {
                    throw std::runtime_error("Could not set the row for image " + std::to_string(point_index) + "!");
                }
            }
            if (PyList_SetItem(py_images, image_index, py_image) != 0)
            {
                throw std::runtime_error("Could not set the image at frame " + std::to_string(image_index) + "!");
            }
        }

        if (PyTuple_SetItem(args, 1, py_images) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the image!");
        }

        PyObject *pyRetval = PyObject_CallObject(image_handle, args);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the image_handle!");
        }

        Py_DECREF(pyRetval);
        Py_DECREF(args);
    }

    void generate_html(PyObject *figure, const std::string &filename)
    {
        PyObject *args = PyTuple_New(2);
        if (args == nullptr)
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

    void add_to_path(const std::string &path)
    {
        auto cpath = path.c_str();
        PyList_Append(PySys_GetObject("path"), PyUnicode_FromString(cpath));
    }


private:

    template <template<typename...> class C1, template<typename...> class C2, typename T1, typename T2>
    void set_data(PyObject *args, const C1<T1> &datax, const C2<T2> &datay)
    {
        PyObject *xs = PyList_New(datax.size());
        PyObject *ys = PyList_New(datay.size());
        if (xs == nullptr || ys == nullptr)
        {
            throw std::runtime_error("Could not create a Python list for x or y!");
        }
        if (PyTuple_SetItem(args, 1, xs) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the xs!");
        }
        if (PyTuple_SetItem(args, 2, ys) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the ys!");
        }

        size_t point_count = datax.size();
        for (size_t point_index = 0; point_index < point_count; ++point_index)
        {
            if (PyList_SetItem(xs, point_index, convert_to_python(datax[point_index])) != 0)
            {
                throw std::runtime_error("Could not set the x for point " + std::to_string(point_index) + "!");
            }
            if (PyList_SetItem(ys, point_index, convert_to_python(datay[point_index])) != 0)
            {
                throw std::runtime_error("Could not set the y for point " + std::to_string(point_index) + "!");
            }
        }
    }

    PyObject *py_module;
    PyObject *make_new_figure_handle;
    PyObject *line_handle;
    PyObject *circle_handle;
    PyObject *vbar_handle;
    PyObject *image_handle;
    PyObject *generate_html_handle;
};
}
