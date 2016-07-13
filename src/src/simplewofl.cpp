#include "serial.h"
#include "WOFL_lib/wofl.h"
#include "simplewofl.h"
#include "SBGC_lib/SBGC.h"
#include "simplebgc.h"
#define woflserial 2
class WOFLStream{
	public:
	inline uint16_t available(){
            return serialXAvailable(woflserial);
       }
       inline uint8_t read(){
            return serialXRead(woflserial);
       }
       inline void write(uint8_t b){
            serialXWrite(woflserial,b);
       }
};

class WOFLComObj : public WOFL_ComObj {
	WOFLStream *serial;
	public:
	inline void init(WOFLStream *s) {
		serial = s;
	}

	virtual uint16_t getBytesAvailable() {
		return serial->available();
	}
	
	virtual uint8_t readByte() {
		return serial->read();
	}
	
	virtual void writeByte(uint8_t b) {
		serial->write(b);
	}
	
	// Arduino com port is not buffered, so empty space is unknown.
	virtual uint16_t getOutEmptySpace() {
		return 0xFFFF;
	}

};
static WOFLStream stream;
WOFL_Parser wofl_parser;  // SBGC command parser. Define one for each port.
static WOFLComObj com_obj; // COM-port wrapper required for parser
//static SBGC_cmd_realtime_data_t rt_data;

void simplewofl_init(){
	com_obj.init(&stream);
	wofl_parser.init(&com_obj);
}
//start,id,len,hchecksum,data,data checksum
static uint8_t wofl_dbg_send(uint8_t *data,int len, SBGC_Parser &parser) {
	SerialCommand cmd;
    if(data[0] == SBGC_CMD_START_BYTE){
    	cmd.init(data[1]);//id
    	cmd.len = data[2];
        memcpy(cmd.data, data+4, data[2]);
    	return parser.send_cmd(cmd);
    }
    return 1;
}
static void wofl_parse_bgc(WOFLCommand &cmd){
	switch(cmd.id) {
        case WOFL_GET_VERSION:
            WOFL_version_info_t ver;
            memset(&ver,0,sizeof(ver));
            ver.lib_ver= WOFL_LIB_VERSION;
            WOFL_version_info_send(ver,wofl_parser);
            break;
        case WOFL_SET_CONTROL:
            SBGC_cmd_control_t ctl;
            memset(&ctl,0,sizeof(ctl));
            memcpy(&ctl, cmd.data, cmd.len);//asume little endin
            SBGC_cmd_control_send(ctl, sbgc_parser);
            break;
        case WOFL_GET_ANDLES:
            SBGC_get_angles_t angle;
            memset(&angle,0,sizeof(angle));
            memcpy(&angle, cmd.data, cmd.len);//asume little endin
            SBGC_get_angles_send(angle, sbgc_parser);
            break;
        case WOFL_RUN_SCRIPT:
            SBGC_run_script_t script;
            memset(&script,0,sizeof(script));
            memcpy(&script, cmd.data, cmd.len);//asume little endin
            SBGC_run_script_send(script, sbgc_parser);
            break;
        //case WOFL_DEB_SCRIPT:
        //    break;
        case WOFL_VIR_CONTROL:
            SBGC_cmd_api_virt_ch_control_t vir;
            memset(&vir,0,sizeof(vir));
            memcpy(&vir, cmd.data, cmd.len);//asume little endin
            SBGC_cmd_api_virt_ch_control_send(vir, sbgc_parser);
            break;
        case WOFL_DBG_CONTROL:
            wofl_dbg_send(cmd.data, cmd.len, sbgc_parser);
            break;
        default:
            WOFL_unsupport_cmd_t unsupport_cmd;
            memset(&unsupport_cmd,0,sizeof(unsupport_cmd));
            unsupport_cmd.lib_ver = WOFL_LIB_VERSION;
            unsupport_cmd.cmd_id = cmd.id;
            WOFL_unsupport_cmd_send(unsupport_cmd,wofl_parser);
            break;
		}
}
// Process incoming commands. Call it as frequently as possible, to prevent overrun of serial input buffer.
void simplewofl_process_inqueue() {
	while(wofl_parser.read_cmd()) {
		WOFLCommand &cmd = wofl_parser.in_cmd;
		//uint8_t error = 0;
		switch(cmd.type){
                case WOFL_TYPE_BGC:
                    wofl_parse_bgc(cmd);
                    break;
                case WOFL_TYPE_OTH:
                    break;
            }
	}
}
