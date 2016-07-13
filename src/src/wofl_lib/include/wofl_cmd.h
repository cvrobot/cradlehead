#ifndef  __WOFL_command__
#define  __WOFL_command__

// Size of header and checksums
#define WOFL_CMD_NON_PAYLOAD_BYTES 6
// Max. size of a command after packing to bytes
#define WOFL_CMD_MAX_BYTES 255
// Max. size of a payload data
#define WOFL_CMD_DATA_SIZE (WOFL_CMD_MAX_BYTES - WOFL_CMD_NON_PAYLOAD_BYTES)

#define WOFL_LIB_VERSION 100
//COMMAND
#define WOFL_GET_VERSION 1
#define WOFL_SET_CONTROL 2
#define WOFL_GET_ANDLES  3
#define WOFL_RUN_SCRIPT  4
#define WOFL_DEB_SCRIPT  5
#define WOFL_VIR_CONTROL 6
#define WOFL_DBG_CONTROL 7
#define WOFL_UNSUPPORT_CMD	 8
#define WOFL_CMD_ERROR	 9


#endif // __WOFL_command__
