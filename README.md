# malen

[![Tests Status](https://github.com/Lnk2past/malen/workflows/Tests/badge.svg)](https://github.com/Lnk2past/malen/actions)
[![Static Analysis Status](https://github.com/Lnk2past/malen/workflows/Static-Analysis/badge.svg)](https://github.com/Lnk2past/malen/actions)
![PyPI - Python Version](https://img.shields.io/badge/python-3.6%20%7C%203.7%20%7C%203.8%20-blue)
![PyPI - Implementation](https://img.shields.io/pypi/implementation/cpython)

*Malen* is the German word meaning *to paint*. This header-only library is meant for simple embedding of Python within a C++ application.

## Introduction

`malen` is a generic embedding framework for `Python`. At its simplest, you write a wrapper class for a module that you plan on interfacing with. You point it to the module you wish to load, paths that need to be available to Python, and wrappers for any number of methods you wish to call. Then you can instantiate the wrapper and call away!

## Motivation

This has gone through a number of iterations and improvements for a C++ class that I teach. The idea was to provide a number of simple calls to allow users to generate simple visualizations of their data directly from their programs. All of the visualization were done through `Bokeh`, and so a number of prebaked methods were exposed to the students. This would let them make various plot types and add a limited set of widgets and such. I opted for `Bokeh` because generating HTML works a good bit better than static images (e.g. through `Matplotlib`) on the university's servers (to which everyone connects to remotely). Upon expanding the features and capabilities, it became immediately clear that any amount of improvements would directly lead to creating a more generalized embedding frame work.

## How It Works

`malen` works by attempting to use a set of overloads and clever variadic templates for building argument lists. Here is a simple example (this is a version of the [01_hello_world sample](samples/01_hello_world/main.cpp) without the comments, for brevity):

Here is the Python module we want to wrap:

```python
def greet(name):
    print('Welcome to the world, ' + name + '!')
```

And here is the C++ code to embed it:

```c++
#include "malen/malen.h"
#include <string>

class PyHelloWorld : public malen::Malen
{
public:
    PyHelloWorld():
        malen::Malen({
            {
                "hello_world",
                {
                    "greet"
                }
            }},
            {"."})
    {}

    void greet(const std::string &name)
    {
        invoke(__FUNCTION__, malen::args(name));
    }
};

int main()
{
    PyHelloWorld phw;
    phw.greet("Lnk2past");
}
```

Note that out `PyHelloWorld` class only defines a constructor and a method (to wrap around the single method provided by hello_world.py). All of the state is held inside of the inherited `malen::Malen` class. The state refers to a handle to the module and any methods we end up calling. Methods can be preloaded, as they are in this example, or can be loaded upon being called (and then their handles are cached).

`malen::Malen` has a few constructors that we can use to define our wrapper(s). You can give it modules, modules + methods, and/or additional paths to search. In our example above we pass a mapping instructing it to load the `hello_world` module and from it load the `greet` method. We also tell it to include the current directory in the search path (as that is where we assume our module is in this example). You can use `malen::Malen::load_python_attribute` to load methods or `malen::Malen::add_to_path` to directory(s) to the search path whenever you want, and can even pass a module to `invoke` to load a method is if it not currently cached.

The `malen::Malen::invoke` method takes in a method name and two pointers to the functions `args` and `kwargs`. `malen` provides a handful of utility functions for constructing these pointers for you. Both functions `malen::args` and `malen::kwargs` are variadic templates, and handle the construction of the tuple and dictionary to be given to Python. For now you can pass simple types and some STL containers. As mentioned in the previous paragraph, you may also supply a module name in the case you may expect the method to not be laoded yet.

The kwargs default to `nullptr` and therefore can be omitted, but the `args` must always be supplied. If you do not have any arguments to pass along, just pass `nullptr` and `malen` will handle the rest.

Technically you could just instantiate a `malen::Malen` directly and write code/functions to work with the `malen::Malen` instance and whatever methods you want to call; in the end the effect is all the same and you can do what you want or need (but I prefer writing wrapper classes).

## Examples & Usage

See the various examples found [here](samples/). More will be added!
