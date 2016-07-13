
#include <string.h>
#include "wofl.h"

/* Packs command structure to SerialCommand object */
void WOFL_get_angles_pack(WOFL_get_angles_t &p, WOFLCommand &cmd) {
       cmd.init(WOFL_TYPE_BGC);
	cmd.cmd(WOFL_GET_ANDLES);
#ifdef WOFL_CMD_STRUCT_ALIGNED
	memcpy(cmd.data, &p, sizeof(p));
	cmd.len = sizeof(p);
#else
	cmd.writeWord(p.sensor_data[0].imu_angle);
	cmd.writeWord(p.sensor_data[0].rc_angle);
	cmd.writeWord(p.sensor_data[0].rc_speed);
	cmd.writeWord(p.sensor_data[1].imu_angle);
	cmd.writeWord(p.sensor_data[1].rc_angle);
	cmd.writeWord(p.sensor_data[1].rc_speed);
	cmd.writeWord(p.sensor_data[2].imu_angle);
	cmd.writeWord(p.sensor_data[2].rc_angle);
	cmd.writeWord(p.sensor_data[2].rc_speed);
#endif
}

/* Packs command structure to SerialCommand object */
void WOFL_version_info_pack(WOFL_version_info_t &p, WOFLCommand &cmd) {
    cmd.init(WOFL_TYPE_BGC);
	cmd.cmd(WOFL_GET_VERSION);
#ifdef WOFL_CMD_STRUCT_ALIGNED
	memcpy(cmd.data, &p, sizeof(p));
	cmd.len = sizeof(p);
#else
	cmd.writeWord(p.lib_ver);
	cmd.writeWord(p.firmware_ver);
	cmd.writeWord(p.debug_mode);
	cmd.writeWord(p.connection_flags);
#endif
}

/* Packs command structure to SerialCommand object */
void WOFL_unsupport_cmd_pack(WOFL_unsupport_cmd_t &p, WOFLCommand &cmd){
    cmd.init(WOFL_TYPE_BGC);
	cmd.cmd(WOFL_UNSUPPORT_CMD);
#ifdef WOFL_CMD_STRUCT_ALIGNED
	memcpy(cmd.data, &p, sizeof(p));
	cmd.len = sizeof(p);
#else
	cmd.writeByte(p.lib_ver);
	cmd.writeWord(p.cmd_id);
#endif
}

