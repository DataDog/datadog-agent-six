// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2019 Datadog, Inc.
#ifdef _WIN32
#    include <Windows.h>
#else
#    include <dlfcn.h>
#endif

#include <iostream>

#include <datadog_agent_six.h>
#include <six.h>

#if __linux__
#    define DATADOG_AGENT_TWO "libdatadog-agent-two.so"
#    define DATADOG_AGENT_THREE "libdatadog-agent-three.so"
#elif __APPLE__
#    define DATADOG_AGENT_TWO "libdatadog-agent-two.dylib"
#    define DATADOG_AGENT_THREE "libdatadog-agent-three.dylib"
#elif _WIN32
#    define DATADOG_AGENT_TWO "datadog-agent-two.dll"
#    define DATADOG_AGENT_THREE "datadog-agent-three.dll"
#else
#    error Platform not supported
#endif

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_PTYPE(Type, Obj) reinterpret_cast<Type **>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#ifdef _WIN32
static HMODULE six_backend = NULL;
#else
static void *six_backend = NULL;
#endif

#ifdef _WIN32

six_t *make2() {
    // load library
    six_backend = LoadLibraryA(DATADOG_AGENT_TWO);
    if (!six_backend) {
        std::cerr << "Unable to open 'two' library: " << GetLastError() << std::endl;
        return 0;
    }

    // dlsym class factory
    create_t *create = (create_t *)GetProcAddress(six_backend, "create");
    if (!create) {
        std::cerr << "Unable to open 'two' factory: " << GetLastError() << std::endl;
        return 0;
    }

    return AS_TYPE(six_t, create());
}

void destroy2(six_t *six) {
    if (six_backend) {
        // dlsym object destructor
        destroy_t *destroy = (destroy_t *)GetProcAddress(six_backend, "destroy");
        if (!destroy) {
            std::cerr << "Unable to open 'two' destructor: " << GetLastError() << std::endl;
            return;
        }
        destroy(AS_TYPE(Six, six));
    }
}

six_t *make3() {
    // load the library
    six_backend = LoadLibraryA(DATADOG_AGENT_THREE);
    if (!six_backend) {
        std::cerr << "Unable to open 'three' library: " << GetLastError() << std::endl;
        return 0;
    }

    // dlsym class factory
    create_t *create_three = (create_t *)GetProcAddress(six_backend, "create");
    if (!create_three) {
        std::cerr << "Unable to open 'three' factory: " << GetLastError() << std::endl;
        return 0;
    }

    return AS_TYPE(six_t, create_three());
}

void destroy3(six_t *six) {
    if (six_backend) {
        // dlsym object destructor
        destroy_t *destroy = (destroy_t *)GetProcAddress(six_backend, "destroy");

        if (!destroy) {
            std::cerr << "Unable to open 'three' destructor: " << GetLastError() << std::endl;
            return;
        }
        destroy(AS_TYPE(Six, six));
    }
}

#else
six_t *make2() {
    if (six_backend != NULL) {
        std::cerr << "Six alrady initialized!" << std::endl;
        return NULL;
    }
    // load library
    six_backend = dlopen(DATADOG_AGENT_TWO, RTLD_LAZY | RTLD_GLOBAL);
    if (!six_backend) {
        std::cerr << "Unable to open 'two' library: " << dlerror() << std::endl;
        return NULL;
    }

    // reset dl errors
    dlerror();

    // dlsym class factory
    create_t *create = (create_t *)dlsym(six_backend, "create");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Unable to open 'two' factory: " << dlsym_error << std::endl;
        return NULL;
    }

    return AS_TYPE(six_t, create());
}

six_t *make3() {
    if (six_backend != NULL) {
        std::cerr << "Six alrady initialized!" << std::endl;
        return NULL;
    }

    // load the library
    six_backend = dlopen(DATADOG_AGENT_THREE, RTLD_LAZY | RTLD_GLOBAL);
    if (!six_backend) {
        std::cerr << "Unable to open 'three' library: " << dlerror() << std::endl;
        return NULL;
    }

    // reset dl errors
    dlerror();

    // dlsym class factory
    create_t *create_three = (create_t *)dlsym(six_backend, "create");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Unable to open 'three' factory: " << dlsym_error << std::endl;
        return NULL;
    }

    return AS_TYPE(six_t, create_three());
}

void destroy(six_t *six) {
    if (six_backend) {
        // dlsym object destructor
        destroy_t *destroy = (destroy_t *)dlsym(six_backend, "destroy");
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Unable to dlopen backend destructor: " << dlsym_error << std::endl;
            return;
        }
        destroy(AS_TYPE(Six, six));
    }
}
#endif

