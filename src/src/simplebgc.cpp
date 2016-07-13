#include "serial.h"
#include "SBGC_lib/SBGC.h"
#include "simplebgc.h"
#include "WOFL_lib/wofl.h"
#include "simplewofl.h"
#define sbgcserial 1
class Stream{
	public:
	inline uint16_t available(){
            return serialXAvailable(sbgcserial);
       }
       inline uint8_t read(){
            return serialXRead(sbgcserial);
       }
       inline void write(uint8_t b){
            serialXWrite(sbgcserial,b);
       }
};

class Stm32ComObj : public SBGC_ComObj {
	Stream *serial;
	public:
	inline void init(Stream *s) {
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
static Stream stream;
SBGC_Parser sbgc_parser;  // SBGC command parser. Define one for each port.
static Stm32ComObj com_obj; // COM-port wrapper required for parser
static SBGC_cmd_realtime_data_t rt_data;

void simplebgc_init(){
	com_obj.init(&stream);
	sbgc_parser.init(&com_obj);
}

// Process incoming commands. Call it as frequently as possible, to prevent overrun of serial input buffer.
void simplebgc_process_inqueue() {
    int i; 
	while(sbgc_parser.read_cmd()) {
		SerialCommand &cmd = sbgc_parser.in_cmd;
		
		uint8_t error = 0;
		char tmp[12];
		switch(cmd.id) {
            case SBGC_CMD_GET_ANGLES:
                WOFL_get_angles_t t;
                memcpy(&t, cmd.data, cmd.len);//asume little endin
                WOFL_get_angles_send(t, wofl_parser);
                break;
   
		// Receive realtime data
		case SBGC_CMD_REALTIME_DATA_3:
		case SBGC_CMD_REALTIME_DATA_4:
			error = SBGC_cmd_realtime_data_unpack(rt_data, cmd);
			if(!error) {
				// Extract some usefull data
				/*
                rt_data.imu_angle[ROLL];
                rt_data.target_angle[ROLL];
				rt_data.imu_angle[PITCH];
                rt_data.target_angle[PITCH];
				rt_data.imu_angle[YAW];
                rt_data.target_angle[YAW];
                */
                memcpy(tmp,rt_data.rc_raw_data,12);
                serialXPrint(2,":");
                for(i=0;i<12;i++)
                    serialXWrite(2,tmp[i]);
				serialXWrite(2,'\n');
			} else {
				sbgc_parser.onParseError(error);
			}
			break;
		    default:
		        serialPrint("unparsed command:");
                serialWrite(cmd.id);
		        for(i=0;i<cmd.len;i++)
                    serialWrite(cmd.data[i]);
                serialWrite('\n');
		}
	}

}

void simplebgc_parse(void){
        SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
        c.mode = SBGC_CONTROL_MODE_ANGLE;
        c.speedROLL = c.speedPITCH = c.speedYAW = 30 * SBGC_SPEED_SCALE;
        SBGC_cmd_control_send(c, sbgc_parser);
}
