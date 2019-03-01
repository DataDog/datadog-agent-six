package testsix

import (
	"fmt"
	"os"
	"strings"
	"testing"
)

func TestMain(m *testing.M) {
	err := setUp()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error setting up tests: %v", err)
		os.Exit(-1)
	}

	os.Exit(m.Run())
}

func TestGetVersion(t *testing.T) {
	ver := getVersion()
	prefix := "3."
	if _, ok := os.LookupEnv("TESTING_TWO"); ok {
		prefix = "2.7."
	}

	if !strings.HasPrefix(ver, prefix) {
		t.Errorf("Version doesn't start with `%s`: %s", prefix, ver)
	}
}

func TestRunSimpleString(t *testing.T) {
	output, err := runString("import sys; sys.stderr.write('Hello, World!'); sys.stderr.flush()")

	if err != nil {
		t.Fatalf("`run_simple_string` error: %v", err)
	}

	if output != "Hello, World!" {
		t.Errorf("Unexpected printed value: '%s'", output)
	}
}

func TestGetError(t *testing.T) {
	errorStr := getError()
	expected := "unable to import module 'foo': No module named 'foo'"
	if _, ok := os.LookupEnv("TESTING_TWO"); ok {
		expected = "unable to import module 'foo': No module named foo"
	}
	if errorStr != expected {
		t.Fatalf("Wrong error string returned: %s", errorStr)
	}
}

func TestHasError(t *testing.T) {
	if !hasError() {
		t.Fatal("has_error should return true, got false")
	}
}

func TestGetCheck(t *testing.T) {
	version, err := getFakeCheck()

	if err != nil {
		t.Fatal(err)
	}

	if version != "0.4.2" {
		t.Fatalf("expected version '0.4.2', found '%s'", version)
	}
}

func TestRunCheck(t *testing.T) {
	res, err := runFakeCheck()

	if err != nil {
		t.Fatal(err)
	}

	if res != "" {
		t.Fatal(res)
	}
}
