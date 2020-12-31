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
#include <malen/core/load.h>
#include <malen/core/invoke.h>
#include <malen/core/cast.h>
#include <malen/core/structure.h>
