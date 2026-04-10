class_name BlockTypes
extends RefCounted

## Block type constants matching server-side definitions.
## Colors are RGBA float values used for vertex coloring.

enum Type {
	AIR = 0,
	STONE = 1,
	DIRT = 2,
	GRASS = 3,
	SAND = 4,
	WATER = 5,
	WOOD = 6,
	LEAVES = 7,
	SNOW = 8,
	CLAY = 9,
}

const COLORS: Dictionary = {
	Type.AIR:    Color(0.0, 0.0, 0.0, 0.0),
	Type.STONE:  Color(0.5, 0.5, 0.5, 1.0),
	Type.DIRT:   Color(0.45, 0.30, 0.18, 1.0),
	Type.GRASS:  Color(0.30, 0.55, 0.20, 1.0),
	Type.SAND:   Color(0.85, 0.78, 0.55, 1.0),
	Type.WATER:  Color(0.20, 0.40, 0.75, 0.6),
	Type.WOOD:   Color(0.45, 0.30, 0.15, 1.0),
	Type.LEAVES: Color(0.20, 0.50, 0.15, 0.9),
	Type.SNOW:   Color(0.95, 0.95, 0.97, 1.0),
	Type.CLAY:   Color(0.72, 0.55, 0.45, 1.0),
}

## Returns true if the block type uses transparency.
static func is_transparent(block_type: int) -> bool:
	return block_type == Type.WATER or block_type == Type.LEAVES

## Returns the display name for a block type.
static func get_name(block_type: int) -> String:
	match block_type:
		Type.AIR: return "Air"
		Type.STONE: return "Stone"
		Type.DIRT: return "Dirt"
		Type.GRASS: return "Grass"
		Type.SAND: return "Sand"
		Type.WATER: return "Water"
		Type.WOOD: return "Wood"
		Type.LEAVES: return "Leaves"
		Type.SNOW: return "Snow"
		Type.CLAY: return "Clay"
		_: return "Unknown"
