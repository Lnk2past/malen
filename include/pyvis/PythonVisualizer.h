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
        if (py_module == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer module.");
        }

        make_new_figure_handle = PyObject_GetAttrString(py_module, "make_new_figure");
        if (make_new_figure_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.make_new_figure method.");
        }

        plot_handle = PyObject_GetAttrString(py_module, "plot");
        if (plot_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.plot method.");
        }

        plot_color_handle = PyObject_GetAttrString(py_module, "plot_color");
        if (plot_color_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.plot_color method.");
        }

        image_handle = PyObject_GetAttrString(py_module, "image");
        if (image_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.image method.");
        }

        image_slider_handle = PyObject_GetAttrString(py_module, "image_slider");
        if (image_slider_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.image_slider method.");
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
    PyObject* plot(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, const std::string &legend = "")
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

        if (PyTuple_SetItem(args, 1, PyUnicode_FromString(plot_type.c_str())) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the plot_type!");
        }

        set_vector_data(args, 2, datax);
        set_vector_data(args, 3, datay);
        
        PyObject *kwargs = nullptr;
        if (!legend.empty())
        {
            kwargs = PyDict_New();
            PyDict_SetItemString(kwargs, "legend", PyUnicode_FromString(legend.c_str()));
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
    PyObject* plot_color(PyObject* figure, const std::string &plot_type, const C1<T1> &datax, const C2<T2> &datay, const C3<T3> &datacolor, const std::string &legend = "")
    {
        PyObject *args = PyTuple_New(5);
        if (args == nullptr)
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

        set_vector_data(args, 2, datax);
        set_vector_data(args, 3, datay);
        set_vector_data(args, 4, datacolor);

        PyObject *kwargs = nullptr;
        if (!legend.empty())
        {
            kwargs = PyDict_New();
            PyDict_SetItemString(kwargs, "legend", PyUnicode_FromString(legend.c_str()));
        }

        PyObject *pyRetval = PyObject_Call(plot_color_handle, args, kwargs);
        if (pyRetval == nullptr)
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
        if (args == nullptr)
        {
            throw std::runtime_error("Could not create a Python tuple!");
        }

        if (PyTuple_SetItem(args, 0, figure) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the figure!");
        }

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

        if (PyTuple_SetItem(args, 1, py_image) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the image!");
        }

        PyObject *pyRetval = PyObject_CallObject(image_handle, args);
        if (pyRetval == nullptr)
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

        PyObject *pyRetval = PyObject_CallObject(image_slider_handle, args);
        if (pyRetval == nullptr)
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
    template <template<typename...> class C, typename T>
    void set_vector_data(PyObject *args, const std::size_t arg_idx, const C<T> &data)
    {
        PyObject *pd = PyList_New(data.size());
        if (pd == nullptr)
        {
            throw std::runtime_error("Could not create a Python list for the input data");
        }
        if (PyTuple_SetItem(args, arg_idx, pd) != 0)
        {
            throw std::runtime_error("Could not populate the argument tuple with the pd!");
        }

        size_t point_count = data.size();
        for (size_t point_index = 0; point_index < point_count; ++point_index)
        {
            if (PyList_SetItem(pd, point_index, convert_to_python(data[point_index])) != 0)
            {
                throw std::runtime_error("Could not set the data for point " + std::to_string(point_index) + "!");
            }
        }
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
