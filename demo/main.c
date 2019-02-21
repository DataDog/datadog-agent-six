// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2019 Datadog, Inc.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <datadog_agent_six.h>

static six_t *six;

static six_pyobject_t *print_foo() {
    printf("I'm extending Python!\n\n");
    return get_none(six);
}

char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    long read = fread(string, fsize, 1, f);
    if (read < 1) {
        fprintf(stderr, "Error reading file!\n");
    }
    fclose(f);

    string[fsize] = 0;

    return string;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please run: demo <2|3> [path_to_python_home]. For example:\n\n");
        printf("demo 3 $VIRTUAL_ENV\n");
        return 1;
    }

    // Python home
    char *python_home = NULL;
    if (argc == 3) {
        python_home = argv[2];
    }

    // Embed Python2
    if (strcmp(argv[1], "2") == 0) {
        six = make2();
        if (!six) {
            printf("Unable to init Python2\n");
            return 1;
        }
    }
    // Embed Python3
    else if (strcmp(argv[1], "3") == 0) {
        six = make3();
        if (!six) {
            printf("Unable to init Python3\n");
            return 1;
        }
    }
    // Error
    else {
        printf("Unrecognized version: %s, %d\n", argv[1], strncmp(argv[1], "2", strlen(argv[1])));
        return 2;
    }

    // add a new `print_foo` to the custom builtin module `datadog_agent`
    add_module_func(six, DATADOG_AGENT_SIX__UTIL, DATADOG_AGENT_SIX_NOARGS, "print_foo", print_foo);
    add_module_int_const(six, DATADOG_AGENT_SIX__UTIL, "constant_number", 21);

    if (!init(six, python_home)) {
        printf("Error initializing six: %s\n", get_error(six));
        return 1;
    }

    printf("Embedding Python version %s\n\n", get_py_version(six));

    // run a script from file
    char *code = read_file("./demo/main.py");
    run_simple_string(six, code);

    // load the Directory check if available
    six_pyobject_t *py_class;

    printf("importing check\n");
    int ok = get_class(six, "datadog_checks.directory", &py_class);
    if (!ok) {
        if (has_error(six)) {
            printf("error getting class: %s\n", get_error(six));
        }
        printf("Failed to get_class\n");
        return 1;
    }

    char *version = NULL;
    ok = get_class_version(six, py_class, &version);
    if (!ok) {
        if (has_error(six)) {
            printf("error getting class version: %s\n", get_error(six));
        }
        printf("Failed to get_version\n");
        return 1;
    }
    printf("Directory version: %s.\n", version);
    free(version);

    char *file = NULL;
    ok = get_class_file(six, py_class, &file);
    if (!ok) {
        if (has_error(six)) {
            printf("error getting class file: %s\n", get_error(six));
        }
        printf("Failed to get_file\n");
        return 1;
    }
    printf("Directory file: %s.\n", file);
    free(file);

    // load the Directory check if available
    version = NULL;
    six_pyobject_t *check;

    ok = get_check(six, "datadog_checks.directory", "", "[{directory: \"/\"}]", &check, &version);
    if (!ok) {
        if (has_error(six)) {
            printf("error loading check: %s\n", get_error(six));
        }
        return 1;
    }

    if (version != NULL) {
        printf("Successfully imported Directory integration v%s.\n", version);
    } else {
        printf("Successfully imported Directory integration.\n");
    }

    const char *result = run_check(six, check);

    if (result == NULL) {
        printf("Unable to run the check!\n");
        return 1;
    }

    if (strlen(result) == 0) {
        printf("Successfully run the check\n");
    } else {
        printf("Error running the check, output:\n %s\n", result);
    }

    return 0;
}
