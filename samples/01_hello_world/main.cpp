#include "malen/malen.h"
#include <string>

// 01 - Inherit from malen::Malen
class PyHelloWorld : public malen::Malen
{
public:
    PyHelloWorld():
        // 02 - Forward along the module name and any paths that need to available
        malen::Malen("hello_world", {"."})
    {}

    // 03 - Implement whatever wrapper(s) is needed for methods in your module
    void greet(const std::string &name)
    {
        // 04 - Call invoke (inherited from malen::Malen)
        //
        // Some additional notes here:
        //  - no need to use __FUNCTION__ here, just used this because the C++ function
        //    here uses the same name, and it would be redundant to type "greet" again
        //
        //  - nullptr here is the set of kwargs (which in this case is None)
        //
        //  - the rest of the arguments are passed as a variadic template; here we only
        //    have name as an argument and so we pass it along.
        //
        invoke(__FUNCTION__, malen::args(name), nullptr);
    }
};

int main()
{
    // 05 - Instantiate our wrapper
    PyHelloWorld phw;

    // 06 - Call our wrapper method
    phw.greet("Lnk2past");
}
