class_name NetworkClient
extends Node

## WebSocket client that connects to the Go terrain server.
## Parses binary wire protocol and emits typed signals.

signal connected
signal disconnected
signal world_config_received(config: Dictionary)
signal chunk_mesh_received(chunk_pos: Vector3i, mesh_data: Dictionary)
signal all_chunks_sent

const MSG_WORLD_CONFIG := 0x01
const MSG_CHUNK_MESH := 0x02
const MSG_ALL_CHUNKS_SENT := 0x03

var _socket := WebSocketPeer.new()
var _url: String = "ws://localhost:8080/ws"
var _was_connected := false

func is_connected_to_server() -> bool:
	return _was_connected

func connect_to_server(url: String = "") -> void:
	if url != "":
		_url = url
	var err := _socket.connect_to_url(_url)
	if err != OK:
		push_error("NetworkClient: failed to initiate connection to %s: %s" % [_url, error_string(err)])

func disconnect_from_server() -> void:
	_socket.close()

func _process(_delta: float) -> void:
	_socket.poll()

	var state := _socket.get_ready_state()

	if state == WebSocketPeer.STATE_OPEN:
		if not _was_connected:
			_was_connected = true
			print("NetworkClient: connected to %s" % _url)
			connected.emit()
		while _socket.get_available_packet_count() > 0:
			var packet := _socket.get_packet()
			_handle_packet(packet)

	elif state == WebSocketPeer.STATE_CLOSED:
		if _was_connected:
			_was_connected = false
			var code := _socket.get_close_code()
			var reason := _socket.get_close_reason()
			print("NetworkClient: disconnected (code=%d, reason=%s)" % [code, reason])
			disconnected.emit()

func _handle_packet(data: PackedByteArray) -> void:
	if data.size() < 1:
		push_warning("NetworkClient: received empty packet")
		return

	var msg_type := data[0]
	var payload := data.slice(1)

	match msg_type:
		MSG_WORLD_CONFIG:
			_parse_world_config(payload)
		MSG_CHUNK_MESH:
			_parse_chunk_mesh(payload)
		MSG_ALL_CHUNKS_SENT:
			print("NetworkClient: all chunks sent")
			all_chunks_sent.emit()
		_:
			push_warning("NetworkClient: unknown message type 0x%02x" % msg_type)

func _parse_world_config(payload: PackedByteArray) -> void:
	var json_str := payload.get_string_from_utf8()
	var json := JSON.new()
	var err := json.parse(json_str)
	if err != OK:
		push_error("NetworkClient: failed to parse world config JSON: %s" % json.get_error_message())
		return
	var config: Dictionary = json.data
	print("NetworkClient: world config received: %s" % str(config))
	world_config_received.emit(config)

func _parse_chunk_mesh(payload: PackedByteArray) -> void:
	# Header: int32[3] chunkPos | uint32 vertexCount | uint32 indexCount
	var header_size := 3 * 4 + 2 * 4  # 20 bytes
	if payload.size() < header_size:
		push_error("NetworkClient: chunk mesh payload too small (%d bytes)" % payload.size())
		return

	var offset := 0
	var cx := payload.decode_s32(offset); offset += 4
	var cy := payload.decode_s32(offset); offset += 4
	var cz := payload.decode_s32(offset); offset += 4
	var vertex_count := payload.decode_u32(offset); offset += 4
	var index_count := payload.decode_u32(offset); offset += 4

	var chunk_pos := Vector3i(cx, cy, cz)

	# Calculate expected payload size after header
	var positions_size := vertex_count * 3 * 4  # float32[3] per vertex
	var normals_size := vertex_count * 3 * 4
	var colors_size := vertex_count * 4 * 4     # float32[4] per vertex (RGBA)
	var indices_size := index_count * 4          # uint32 per index
	var expected_size := header_size + positions_size + normals_size + colors_size + indices_size

	if payload.size() < expected_size:
		push_error("NetworkClient: chunk mesh data truncated at %s (got %d, expected %d)" % [
			str(chunk_pos), payload.size(), expected_size])
		return

	var mesh_data := {
		"chunk_pos": chunk_pos,
		"vertex_count": vertex_count,
		"index_count": index_count,
		"payload": payload,
		"data_offset": header_size,
	}

	chunk_mesh_received.emit(chunk_pos, mesh_data)
