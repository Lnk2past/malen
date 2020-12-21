#include "malen/malen.h"
#include <vector>

class PyFoo : public malen::Malen
{
public:
    PyFoo():
        malen::Malen({
            {
                "foo",
                {
                    "bar"
                }
            }},
            {".", "../.."})
    {}

    void bar()
    {
        auto kw1 = malen::kwarg("b", 1);
        auto kw2 = malen::kwarg("c", std::vector<int>{1, 2, 3});
        invoke(__FUNCTION__, nullptr, malen::kwargs(kw1, kw2));
    }
};

int main()
{
    PyFoo pf;
    pf.bar();
}
