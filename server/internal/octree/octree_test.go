package octree

import (
	"testing"

	"github.com/weigert/territory/server/internal/block"
	"github.com/weigert/territory/server/internal/chunk"
)

func TestSetGetPosition(t *testing.T) {
	o := New(4) // depth 4 = 16x16x16

	o.SetPosition(3, 4, 5, block.Stone)
	got := o.GetPosition(3, 4, 5, 0)
	if got != block.Stone {
		t.Errorf("GetPosition(3,4,5,0) = %d, want Stone(%d)", got, block.Stone)
	}

	// Unset position should be Air.
	got = o.GetPosition(0, 0, 0, 0)
	if got != block.Air {
		t.Errorf("GetPosition(0,0,0,0) = %d, want Air(%d)", got, block.Air)
	}

	// Set multiple positions.
	o.SetPosition(0, 0, 0, block.Dirt)
	o.SetPosition(15, 15, 15, block.Grass)

	if got := o.GetPosition(0, 0, 0, 0); got != block.Dirt {
		t.Errorf("GetPosition(0,0,0,0) = %d, want Dirt(%d)", got, block.Dirt)
	}
	if got := o.GetPosition(15, 15, 15, 0); got != block.Grass {
		t.Errorf("GetPosition(15,15,15,0) = %d, want Grass(%d)", got, block.Grass)
	}
	// Previous set should still be intact.
	if got := o.GetPosition(3, 4, 5, 0); got != block.Stone {
		t.Errorf("GetPosition(3,4,5,0) after additional sets = %d, want Stone(%d)", got, block.Stone)
	}
}

func TestGetPosAndIndex(t *testing.T) {
	// GetPos and GetChildIndex should be inverses.
	for i := 0; i < 8; i++ {
		x, y, z := GetPos(i)
		got := GetChildIndex(x, y, z)
		if got != i {
			t.Errorf("GetChildIndex(GetPos(%d)) = %d, want %d", i, got, i)
		}
	}

	// Verify specific known values.
	x, y, z := GetPos(0)
	if x != 0 || y != 0 || z != 0 {
		t.Errorf("GetPos(0) = (%d,%d,%d), want (0,0,0)", x, y, z)
	}

	x, y, z = GetPos(7)
	if x != 1 || y != 1 || z != 1 {
		t.Errorf("GetPos(7) = (%d,%d,%d), want (1,1,1)", x, y, z)
	}

	// Index 5 = binary 101 => x=1, y=0, z=1
	x, y, z = GetPos(5)
	if x != 1 || y != 0 || z != 1 {
		t.Errorf("GetPos(5) = (%d,%d,%d), want (1,0,1)", x, y, z)
	}
}

func TestTrySimplify(t *testing.T) {
	o := New(4)

	// Fill the entire volume with Stone.
	for x := 0; x < 16; x++ {
		for y := 0; y < 16; y++ {
			for z := 0; z < 16; z++ {
				o.SetPosition(x, y, z, block.Stone)
			}
		}
	}

	simplified := o.TrySimplify()
	if !simplified {
		t.Error("TrySimplify returned false for uniform octree")
	}
	if len(o.SubTree) != 0 {
		t.Errorf("after simplify, SubTree has %d children, want 0", len(o.SubTree))
	}
	if o.Type != block.Stone {
		t.Errorf("after simplify, Type = %d, want Stone(%d)", o.Type, block.Stone)
	}
}

func TestTrySimplifyMixed(t *testing.T) {
	o := New(4)

	// Fill with Stone, then place one Dirt block.
	for x := 0; x < 16; x++ {
		for y := 0; y < 16; y++ {
			for z := 0; z < 16; z++ {
				o.SetPosition(x, y, z, block.Stone)
			}
		}
	}
	o.SetPosition(0, 0, 0, block.Dirt)
	o.TrySimplify()

	// Should NOT fully simplify since blocks differ.
	if len(o.SubTree) == 0 {
		t.Error("TrySimplify collapsed a non-uniform octree to a leaf")
	}

	// The special block should still read back correctly.
	if got := o.GetPosition(0, 0, 0, 0); got != block.Dirt {
		t.Errorf("GetPosition(0,0,0,0) = %d, want Dirt(%d)", got, block.Dirt)
	}
	if got := o.GetPosition(1, 0, 0, 0); got != block.Stone {
		t.Errorf("GetPosition(1,0,0,0) = %d, want Stone(%d)", got, block.Stone)
	}
}

