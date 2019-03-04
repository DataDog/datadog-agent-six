package testdatadogagent

import (
	"fmt"
	"os"
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
	code := `
	sys.stderr.write(datadog_agent.get_version())
	sys.stderr.flush()
	`
	out, err := run(code)
	if err != nil {
		t.Fatal(err)
	}
	if out != "1.2.3" {
		t.Errorf("Unexpected printed value: '%s'", out)
	}
}

func TestGetConfig(t *testing.T) {
	code := `
	d = datadog_agent.get_config("foo")
	sys.stderr.write("{}:{}:{}".format(d.get('name'), d.get('body'), d.get('time')))
	sys.stderr.flush()
	`
	out, err := run(code)
	if err != nil {
		t.Fatal(err)
	}
	if out != "foo:Hello:123456" {
		t.Errorf("Unexpected printed value: '%s'", out)
	}
}

func TestHeaders(t *testing.T) {
	code := `
	d = datadog_agent.headers(http_host="myhost", ignore_me="snafu")
	sys.stderr.write(",".join(d.keys()))
	sys.stderr.flush()
	`
	out, err := run(code)
	if err != nil {
		t.Fatal(err)
	}
	if out != "Accept,Content-Type,User-Agent,Host" {
		t.Errorf("Unexpected printed value: '%s'", out)
	}
}

func TestGetHostname(t *testing.T) {
	code := `
	sys.stderr.write(datadog_agent.get_hostname())
	sys.stderr.flush()
	`
	out, err := run(code)
	if err != nil {
		t.Fatal(err)
	}
	if out != "localfoobar" {
		t.Errorf("Unexpected printed value: '%s'", out)
	}
}

func TestGetClustername(t *testing.T) {
	code := `
	sys.stderr.write(datadog_agent.get_clustername())
	sys.stderr.flush()
	`
	out, err := run(code)
	if err != nil {
		t.Fatal(err)
	}
	if out != "the-cluster" {
		t.Errorf("Unexpected printed value: '%s'", out)
	}
}

func TestLog(t *testing.T) {
	code := `
	datadog_agent.log("foo message", 99)
	`
	out, err := run(code)
	if err != nil {
		t.Fatal(err)
	}
	if out != "[99]foo message" {
		t.Errorf("Unexpected printed value: '%s'", out)
	}
}
