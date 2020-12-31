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

#include <malen/core/interpreter.h>
#include <malen/core/loaders.h>
#include <malen/core/invokers.h>
#include <malen/core/convert_cpp_to_py.h>
#include <malen/core/convert_py_to_cpp.h>
#include <malen/core/structures.h>
