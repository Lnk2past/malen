# malen

*Malen* is the German word meaning *to paint*.

## Introduction

`malen` is a C++ interface to the Python library `Bokeh`. Grab the header file `malen/malen.h` and the Python module `malen.py` and compile and link with your Python distribution. This means that not only should `Python 3` be available in your environment, but `Bokeh` needs to be installed in your Python environment.

## Motivation

This has gone through a number of iterations and improvements for a C++ class that I teach. The idea was to provide a number of simple calls to allow users to generate simple visualizations of their data. There is much to do still, but wanted to open this up for anyone else that might be interested.

## How It Works

Right now a Python interpreter is instantiated and the `malen.py` module is loaded. The module provides a number of entry level methods for interfacing with `Bokeh` through `malen.py`. Ideally I want to work on slimming down, or outright eliminating `malen.py`, but for now it is here to stay.

I need to be able to automatically convert input from C++ (namely STL) structures to Python objects, and so there is a growing set of functions used for converting and packing argument lists. On that same note, there are a number of methods in `Bokeh` that have a set of keyword arguments; a `kwarg` function is provided for creating individual kwargs, which can be passed through `malen` and built into a complete keyword argument input (i.e. a Python dictionary).

## Examples & Usage

TBD. See [the example](example/main.cpp) for simple usage.

## To-Do

- Provide higher level wrapper methods for common use cases
- Provide control over legends
- Remove need to explicitly instantiate `malen` interface (use singleton?)

### Long-Term Goals

- Remove use of `malen.py`
- Implement dynamic and lazy loading of `Bokeh` methods and features.
- Remove hard-coded `Bokeh` methods and features, rely on dynamic loading.

