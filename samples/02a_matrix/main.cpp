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
                    "print_matrix"
                }
            }},
            {".", "../.."})
    {}

    void populate(std::vector<int> &d)
    {
        malen::convert_to_cpp(invoke(__FUNCTION__, nullptr, nullptr), d);
    }

    void print_matrix(const std::vector<int> &d)
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
}
