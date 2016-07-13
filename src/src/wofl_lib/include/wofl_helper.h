#ifndef __WOFL_HELPER__
#define __WOFL_HELPER__

// WOFL_GET_BOARDINFO
typedef struct {
  uint16_t lib_ver;
  int16_t firmware_ver;
  int16_t debug_mode;
  int16_t connection_flags;
  int16_t reserved[2];
} WOFL_version_info_t;

void WOFL_version_info_pack(WOFL_version_info_t &p, WOFLCommand &cmd);
inline uint8_t WOFL_version_info_send(WOFL_version_info_t &p, WOFL_Parser &parser) {
	WOFLCommand cmd;
	WOFL_version_info_pack(p, cmd);
	return parser.send_cmd(cmd);
}

// WOFL_GET_BOARDINFO
typedef struct {
  uint16_t lib_ver;
  int16_t cmd_id;
  int16_t reserved[2];
} WOFL_unsupport_cmd_t;

void WOFL_unsupport_cmd_pack(WOFL_unsupport_cmd_t &p, WOFLCommand &cmd);
inline uint8_t WOFL_unsupport_cmd_send(WOFL_unsupport_cmd_t &p, WOFL_Parser &parser) {
	WOFLCommand cmd;
	WOFL_unsupport_cmd_pack(p, cmd);
	return parser.send_cmd(cmd);
}

// CMD_GET_ANGLES
typedef struct {
	struct {
		int16_t imu_angle;
		int16_t rc_angle;
		int16_t rc_speed;
	} sensor_data[3];  // roll,pitch,yaw
} WOFL_get_angles_t;
void WOFL_get_angles_pack(WOFL_get_angles_t &p, WOFLCommand &cmd);
inline uint8_t WOFL_get_angles_send(WOFL_get_angles_t &p, WOFL_Parser &parser) {
	WOFLCommand cmd;
	WOFL_get_angles_pack(p, cmd);
	return parser.send_cmd(cmd);
}

#endif
