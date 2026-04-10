// Package chunk implements the flat-array voxel chunk storage.
//
// Ported from source/include/world/chunk.h and chunk.cpp.
// A chunk is a 16x16x16 cube of blocks stored in a flat array
// with index = x*size*size + y*size + z.
package chunk

import "github.com/weigert/territory/server/internal/block"

// BiomeType identifies the biome a chunk belongs to.
type BiomeType uint8

const (
	BiomeVoid   BiomeType = iota // 0
	BiomeDesert                  // 1
	BiomeForest                  // 2
)

// Size is the side length of a chunk in all three dimensions.
const Size = 16

// Chunk holds voxel data for a 16x16x16 cube of blocks.
type Chunk struct {
	Pos    [3]int           // chunk position in world-space
	Size   int              // always 16
	Biome  BiomeType        // biome classification
	Data   [4096]uint8      // 16*16*16 block data
	Remesh bool             // whether the chunk needs re-meshing
}

// New returns a Chunk with Size initialized to 16 and all blocks set to Air.
func New() *Chunk {
	return &Chunk{Size: Size}
}

// GetIndex returns the flat-array index for the given (x,y,z) position.
// Returns -1 if any coordinate is out of bounds (< 0 or >= Size).
func (c *Chunk) GetIndex(x, y, z int) int {
	if x < 0 || x >= c.Size || y < 0 || y >= c.Size || z < 0 || z >= c.Size {
		return -1
	}
	return x*c.Size*c.Size + y*c.Size + z
}

// SetPosition sets the block type at position (x,y,z).
// Out-of-bounds positions are silently ignored.
func (c *Chunk) SetPosition(x, y, z int, blockType block.BlockType) {
	idx := c.GetIndex(x, y, z)
	if idx < 0 {
		return
	}
	c.Data[idx] = uint8(blockType)
}

// GetPosition returns the block type at position (x,y,z).
// Returns Air for out-of-bounds positions.
func (c *Chunk) GetPosition(x, y, z int) block.BlockType {
	idx := c.GetIndex(x, y, z)
	if idx < 0 {
		return block.Air
	}
	return block.BlockType(c.Data[idx])
}
