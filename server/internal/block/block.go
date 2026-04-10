// Package block defines the block types used throughout the voxel engine.
package block

// BlockType represents the type of a voxel block.
type BlockType uint8

const (
	Air          BlockType = iota // 0
	Grass                        // 1
	Dirt                         // 2
	Sand                         // 3
	Clay                         // 4
	Gravel                       // 5
	Sandstone                    // 6
	Stone                        // 7
	Water                        // 8
	Leaves                       // 9
	Wood                         // 10
	Pumpkin                      // 11
	Cactus                       // 12
	Planks                       // 13
	Glass                        // 14
	Void                         // 15
	CactusFlower                 // 16
)

// IsVisible returns true if the block type is rendered.
func IsVisible(t BlockType) bool {
	return t != Air && t != Void
}

// IsCubic returns true if the block uses a cube model.
func IsCubic(t BlockType) bool {
	return t != CactusFlower
}
