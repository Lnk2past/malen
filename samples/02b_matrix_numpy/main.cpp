#include "malen/malen.h"
#include <vector>

class PyMatrix : public malen::Malen
{
public:
    PyMatrix():
        malen::Malen({
            {
                "matrix",
                {
                    "populate",
                    "populate2d",
                    "populate3d",
                    "print_matrix"
                }
            }},
            {".", "../.."})
    {}

    void populate(std::vector<int> &d)
    {
        malen::cpp_cast(invoke(__FUNCTION__, nullptr, nullptr), d);
    }

    void populate2d(std::vector<std::vector<int>> &d2)
    {
        malen::cpp_cast(invoke(__FUNCTION__, nullptr, nullptr), d2);
    }

    void populate3d(std::vector<std::vector<std::vector<int>>> &d3)
    {
        malen::cpp_cast(invoke(__FUNCTION__, nullptr, nullptr), d3);
    }

    template <typename T>
    void print_matrix(T &d)
    {
        invoke(__FUNCTION__, malen::args(d));
    }
};

int main()
{
    PyMatrix pm;

    std::vector<int> d;
    pm.populate(d);
    pm.print_matrix(d);

    std::vector<std::vector<int>> d2;
    pm.populate2d(d2);
    pm.print_matrix(d2);

    std::vector<std::vector<std::vector<int>>> d3;
    pm.populate3d(d3);
    pm.print_matrix(d3);
}
