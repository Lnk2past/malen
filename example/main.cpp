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
    std::uniform_int_distribution<int> dis(0,100);

    py::PythonVisualizer pyvis({".."});
    auto figure = pyvis.make_new_figure("Test Plot");
    {
        std::vector<int> x_data(100);
        std::vector<int> y_data(100);
        std::vector<int> c_data(100);
        for (int i = 0; i < 100; ++i)
        {
            x_data.push_back(dis(eng));
            y_data.push_back(dis(eng));
            c_data.push_back(dis(eng));
        }
        pyvis.plot_color(figure, "scatter", x_data, y_data, c_data);
        pyvis.generate_html(figure, "test.html");
    }
    {
        std::vector<std::vector<std::vector<int>>> data(100);
        for (auto &image : data)
        {
            image.resize(100);
            for (auto &el : image)
            {
                el = {dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng), dis(eng)};
            }
        }
        auto figure = pyvis.make_new_figure("Test Plot");
        pyvis.generate_html(figure, "test.html");
    }
    
}
