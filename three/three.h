// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2019 Datadog, Inc.
#ifndef DATADOG_AGENT_SIX_THREE_H
#define DATADOG_AGENT_SIX_THREE_H
#include <map>
#include <string>
#include <vector>

#include <Python.h>
#include <six.h>

class Three : public Six {
public:
    Three()
        : _modules()
        , _pythonHome(NULL) {};
    ~Three();

    void init(const char *pythonHome);
    int addModuleFunction(six_module_t module, six_module_func_t t, const char *funcName, void *func);
    int addModuleIntConst(six_module_t module, const char *name, long value) {
        /* FIXME */
        return 1;
    }
    six_gilstate_t GILEnsure();
    void GILRelease(six_gilstate_t);
    /* FIXME */
    SixPyObject *getCheckClass(const char *module) { return NULL; }
    SixPyObject *getCheck(const char *module, const char *init_config_str, const char *instances_str);
    const char *runCheck(SixPyObject *check);

    // const API
    bool isInitialized() const;
    const char *getPyVersion() const;
    int runSimpleString(const char *path) const;
    SixPyObject *getNone() const { return reinterpret_cast<SixPyObject *>(Py_None); }

private:
    PyObject *_importFrom(const char *module, const char *name);
    PyObject *_findSubclassOf(PyObject *base, PyObject *module);
    std::string _fetchPythonError();

    typedef std::vector<PyMethodDef> PyMethods;
    typedef std::map<six_module_t, PyMethods> PyModules;

    PyModules _modules;
    wchar_t *_pythonHome;
};

#ifdef __cplusplus
extern "C" {
#endif

extern Six *create() { return new Three(); }

extern void destroy(Six *p) { delete p; }

#ifdef __cplusplus
}
#endif
#endif