func TestFromChunkRoundTrip(t *testing.T) {
	// Create a chunk with a pattern of blocks.
	c := chunk.New()
	for x := 0; x < chunk.Size; x++ {
		for y := 0; y < chunk.Size; y++ {
			for z := 0; z < chunk.Size; z++ {
				// Use a deterministic pattern.
				if (x+y+z)%3 == 0 {
					c.SetPosition(x, y, z, block.Stone)
				} else if (x+y+z)%3 == 1 {
					c.SetPosition(x, y, z, block.Dirt)
				}
				// else remains Air
			}
		}
	}

	// Convert chunk -> octree.
	o := New(4)
	o.FromChunk(c)

	// Convert octree -> new chunk.
	c2 := chunk.New()
	o.ToChunk(c2, [3]int{0, 0, 0})

	// Verify all positions match.
	for x := 0; x < chunk.Size; x++ {
		for y := 0; y < chunk.Size; y++ {
			for z := 0; z < chunk.Size; z++ {
				expected := c.GetPosition(x, y, z)
				got := c2.GetPosition(x, y, z)
				if got != expected {
					t.Fatalf("round-trip mismatch at (%d,%d,%d): got %d, want %d",
						x, y, z, got, expected)
				}
			}
		}
	}
}

func TestFromChunkRoundTripUniform(t *testing.T) {
	// All-stone chunk should survive round-trip.
	c := chunk.New()
	for x := 0; x < chunk.Size; x++ {
		for y := 0; y < chunk.Size; y++ {
			for z := 0; z < chunk.Size; z++ {
				c.SetPosition(x, y, z, block.Stone)
			}
		}
	}

	o := New(4)
	o.FromChunk(c)

	// Should have simplified to a single node.
	if len(o.SubTree) != 0 {
		t.Errorf("uniform chunk produced octree with %d children", len(o.SubTree))
	}

	c2 := chunk.New()
	o.ToChunk(c2, [3]int{0, 0, 0})

	for x := 0; x < chunk.Size; x++ {
		for y := 0; y < chunk.Size; y++ {
			for z := 0; z < chunk.Size; z++ {
				if got := c2.GetPosition(x, y, z); got != block.Stone {
					t.Fatalf("round-trip mismatch at (%d,%d,%d): got %d, want Stone", x, y, z, got)
				}
			}
		}
	}
}

func TestLOD(t *testing.T) {
	o := New(4) // 16x16x16

	// Set a single block in the all-Air volume.
	o.SetPosition(0, 0, 0, block.Stone)

	// At LOD 0 (finest), the exact block should be Stone.
	if got := o.GetPosition(0, 0, 0, 0); got != block.Stone {
		t.Errorf("LOD 0 at (0,0,0) = %d, want Stone(%d)", got, block.Stone)
	}

	// At LOD 0, a nearby position should be Air.
	if got := o.GetPosition(1, 0, 0, 0); got != block.Air {
		t.Errorf("LOD 0 at (1,0,0) = %d, want Air(%d)", got, block.Air)
	}

	// At LOD 4 (coarsest), the root node type is returned.
	// The root is Air since we only set one block.
	got := o.GetPosition(0, 0, 0, 4)
	if got != block.Air {
		t.Errorf("LOD 4 at (0,0,0) = %d, want Air(%d)", got, block.Air)
	}

	// At LOD 1, the depth-1 node's own type is Air (parent was Air).
	got = o.GetPosition(0, 0, 0, 1)
	if got != block.Air {
		t.Errorf("LOD 1 at (0,0,0) = %d, want Air(%d)", got, block.Air)
	}

	// Fill the smallest octant (0..1 in each axis) with Grass.
	o2 := New(4)
	for x := 0; x < 2; x++ {
		for y := 0; y < 2; y++ {
			for z := 0; z < 2; z++ {
				o2.SetPosition(x, y, z, block.Grass)
			}
		}
	}
	o2.TrySimplify()

	// At LOD 0, each position returns Grass.
	if got := o2.GetPosition(0, 0, 0, 0); got != block.Grass {
		t.Errorf("o2 LOD 0 at (0,0,0) = %d, want Grass(%d)", got, block.Grass)
	}

	// An unset position far away remains Air.
	if got := o2.GetPosition(8, 8, 8, 0); got != block.Air {
		t.Errorf("o2 LOD 0 at (8,8,8) = %d, want Air(%d)", got, block.Air)
	}
}

func TestContains(t *testing.T) {
	o := New(4) // covers 0..15

	tests := []struct {
		x, y, z int
		want    bool
	}{
		{0, 0, 0, true},
		{15, 15, 15, true},
		{8, 8, 8, true},
		{-1, 0, 0, false},
		{0, -1, 0, false},
		{0, 0, -1, false},
		{16, 0, 0, false},
		{0, 16, 0, false},
		{0, 0, 16, false},
	}

	for _, tt := range tests {
		got := o.Contains(tt.x, tt.y, tt.z)
		if got != tt.want {
			t.Errorf("Contains(%d,%d,%d) = %v, want %v", tt.x, tt.y, tt.z, got, tt.want)
		}
	}
}
