// Package octree implements a sparse octree for voxel data compression.
//
// Ported from source/include/world/octree.h and octree.cpp.
// The octree recursively subdivides a cubic volume into 8 children.
// At depth 4 the volume is 16x16x16 (matching chunk dimensions).
// At depth 0 a node represents a single voxel.
package octree

import (
	"github.com/weigert/territory/server/internal/block"
	"github.com/weigert/territory/server/internal/chunk"
)

// Octree is a sparse octree node for voxel storage.
type Octree struct {
	Type    block.BlockType // block type stored at this node
	SubTree []Octree        // sparse children (0 or up to 8)
	Depth   uint8           // current depth (0 = single voxel, 4 = 16^3)
	Index   uint8           // index within parent (0-7)
}

// New returns an Octree with the given depth and Air type.
func New(depth uint8) *Octree {
	return &Octree{
		Type:  block.Air,
		Depth: depth,
	}
}

// GetPos converts a child index (0-7) to a 3D offset.
//
// C++ equivalent: glm::vec3((index>>2)%2, (index>>1)%2, (index>>0)%2)
func GetPos(index int) (int, int, int) {
	x := (index >> 2) & 1
	y := (index >> 1) & 1
	z := index & 1
	return x, y, z
}

// GetChildIndex converts a 3D position (each component 0 or 1) to a child index (0-7).
//
// C++ equivalent: _pos.x*4 + _pos.y*2 + _pos.z
func GetChildIndex(x, y, z int) int {
	return x*4 + y*2 + z
}

// Contains checks whether position (x,y,z) is within the bounds of this node.
// All coordinates must be >= 0 and < 2^depth.
func (o *Octree) Contains(x, y, z int) bool {
	width := 1 << o.Depth
	return x >= 0 && x < width && y >= 0 && y < width && z >= 0 && z < width
}

// SetPosition inserts a block at position (x,y,z) relative to this node.
//
// Ported from octree.cpp lines 76-133. Uses lazy splitting: when a leaf
// node of non-Air type must be subdivided, all 8 children are created
// with the parent's type before the target child is updated.
func (o *Octree) SetPosition(x, y, z int, blockType block.BlockType) bool {
	// If we are at a leaf node (no children)
	if len(o.SubTree) == 0 {
		// Values are identical - nothing to do
		if o.Type == blockType {
			return true
		}
		// We are at the lowest depth - set the element
		if o.Depth == 0 {
			o.Type = blockType
			return true
		}
	}

	// Calculate which child octant the position falls into.
	// width = 2^(depth-1), the half-width of this node.
	width := 1 << (o.Depth - 1)

	// Determine the octant: which half of each axis the position lies in.
	px := 0
	if x >= width {
		px = 1
	}
	py := 0
	if y >= width {
		py = 1
	}
	pz := 0
	if z >= width {
		pz = 1
	}
	childIndex := GetChildIndex(px, py, pz)

	// Local position within the child octant.
	lx := x - px*width
	ly := y - py*width
	lz := z - pz*width

	// Search existing children for the target index.
	for i := range o.SubTree {
		if o.SubTree[i].Index == uint8(childIndex) {
			return o.SubTree[i].SetPosition(lx, ly, lz, blockType)
		}
	}

	// Child not found - we need to create it.

	// If subtree is empty and our type is not Air, split into 8 children
	// carrying the current type (lazy splitting).
	if len(o.SubTree) == 0 && o.Type != block.Air {
		for i := 0; i < 8; i++ {
			element := Octree{
				Depth: o.Depth - 1,
				Index: uint8(i),
				Type:  o.Type,
			}
			if i == childIndex {
				element.SetPosition(lx, ly, lz, blockType)
			}
			o.SubTree = append(o.SubTree, element)
		}
		return true
	}

	// SubTree is either empty with Air type, or non-empty but missing this child.
	// Create a new child carrying the parent's type.
	element := Octree{
		Depth: o.Depth - 1,
		Type:  o.Type,
		Index: uint8(childIndex),
	}
	element.SetPosition(lx, ly, lz, blockType)
	o.SubTree = append(o.SubTree, element)

	return true
}

