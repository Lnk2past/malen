// PythonVisualizer.h
//
// Note: This must be included first! The Python.h header must come first within
// a translation unit!
//

#include <Python.h>

#include <cstdlib>
#include <stdexcept>
#include <vector>

namespace py
{
class PythonVisualizer
{
public:
    PythonVisualizer(const PythonVisualizer&) = delete;
    void operator=(const PythonVisualizer&) = delete;

    PythonVisualizer(const std::vector<std::string> &additional_paths = std::vector<std::string>()) :
        py_module(nullptr),
        make_new_figure_handle(nullptr),
        line_handle(nullptr),
        scatter_handle(nullptr),
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

        scatter_handle = PyObject_GetAttrString(py_module, "scatter");
        if (scatter_handle == nullptr)
        {
            throw std::runtime_error("There were problems loading the Visualizer.scatter method.");
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

        Py_DECREF(pyRetval);
        return pyRetval;
    }

    void line(PyObject* figure, const std::vector<std::vector<int>> &data)
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

        set_data(args, data);

        PyObject *pyRetval = PyObject_CallObject(line_handle, args);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the scatter_handle!");
        }

        Py_DECREF(pyRetval);
        Py_DECREF(args);
    }

    void scatter(PyObject* figure, const std::vector<std::vector<int>> &data)
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

        set_data(args, data);

        PyObject *pyRetval = PyObject_CallObject(scatter_handle, args);
        if (pyRetval == nullptr)
        {
            throw std::runtime_error("Failed to invoke the scatter_handle!");
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

    void set_data(PyObject *args, const std::vector<std::vector<int>> &data)
    {
        PyObject *xs = PyList_New(data.size());
        PyObject *ys = PyList_New(data.size());
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

        size_t point_count = data.size();
        for (size_t point_index = 0; point_index < point_count; ++point_index)
        {
            if (PyList_SetItem(xs, point_index, PyLong_FromLong(data[point_index][0])) != 0)
            {
                throw std::runtime_error("Could not set the x for point " + std::to_string(point_index) + "!");
            }
            if (PyList_SetItem(ys, point_index, PyLong_FromLong(data[point_index][1])) != 0)
            {
                throw std::runtime_error("Could not set the y for point " + std::to_string(point_index) + "!");
            }
        }
    }


    PyObject *py_module;
    PyObject *make_new_figure_handle;
    PyObject *line_handle;
    PyObject *scatter_handle;
    PyObject *generate_html_handle;
};
}
