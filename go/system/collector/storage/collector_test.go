package storage

import (
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func TestCollect(t *testing.T) {
	c := &Collector{}
	d, err := c.Collect()
	require.NoError(t, err)

	data, ok := d.(*Data)
	require.True(t, ok)

	assert.Greater(t, len(data.Drives)+len(data.Partitions)+len(data.Volumes), 0)
}
