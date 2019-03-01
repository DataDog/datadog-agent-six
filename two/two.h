// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2019 Datadog, Inc.
#ifndef DATADOG_AGENT_SIX_TWO_H
#define DATADOG_AGENT_SIX_TWO_H
#include <map>
#include <string>
#include <vector>

#include <Python.h>
#include <six.h>

class Two : public Six {
public:
    Two()
        : Six()
        , _baseClass(NULL)
        , _pythonPaths(){};
    ~Two();

    bool init(const char *pythonHome);
    bool addPythonPath(const char *path);
    six_gilstate_t GILEnsure();
    void GILRelease(six_gilstate_t);
    SixPyObject *getCheckClass(const char *module);

    bool getClass(const char *module, SixPyObject *&pyModule, SixPyObject *&pyClass);
    bool getAttrString(SixPyObject *obj, const char *attributeName, char *&value) const;
    bool getCheck(SixPyObject *py_class, const char *init_config, const char *instance, const char *agent_config,
                  const char *check_id, SixPyObject *&check);

    const char *runCheck(SixPyObject *check);
    void decref(SixPyObject *);

    // const API
    bool isInitialized() const;
    const char *getPyVersion() const;
    bool runSimpleString(const char *code) const;
    SixPyObject *getNone() const {
        return reinterpret_cast<SixPyObject *>(Py_None);
    }

    // aggregator API
    void setSubmitMetricCb(cb_submit_metric_t);
    void setSubmitServiceCheckCb(cb_submit_service_check_t);
    void setSubmitEventCb(cb_submit_event_t);

    // datadog_agent
    void setGetVersionCb(cb_get_version_t);
    void setGetConfigCb(cb_get_config_t);

private:
    PyObject *_importFrom(const char *module, const char *name);
    PyObject *_findSubclassOf(PyObject *base, PyObject *moduleName);
    PyObject *_getClass(const char *module, const char *base);
    std::string _fetchPythonError();

    typedef std::vector<std::string> PyPaths;

    PyObject *_baseClass;
    PyPaths _pythonPaths;
    PyThreadState *_state;
};

#endif
