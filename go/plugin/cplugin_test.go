package plugin

import "testing"

func TestLoad(t *testing.T) {
	var pl *CPlugin
	var err error
	if pl, err = Load("plugin.so"); err != nil {
		t.Error("Load fail" + err.Error())
	} else {
		if err = pl.Unload(); err != nil {
			t.Error("Unload fail")
		}
	}
}