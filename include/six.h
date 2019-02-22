// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2019 Datadog, Inc.
#ifndef DATADOG_AGENT_SIX_SIX_H
#define DATADOG_AGENT_SIX_SIX_H

#include "six_types.h"
#include <map>
#include <mutex>
#include <string>
#include <vector>

// Opaque type to wrap PyObject
class SixPyObject {};

class Six {
public:
    Six()
        : _error()
        , _errorFlag(false){};
    virtual ~Six(){};

    // Public API
    virtual bool init(const char *pythonHome) = 0;
    virtual bool addModuleFunction(six_module_t module, six_module_func_t t, const char *funcName, void *func) = 0;
    virtual bool addModuleIntConst(six_module_t module, const char *name, long value) = 0;
    virtual bool addPythonPath(const char *path) = 0;
    virtual six_gilstate_t GILEnsure() = 0;
    virtual void GILRelease(six_gilstate_t) = 0;

    virtual bool getClass(const char *module, SixPyObject *&pyModule, SixPyObject *&pyClass) = 0;
    virtual bool getClassVersion(SixPyObject *py_class, char *&version) = 0;
    virtual bool getClassFile(SixPyObject *py_class, char *&file) = 0;
    virtual bool getCheck(SixPyObject *py_class, const char *init_config, const char *instance,
                          const char *agent_config, const char *check_id, SixPyObject *&check)
        = 0;

    // virtual bool getCheck(const char *name, const char *init_config, const char *instances, SixPyObject *&check,
    //                      char *&version)
    //= 0;
    virtual const char *runCheck(SixPyObject *check) = 0;
    void clearError();

    // Public Const API
    virtual bool isInitialized() const = 0;
    virtual const char *getPyVersion() const = 0;
    virtual bool runSimpleString(const char *code) const = 0;
    virtual SixPyObject *getNone() const = 0;
    const char *getError() const;
    bool hasError() const;
    void setError(const std::string &msg) const; // let const methods set errors
    void setError(const char *msg) const;

protected:
    const char *getExtensionModuleName(six_module_t m);
    const char *getUnknownModuleName();

private:
    mutable std::string _error;
    mutable bool _errorFlag;
};

typedef Six *create_t();
typedef void destroy_t(Six *);

typedef std::pair<std::string, long> PyModuleConst;
typedef std::map<six_module_t, std::vector<PyModuleConst> > PyModuleConstants;

#ifndef WIN32
#    define _strdup(x) strdup(x)
#endif

#endif
