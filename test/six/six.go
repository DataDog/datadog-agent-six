package testsix

// #cgo CFLAGS: -I../../include
// #cgo LDFLAGS: -L../../six/ -ldatadog-agent-six -ldl
// #include <datadog_agent_six.h>
//
import "C"

import (
	"fmt"
	"os"

	common "../common"
)

var six *C.six_t

func setUp() error {
	if _, ok := os.LookupEnv("TESTING_TWO"); ok {
		six = C.make2()
		if six == nil {
			return fmt.Errorf("`make2` failed")
		}
	} else {
		six = C.make3()
		if six == nil {
			return fmt.Errorf("`make3` failed")
		}
	}

	// Updates sys.path so testing Check can be found
	C.add_python_path(six, C.CString("../python"))

	ok := C.init(six, nil)
	if ok != 1 {
		return fmt.Errorf("`init` failed: %s", C.GoString(C.get_error(six)))
	}

	return nil
}

func tearDown() {
	C.destroy(six)
	six = nil
}

func getVersion() string {
	ret := C.GoString(C.get_py_version(six))
	return ret
}

func runString(code string) (string, error) {
	var ret bool
	var err error
	var output []byte
	output, err = common.Capture(func() {
		ret = C.run_simple_string(six, C.CString(code)) == 1
	})

	if err != nil {
		return "", err
	}

	if !ret {
		return "", fmt.Errorf("`run_simple_string` errored")
	}

	return string(output), err
}

func getError() string {
	// following is supposed to raise an error
	C.get_check(six, C.CString("foo"), C.CString(""), C.CString("[{foo: \"/\"}]"), nil, nil)
	return C.GoString(C.get_error(six))
}

func hasError() bool {
	// following is supposed to raise an error
	C.get_check(six, C.CString("foo"), C.CString(""), C.CString("[{foo: \"/\"}]"), nil, nil)
	ret := C.has_error(six) == 1
	C.clear_error(six)
	return ret
}

func getFakeCheck() (string, error) {
	var check *C.six_pyobject_t
	var version *C.char

	// class
	ret := C.get_class(six, C.CString("fake_check"), &module, &class)
	if ret != 1 || module == nil || class == nil {
		return "", fmt.Errorf(C.GoString(C.get_error(six)))
	}

	// version
	ret = C.get_attr_string(six, module, C.CString("__version__"), &version)
	if ret != 1 || version == nil {
		return "", fmt.Errorf(C.GoString(C.get_error(six)))
	}

	if ret != 1 || check == nil || version == nil {
		return "", fmt.Errorf(C.GoString(C.get_error(six)))
	}

	return C.GoString(version), nil
}

func runFakeCheck() (string, error) {
	var check *C.six_pyobject_t
	var version *C.char

	C.get_class(six, C.CString("datadog_checks.directory"), &module, &class)
	C.get_attr_string(six, module, C.CString("__version__"), &version)
	C.get_check(six, class, C.CString(""), C.CString("[{fake_check: \"/\"}]"), C.CString(""), C.CString("checkID"), &check)

	return C.GoString(C.run_check(six, check)), nil
}
