// main.cpp
// contains samplue usage of PythonVisualizer.h

#include "malen/malen.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

int main()
{
    std::mt19937 eng(11);
    std::uniform_int_distribution<int> dis(0,100);

    malen::Malen mln({".."});
    auto figure = mln.make_new_figure("Test Plot", malen::kwarg("x_range", std::vector<int>{0,100}), malen::kwarg("y_range", std::vector<int>{0,100}));
    // {
    //     auto rgen = [&eng, &dis]()
    //     {
    //         return dis(eng);
    //     };

    //     std::vector<int> x_data(100);
    //     std::vector<int> y_data(100);
    //     std::vector<int> c_data(100);

    //     std::generate_n(std::begin(x_data), 100, rgen);
    //     std::generate_n(std::begin(y_data), 100, rgen);
    //     std::generate_n(std::begin(c_data), 100, rgen);

    //     std::vector<std::vector<int>> s_data;
    //     s_data.push_back(y_data);
    //     for (int i = 0; i < 99; ++i)
    //     {
    //         s_data.emplace_back(100);
    //         std::generate_n(std::begin(s_data.back()), 100, rgen);
    //     }

    //     auto sc = mln.plot(figure, "circle", x_data, y_data, malen::kwarg("color", c_data));
    //     auto slider = mln.slider(sc, "Test Scatter", 0, 99, malen::kwarg("y", s_data));
    //     auto layout = mln.layout(figure, slider);
    //     mln.generate_html(layout, "test.html");
    // }
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
        auto sc = mln.image(figure, data[0]);
        auto slider = mln.slider(sc, "Frame", 0, data.size()-1, malen::kwarg("image", data));
        auto layout = mln.layout(figure, slider);
        mln.generate_html(layout, "test.html");
    }
}
