// main.cpp
// contains samplue usage of PythonVisualizer.h

#include "pyvis/PythonVisualizer.h"
#include <iostream>
#include <random>
#include <vector>
#include <utility>

int main()
{
    std::mt19937 eng(11);
    std::uniform_int_distribution<int> dis(0,10);

    py::PythonVisualizer pyvis({".."});
    auto figure = pyvis.make_new_figure("Test Plot");

    std::vector<std::vector<std::vector<int>>> data(100);
    for (auto &image : data)
    {
        image.resize(100);
        for (auto &el : image)
        {
            el = {dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng)};
        }
    }

    auto slider = pyvis.image_slider(figure, data);

    pyvis.generate_html(slider, "test.html");
}

