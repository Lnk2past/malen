// main.cpp
// contains samplue usage of PythonVisualizer.h

#include "pyvis/PythonVisualizer.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

int main()
{
    std::mt19937 eng(11);
    std::uniform_int_distribution<int> dis(0,100);

    py::PythonVisualizer pyvis({".."});
    auto figure = pyvis.make_new_figure("Test Plot", py::kwarg("x_range", std::vector<int>{0,100}), py::kwarg("y_range", std::vector<int>{0,100}));
    {
        auto rgen = [&eng, &dis]()
        {
            return dis(eng);
        };

        std::vector<int> x_data(100);
        std::vector<int> y_data(100);
        std::vector<int> c_data(100);

        std::generate_n(std::begin(x_data), 100, rgen);
        std::generate_n(std::begin(y_data), 100, rgen);
        std::generate_n(std::begin(c_data), 100, rgen);

        std::vector<std::vector<int>> s_data;
        s_data.push_back(y_data);
        for (int i = 0; i < 99; ++i)
        {
            s_data.emplace_back(100);
            std::generate_n(std::begin(s_data.back()), 100, rgen);
        }

        auto sc = pyvis.plot(figure, "circle", x_data, y_data, py::kwarg("color", c_data));
        auto slider = pyvis.slider(sc, "Test Scatter", 0, 99, py::kwarg("y", s_data));
        auto layout = pyvis.layout(figure, slider);
        pyvis.generate_html(layout, "test.html");
    }
    // {
    //     std::vector<std::vector<std::vector<int>>> data(100);
    //     for (auto &image : data)
    //     {
    //         image.resize(100);
    //         for (auto &el : image)
    //         {
    //             el = {dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng)};
    //         }
    //     }
    //     auto sc = pyvis.image(figure, data[0]);
    //     auto slider = pyvis.slider(sc, "Frame", 0, 99, py::kwarg("image", data));
    //     auto layout = pyvis.layout(figure, slider);
    //     pyvis.generate_html(layout, "test.html");
    // }
}
