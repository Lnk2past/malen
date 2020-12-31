#pragma once
#include <Python.h>
#include <string>
#include <unordered_map>

struct ModuleCache
{
    PyObject *module = nullptr;
    std::unordered_map<std::string, PyObject*> attribute_cache = {};
};

static std::unordered_map<std::string, ModuleCache> py_modules = {};

#include <malen/utilities/interpreter.h>
#include <malen/utilities/loaders.h>
#include <malen/utilities/invokers.h>
#include <malen/utilities/convert_cpp_to_py.h>
#include <malen/utilities/convert_py_to_cpp.h>
#include <malen/utilities/structures.h>
