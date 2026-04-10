class_name ChunkRenderer
extends RefCounted

## Parses binary chunk mesh data and builds an ArrayMesh.

static var _terrain_material: StandardMaterial3D
static var _water_material: StandardMaterial3D

static func _ensure_materials() -> void:
	if _terrain_material == null:
		_terrain_material = load("res://materials/voxel_terrain.tres") as StandardMaterial3D
	if _water_material == null:
		_water_material = load("res://materials/water.tres") as StandardMaterial3D

## Build an ArrayMesh from the binary mesh data dictionary.
## mesh_data keys: vertex_count, index_count, payload, data_offset
static func build_mesh(mesh_data: Dictionary) -> ArrayMesh:
	_ensure_materials()

	var vertex_count: int = mesh_data["vertex_count"]
	var index_count: int = mesh_data["index_count"]
	var payload: PackedByteArray = mesh_data["payload"]
	var offset: int = mesh_data["data_offset"]

	if vertex_count == 0 or index_count == 0:
		return null

	# Parse positions: float32[3] * vertex_count
	var vertices := PackedVector3Array()
	vertices.resize(vertex_count)
	for i in vertex_count:
		var x := payload.decode_float(offset); offset += 4
		var y := payload.decode_float(offset); offset += 4
		var z := payload.decode_float(offset); offset += 4
		vertices[i] = Vector3(x, y, z)

	# Parse normals: float32[3] * vertex_count
	var normals := PackedVector3Array()
	normals.resize(vertex_count)
	for i in vertex_count:
		var nx := payload.decode_float(offset); offset += 4
		var ny := payload.decode_float(offset); offset += 4
		var nz := payload.decode_float(offset); offset += 4
		normals[i] = Vector3(nx, ny, nz)

	# Parse colors: float32[4] * vertex_count (RGBA)
	var colors := PackedColorArray()
	colors.resize(vertex_count)
	var has_transparent := false
	for i in vertex_count:
		var r := payload.decode_float(offset); offset += 4
		var g := payload.decode_float(offset); offset += 4
		var b := payload.decode_float(offset); offset += 4
		var a := payload.decode_float(offset); offset += 4
		colors[i] = Color(r, g, b, a)
		if a < 0.99:
			has_transparent = true

	# Parse indices: uint32 * index_count
	var indices := PackedInt32Array()
	indices.resize(index_count)
	for i in index_count:
		indices[i] = payload.decode_u32(offset); offset += 4

	# Build surface arrays
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = vertices
	arrays[Mesh.ARRAY_NORMAL] = normals
	arrays[Mesh.ARRAY_COLOR] = colors
	arrays[Mesh.ARRAY_INDEX] = indices

	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)

	# Assign appropriate material based on transparency
	if has_transparent:
		mesh.surface_set_material(0, _water_material)
	else:
		mesh.surface_set_material(0, _terrain_material)

	return mesh
