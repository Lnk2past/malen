#include "malen/malen.h"
#include <iostream>
#include <vector>

class PyMatrix : public malen::Malen
{
public:
    PyMatrix():
        malen::Malen("matrix", {".", "../.."})
    {}

    void populate(std::vector<std::vector<int>> &d)
    {
        malen::convert_to_cpp(invoke(__FUNCTION__, malen::args(d), nullptr), d);
    }

    void print_matrix(const std::vector<std::vector<int>> &d)
    {
        invoke(__FUNCTION__, malen::args(d), nullptr);
    }
};

int main()
{
    PyMatrix pm;

    std::vector<std::vector<int>> d;
    pm.populate(d);
    pm.print_matrix(d);
}