// GetPosition returns the block type at position (x,y,z) with the given
// level-of-detail. LOD 0 returns the finest resolution; higher values
// return coarser results (the type of an ancestor node).
//
// Ported from octree.cpp lines 136-147.
func (o *Octree) GetPosition(x, y, z int, lod int) block.BlockType {
	// At target LOD or at a leaf: return this node's type.
	if int(o.Depth) == lod || len(o.SubTree) == 0 {
		return o.Type
	}

	// Find which child contains the requested position.
	width := 1 << (o.Depth - 1)

	for i := range o.SubTree {
		child := &o.SubTree[i]
		cx, cy, cz := GetPos(int(child.Index))
		ox := cx * width
		oy := cy * width
		oz := cz * width

		// Check if position is within this child's bounds.
		rx, ry, rz := x-ox, y-oy, z-oz
		if rx >= 0 && rx < width && ry >= 0 && ry < width && rz >= 0 && rz < width {
			return child.GetPosition(rx, ry, rz, lod)
		}
	}

	// Position not found in any child - return Air.
	return block.Air
}

// TrySimplify performs bottom-up compression of the octree.
//
// Ported from octree.cpp lines 35-69.
// Recursively simplifies children first. If all 8 children exist,
// are all leaves, and share the same block type, collapse them into
// a single node. Air-only children are pruned.
func (o *Octree) TrySimplify() bool {
	// Already simple: leaf or single voxel.
	if len(o.SubTree) == 0 || o.Depth == 0 {
		return true
	}

	success := true

	// Recursively simplify children. Prune Air-only leaves.
	i := 0
	for i < len(o.SubTree) {
		if !o.SubTree[i].TrySimplify() {
			success = false
			i++
		} else if o.SubTree[i].Type == block.Air {
			// Remove Air leaves (matching C++ erase behavior).
			o.SubTree = append(o.SubTree[:i], o.SubTree[i+1:]...)
			// Don't increment i; the next element shifted into position i.
		} else {
			i++
		}
	}

	// Maybe we removed all sub-elements.
	if len(o.SubTree) == 0 {
		return true
	}

	// Only simplify complete, fully-successful subtrees.
	if len(o.SubTree) != 8 || !success {
		return false
	}

	// Check if all children share the same type.
	firstType := o.SubTree[0].Type
	for j := 1; j < len(o.SubTree); j++ {
		if o.SubTree[j].Type != firstType {
			return false
		}
	}

	// All 8 children are identical leaves - collapse.
	o.SubTree = nil
	o.Type = firstType
	return true
}

// FromChunk populates the octree from chunk data.
//
// Ported from octree.cpp lines 24-33. Iterates all positions in the
// chunk and calls SetPosition for each block. After insertion,
// TrySimplify is called to compress the tree.
func (o *Octree) FromChunk(c *chunk.Chunk) {
	for i := 0; i < c.Size; i++ {
		for j := 0; j < c.Size; j++ {
			for k := 0; k < c.Size; k++ {
				bt := c.GetPosition(i, j, k)
				o.SetPosition(i, j, k, bt)
			}
		}
	}
	o.TrySimplify()
}

// ToChunk expands the octree into a chunk's data array.
//
// Ported from chunk.cpp lines 12-30 (Chunk::fromOctree).
// If this node is a leaf or at the finest depth, fill the cubic
// volume starting at offset with the node's type. Otherwise,
// recurse into children.
func (o *Octree) ToChunk(c *chunk.Chunk, offset [3]int) {
	// If leaf or at finest depth, fill the volume.
	if len(o.SubTree) == 0 || o.Depth == 0 {
		side := 1 << o.Depth
		for i := 0; i < side; i++ {
			for j := 0; j < side; j++ {
				for k := 0; k < side; k++ {
					c.SetPosition(
						offset[0]+i,
						offset[1]+j,
						offset[2]+k,
						o.Type,
					)
				}
			}
		}
		return
	}

	// Recurse into children.
	halfWidth := 1 << (o.Depth - 1)
	for i := range o.SubTree {
		child := &o.SubTree[i]
		cx, cy, cz := GetPos(int(child.Index))
		childOffset := [3]int{
			offset[0] + cx*halfWidth,
			offset[1] + cy*halfWidth,
			offset[2] + cz*halfWidth,
		}
		child.ToChunk(c, childOffset)
	}
}