int init(six_t *six, char *pythonHome) {
    return AS_TYPE(Six, six)->init(pythonHome) ? 1 : 0;
}

int is_initialized(six_t *six) {
    return AS_CTYPE(Six, six)->isInitialized();
}

const char *get_py_version(const six_t *six) {
    return AS_CTYPE(Six, six)->getPyVersion();
}

int run_simple_string(const six_t *six, const char *code) {
    return AS_CTYPE(Six, six)->runSimpleString(code) ? 1 : 0;
}

six_pyobject_t *get_none(const six_t *six) {
    return AS_TYPE(six_pyobject_t, AS_CTYPE(Six, six)->getNone());
}

int add_python_path(six_t *six, const char *path) {
    return AS_TYPE(Six, six)->addPythonPath(path) ? 1 : 0;
}

six_gilstate_t ensure_gil(six_t *six) {
    return AS_TYPE(Six, six)->GILEnsure();
}

void release_gil(six_t *six, six_gilstate_t state) {
    AS_TYPE(Six, six)->GILRelease(state);
}

int get_class(six_t *six, const char *name, six_pyobject_t **py_module, six_pyobject_t **py_class) {
    return AS_TYPE(Six, six)->getClass(name, *AS_PTYPE(SixPyObject, py_module), *AS_PTYPE(SixPyObject, py_class)) ? 1
                                                                                                                  : 0;
}

int get_attr_string(six_t *six, six_pyobject_t *py_class, const char *attr_name, char **value) {
    return AS_TYPE(Six, six)->getAttrString(AS_TYPE(SixPyObject, py_class), attr_name, *value);
}

int get_check(six_t *six, six_pyobject_t *py_class, const char *init_config, const char *instance,
              const char *agent_config, const char *check_id, six_pyobject_t **check) {
    return AS_TYPE(Six, six)->getCheck(AS_TYPE(SixPyObject, py_class), init_config, instance, agent_config, check_id,
                                       *AS_PTYPE(SixPyObject, check))
        ? 1
        : 0;
}

const char *run_check(six_t *six, six_pyobject_t *check) {
    return AS_TYPE(Six, six)->runCheck(AS_TYPE(SixPyObject, check));
}

/*
 * error API
 */

int has_error(const six_t *six) {
    return AS_CTYPE(Six, six)->hasError() ? 1 : 0;
}

const char *get_error(const six_t *six) {
    return AS_CTYPE(Six, six)->getError();
}

void clear_error(six_t *six) {
    AS_TYPE(Six, six)->clearError();
}

/*
 * memory management
 */

void six_free(six_t *six, void *ptr) {
    AS_TYPE(Six, six)->free(ptr);
}

void six_decref(six_t *six, six_pyobject_t *obj) {
    AS_TYPE(Six, six)->decref(AS_TYPE(SixPyObject, obj));
}

void six_incref(six_t *six, six_pyobject_t *obj) {
    AS_TYPE(Six, six)->incref(AS_TYPE(SixPyObject, obj));
}

/*
 * aggregator API
 */

void set_submit_metric_cb(six_t *six, cb_submit_metric_t cb) {
    AS_TYPE(Six, six)->setSubmitMetricCb(cb);
}

void set_submit_service_check_cb(six_t *six, cb_submit_service_check_t cb) {
    AS_TYPE(Six, six)->setSubmitServiceCheckCb(cb);
}

void set_submit_event_cb(six_t *six, cb_submit_event_t cb) {
    AS_TYPE(Six, six)->setSubmitEventCb(cb);
}

/*
 * datadog_agent API
 */

void set_get_version_cb(six_t *six, cb_get_version_t cb) {
    AS_TYPE(Six, six)->setGetVersionCb(cb);
}

void set_get_config_cb(six_t *six, cb_get_config_t cb) {
    AS_TYPE(Six, six)->setGetConfigCb(cb);
}

void set_headers_cb(six_t *six, cb_headers_t cb) {
    AS_TYPE(Six, six)->setHeadersCb(cb);
}

void set_get_hostname_cb(six_t *six, cb_get_hostname_t cb) {
    AS_TYPE(Six, six)->setGetHostnameCb(cb);
}

void set_get_clustername_cb(six_t *six, cb_get_clustername_t cb) {
    AS_TYPE(Six, six)->setGetClusternameCb(cb);
}

void set_log_cb(six_t *six, cb_log_t cb) {
    AS_TYPE(Six, six)->setLogCb(cb);
}
