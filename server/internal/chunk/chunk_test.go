package chunk

import (
	"testing"

	"github.com/weigert/territory/server/internal/block"
)

func TestGetIndex(t *testing.T) {
	c := New()

	// x=1, y=2, z=3 => 1*256 + 2*16 + 3 = 291
	got := c.GetIndex(1, 2, 3)
	if got != 291 {
		t.Errorf("GetIndex(1,2,3) = %d, want 291", got)
	}

	// Origin should be 0.
	got = c.GetIndex(0, 0, 0)
	if got != 0 {
		t.Errorf("GetIndex(0,0,0) = %d, want 0", got)
	}

	// Last valid index: (15,15,15) = 15*256+15*16+15 = 4095.
	got = c.GetIndex(15, 15, 15)
	if got != 4095 {
		t.Errorf("GetIndex(15,15,15) = %d, want 4095", got)
	}
}

func TestSetGetPosition(t *testing.T) {
	c := New()

	c.SetPosition(3, 4, 5, block.Stone)
	got := c.GetPosition(3, 4, 5)
	if got != block.Stone {
		t.Errorf("GetPosition(3,4,5) = %d, want Stone(%d)", got, block.Stone)
	}

	// Unset position should be Air (zero value).
	got = c.GetPosition(0, 0, 0)
	if got != block.Air {
		t.Errorf("GetPosition(0,0,0) = %d, want Air(%d)", got, block.Air)
	}
}

func TestOutOfBounds(t *testing.T) {
	c := New()

	tests := []struct {
		name    string
		x, y, z int
	}{
		{"negative x", -1, 0, 0},
		{"negative y", 0, -1, 0},
		{"negative z", 0, 0, -1},
		{"x too large", 16, 0, 0},
		{"y too large", 0, 16, 0},
		{"z too large", 0, 0, 16},
		{"all negative", -1, -1, -1},
		{"all too large", 16, 16, 16},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			idx := c.GetIndex(tt.x, tt.y, tt.z)
			if idx != -1 {
				t.Errorf("GetIndex(%d,%d,%d) = %d, want -1", tt.x, tt.y, tt.z, idx)
			}

			got := c.GetPosition(tt.x, tt.y, tt.z)
			if got != block.Air {
				t.Errorf("GetPosition(%d,%d,%d) = %d, want Air", tt.x, tt.y, tt.z, got)
			}
		})
	}

	// SetPosition on out-of-bounds should not panic.
	c.SetPosition(-1, 0, 0, block.Stone)
	c.SetPosition(16, 0, 0, block.Stone)
}

func TestFullChunk(t *testing.T) {
	c := New()

	// Fill every position with Stone.
	for x := 0; x < Size; x++ {
		for y := 0; y < Size; y++ {
			for z := 0; z < Size; z++ {
				c.SetPosition(x, y, z, block.Stone)
			}
		}
	}

	// Verify all positions read back as Stone.
	for x := 0; x < Size; x++ {
		for y := 0; y < Size; y++ {
			for z := 0; z < Size; z++ {
				got := c.GetPosition(x, y, z)
				if got != block.Stone {
					t.Fatalf("GetPosition(%d,%d,%d) = %d, want Stone(%d)", x, y, z, got, block.Stone)
				}
			}
		}
	}
}
