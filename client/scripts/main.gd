extends Node3D

## Entry point: wires up networking, world management, and camera.

@onready var network_client: NetworkClient = $NetworkClient
@onready var world_manager: WorldManager = $WorldManager
@onready var camera_controller: CameraController = $CameraController

var _server_url := "ws://localhost:8080/ws"
var _reconnect_timer := 0.0
var _reconnect_delay := 3.0
var _should_reconnect := true
var _initial_center_set := false

func _ready() -> void:
	# Parse command-line for --server=url
	for arg in OS.get_cmdline_args():
		if arg.begins_with("--server="):
			_server_url = arg.substr(9)

	# Connect signals
	network_client.connected.connect(_on_connected)
	network_client.disconnected.connect(_on_disconnected)
	network_client.world_config_received.connect(_on_world_config)
	network_client.chunk_mesh_received.connect(_on_chunk_mesh)
	network_client.all_chunks_sent.connect(_on_all_chunks_sent)

	print("Main: connecting to %s" % _server_url)
	network_client.connect_to_server(_server_url)

func _process(delta: float) -> void:
	if _should_reconnect and not _is_connected():
		_reconnect_timer += delta
		if _reconnect_timer >= _reconnect_delay:
			_reconnect_timer = 0.0
			print("Main: attempting reconnection...")
			network_client.connect_to_server(_server_url)

func _is_connected() -> bool:
	return network_client.is_connected_to_server()

func _on_connected() -> void:
	_reconnect_timer = 0.0
	_initial_center_set = false
	world_manager.clear()
	print("Main: connected, waiting for world data...")

func _on_disconnected() -> void:
	print("Main: disconnected from server, will retry in %.0fs" % _reconnect_delay)

func _on_world_config(config: Dictionary) -> void:
	world_manager.apply_world_config(config)

func _on_chunk_mesh(chunk_pos: Vector3i, mesh_data: Dictionary) -> void:
	world_manager.on_chunk_mesh_received(chunk_pos, mesh_data)

func _on_all_chunks_sent() -> void:
	world_manager.on_all_chunks_sent()
	# Center camera on world after initial load
	if not _initial_center_set:
		_initial_center_set = true
		var center := world_manager.get_world_center()
		camera_controller.set_focus(center)
		print("Main: camera centered at %s" % str(center))
