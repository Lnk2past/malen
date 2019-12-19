#include "malen/malen.h"
#include <iostream>
#include <vector>

class PyFoo : public malen::Malen
{
public:
    PyFoo():
        malen::Malen("foo", {"."})
    {}

    void bar()
    {
        auto kw1 = malen::kwarg("b", 1);
        auto kw2 = malen::kwarg("c", std::vector<int>{1, 2, 3});
        auto kwargs = malen::build_kwargs(nullptr, kw1, kw2);
        invoke(__FUNCTION__, kwargs);
    }
};

int main()
{
    PyFoo pf;
    pf.bar();
}
