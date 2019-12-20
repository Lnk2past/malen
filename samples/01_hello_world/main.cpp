#include "malen/malen.h"
#include <string>

// 01 - Inherit from malen::Malen
class PyHelloWorld : public malen::Malen
{
public:
    PyHelloWorld():
        // 02 - Forward along the module name and any paths that need to available.
        //      The ../.. is for the test
        malen::Malen("hello_world", {".", "../.."})
    {}

    // 03 - Implement whatever wrapper(s) is needed for methods in your module
    void greet(const std::string &name)
    {
        // 04 - Call invoke (inherited from malen::Malen)
        //
        // Some additional notes here:
        //  - No need to use __FUNCTION__ here, just used this because the C++ function
        //    here uses the same name, and it would be redundant to type "greet" again
        //
        //  - Arguments are passed into malen::args, which builds the argument list for you.
        //    Here we also omitted kwargs, as there are none to pass!
        //
        invoke(__FUNCTION__, malen::args(name));
    }
};

int main()
{
    // 05 - Instantiate our wrapper
    PyHelloWorld phw;

    // 06 - Call our wrapper method
    phw.greet("Lnk2past");
}
