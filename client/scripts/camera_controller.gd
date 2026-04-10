class_name CameraController
extends Node3D

## Trimetric orthographic camera with 4-snap rotation, WASD pan, and scroll zoom.

@export var move_speed: float = 30.0
@export var zoom_step: float = 0.1
@export var min_zoom: float = 10.0
@export var max_zoom: float = 200.0
@export var rotation_duration: float = 0.3

## Camera tilt angle in degrees (~26.56 for trimetric, atan(0.5))
const TILT_ANGLE := 26.565

@onready var _camera: Camera3D = $Camera3D

var _focus_point := Vector3.ZERO
var _rotation_index := 0  # 0=North, 1=East, 2=South, 3=West
var _target_yaw := 0.0
var _current_yaw := 0.0
var _start_yaw := 0.0
var _zoom: float = 60.0
var _is_rotating := false
var _rotation_timer := 0.0

func _ready() -> void:
	_target_yaw = 0.0
	_current_yaw = 0.0
	if is_instance_valid(_camera):
		_camera.projection = Camera3D.PROJECTION_ORTHOGRAPHIC
	_update_camera_transform()

func _process(delta: float) -> void:
	_handle_movement(delta)
	_handle_rotation(delta)
	_update_camera_transform()

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("zoom_in"):
		_zoom = max(_zoom - _zoom * zoom_step, min_zoom)
	elif event.is_action_pressed("zoom_out"):
		_zoom = min(_zoom + _zoom * zoom_step, max_zoom)
	elif event.is_action_pressed("rotate_left"):
		_start_rotation(-1)
	elif event.is_action_pressed("rotate_right"):
		_start_rotation(1)

func _handle_movement(delta: float) -> void:
	var input_dir := Vector2.ZERO
	if Input.is_action_pressed("move_forward"):
		input_dir.y -= 1.0
	if Input.is_action_pressed("move_back"):
		input_dir.y += 1.0
	if Input.is_action_pressed("move_left"):
		input_dir.x -= 1.0
	if Input.is_action_pressed("move_right"):
		input_dir.x += 1.0

	if input_dir.length_squared() > 0.0:
		input_dir = input_dir.normalized()
		# Transform movement direction by current camera yaw
		var yaw_rad := deg_to_rad(_current_yaw)
		var forward := Vector3(-sin(yaw_rad), 0.0, -cos(yaw_rad))
		var right := Vector3(cos(yaw_rad), 0.0, -sin(yaw_rad))
		var move := (forward * -input_dir.y + right * input_dir.x) * move_speed * delta
		# Scale movement with zoom so panning feels consistent
		move *= _zoom / 60.0
		_focus_point += move

func _handle_rotation(delta: float) -> void:
	if _is_rotating:
		_rotation_timer += delta
		var t := clampf(_rotation_timer / rotation_duration, 0.0, 1.0)
		t = t * t * (3.0 - 2.0 * t)
		_current_yaw = lerpf(_start_yaw, _target_yaw, t)
		if t >= 1.0:
			_is_rotating = false
			_current_yaw = _target_yaw

func _start_rotation(direction: int) -> void:
	if _is_rotating:
		_current_yaw = _target_yaw
	_start_yaw = _current_yaw
	_rotation_index = (_rotation_index + direction) % 4
	if _rotation_index < 0:
		_rotation_index += 4
	_target_yaw = _rotation_index * 90.0
	_is_rotating = true
	_rotation_timer = 0.0

func _update_camera_transform() -> void:
	if not is_instance_valid(_camera):
		return

	_camera.size = _zoom

	var yaw_rad := deg_to_rad(_current_yaw)
	var tilt_rad := deg_to_rad(TILT_ANGLE)

	# Position camera looking at focus point from the tilt angle
	var distance := 200.0  # Far enough to see everything in ortho
	var offset := Vector3(
		sin(yaw_rad) * cos(tilt_rad) * distance,
		sin(tilt_rad) * distance,
		cos(yaw_rad) * cos(tilt_rad) * distance
	)

	global_position = _focus_point
	_camera.global_position = _focus_point + offset
	_camera.look_at(_focus_point, Vector3.UP)

## Set camera focus to a world position.
func set_focus(pos: Vector3) -> void:
	_focus_point = pos
