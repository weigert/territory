class_name WorldManager
extends Node3D

## Manages chunk lifecycle: receives mesh data from the network,
## builds ArrayMeshes, and creates/removes MeshInstance3D nodes.

var _chunks: Dictionary = {}  # Vector3i -> MeshInstance3D
var _world_config: Dictionary = {}
var _chunk_size: int = 32
var _chunks_received := 0
var _total_vertices := 0

func apply_world_config(config: Dictionary) -> void:
	_world_config = config
	if config.has("chunk_size"):
		_chunk_size = int(config["chunk_size"])
	print("WorldManager: config applied (chunk_size=%d)" % _chunk_size)

func on_chunk_mesh_received(chunk_pos: Vector3i, mesh_data: Dictionary) -> void:
	# Remove existing chunk mesh if present
	if _chunks.has(chunk_pos):
		var old_node: MeshInstance3D = _chunks[chunk_pos]
		old_node.queue_free()
		_chunks.erase(chunk_pos)

	var mesh := ChunkRenderer.build_mesh(mesh_data)
	if mesh == null:
		return

	var mesh_instance := MeshInstance3D.new()
	mesh_instance.mesh = mesh
	mesh_instance.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_ON
	mesh_instance.name = "Chunk_%d_%d_%d" % [chunk_pos.x, chunk_pos.y, chunk_pos.z]

	# Position the mesh instance at the chunk's world position
	mesh_instance.position = Vector3(
		chunk_pos.x * _chunk_size,
		chunk_pos.y * _chunk_size,
		chunk_pos.z * _chunk_size
	)

	add_child(mesh_instance)
	_chunks[chunk_pos] = mesh_instance

	_chunks_received += 1
	_total_vertices += int(mesh_data["vertex_count"])

func on_all_chunks_sent() -> void:
	print("WorldManager: all chunks loaded (%d chunks, %d vertices)" % [
		_chunks_received, _total_vertices])

## Remove all chunks.
func clear() -> void:
	for chunk_pos in _chunks:
		var node: MeshInstance3D = _chunks[chunk_pos]
		if is_instance_valid(node):
			node.queue_free()
	_chunks.clear()
	_chunks_received = 0
	_total_vertices = 0

## Returns the center of the loaded world in world coordinates.
func get_world_center() -> Vector3:
	if _chunks.is_empty():
		return Vector3.ZERO
	var min_pos := Vector3(INF, INF, INF)
	var max_pos := Vector3(-INF, -INF, -INF)
	for chunk_pos: Vector3i in _chunks:
		var world_pos := Vector3(chunk_pos) * _chunk_size
		min_pos = min_pos.min(world_pos)
		max_pos = max_pos.max(world_pos + Vector3.ONE * _chunk_size)
	return (min_pos + max_pos) * 0.5

## Returns the number of loaded chunks.
func get_chunk_count() -> int:
	return _chunks.size()
