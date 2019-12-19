#include "malen/malen.h"
#include <string>

class PyHelloWorld : public malen::Malen
{
public:
    PyHelloWorld():
        malen::Malen("hello_world", {"."})
    {}

    void greet(const std::string &name)
    {
        invoke(__FUNCTION__, nullptr, name);
    }
};

int main()
{
    PyHelloWorld phw;
    phw.greet("Lnk2past");
}
