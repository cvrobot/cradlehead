#ifndef  __WOFL_parser__
#define  __WOFL_parser__

#include <inttypes.h>
#include <string.h>
#include "wofl_cmd.h"

//PROTOCOL
//SOH,TYPE,CMD,LEN,STX,DATA,ETX
#if 0
#define WOFL_PROTOCOL_SOH		'\1' //ASSIC SOH
#define WOFL_PROTOCOL_STX		'\2' //ASSIC STX
#define WOFL_PROTOCOL_ETX		'\3' //ASSIC ETX
#else
#define WOFL_PROTOCOL_SOH		'!' //ASSIC SOH
#define WOFL_PROTOCOL_STX		'@' //ASSIC STX
#define WOFL_PROTOCOL_ETX		'#' //ASSIC ETX
#endif
typedef enum {
	WOFL_TYPE_BGC=0,
	WOFL_TYPE_OTH=1,
	WOFL_TYPE_MAX,
} WOLF_types;

typedef enum {
	WOFL_NO_ERROR=0,
	WOFL_ERROR_PROTOCOL=1,
	WOFL_ERROR_WRONG_CMD_SIZE=2,
	WOFL_ERROR_BUFFER_IS_FULL=3,
	WOFL_ERROR_WRONG_DATA_SIZE=4,
} WOLF_parser_errors;


/*
 * Abstract class that implements general primitive reading/writing from stream.
 */
class WOFL_IOStream {
public:

    // Methods need to be implemented
    virtual uint16_t getBytesAvailable() = 0;
    virtual uint8_t readByte() = 0;
    virtual void writeByte(uint8_t b) = 0;


    int16_t readWord() {
        return (uint16_t)readByte() + ((uint16_t)readByte()<<8);
    }


#ifdef SYS_LITTLE_ENDIAN
	// Optimization for little-endian machines only!
    inline void readWordArr(int16_t *arr, uint8_t size) {
		readBuf(arr, (size*2));
	}
#else
    void readWordArr(int16_t *arr, uint8_t size) {
	    for(uint8_t i=0; i<size; i++) {
		    arr[i] = readWord();
	    }
    }
#endif
	
    int32_t readLong() {
        return (int32_t)readByte() + ((int32_t)readByte()<<8) + ((int32_t)readByte()<<16) + ((int32_t)readByte()<<24);
    }

    void readBuf(void* buf, uint8_t size) {
        for(uint8_t i = 0; i < size; i++) {
            ((uint8_t*)buf)[i] = readByte();
        }
    }

    float readFloat() {
        float f;
        readBuf(&f, sizeof(float));
        return f;
    }

    void skipBytes(uint8_t size) {
        while(size-- > 0) {
            readByte();
        }
    }

    void writeWord(int16_t w) {
        writeByte(w); // low
        writeByte(w>>8); // high
    }


#ifdef SYS_LITTLE_ENDIAN
    // Optimization for little-endian machines only!
    inline void writeWordArr(int16_t *arr, uint8_t size) {
		writeBuf(arr, (size*2));
	}
#else
    void writeWordArr(int16_t *arr, uint8_t size) {
        for(uint8_t i=0; i<size; i++) {
            writeWord(arr[i]);
        }
    }
#endif

    void writeLong(int32_t dw) {
        writeWord(dw); // low word
        writeWord(dw>>16); // high word
    }


    void writeFloat(float f) {
        writeBuf(&f, sizeof(float));
    }

    void writeBuf(const void* buf, uint8_t size) {
        for(uint8_t i=0; i<size; i++) {
            writeByte(((uint8_t*)buf)[i]);
        }
    }

    void writeString(const char* str) {
        uint8_t len = strlen(str);
        writeByte(len);
        writeBuf(str, len);
    }

    void writeEmptyBuf(uint8_t size) {
        while(size-- > 0) {
            writeByte(0);
        }
    }
};


/* Class to manipulate command data */
class WOFLCommand : public WOFL_IOStream {
public:
	uint8_t pos;
	uint8_t type;
	uint8_t id;
	uint8_t data[WOFL_CMD_DATA_SIZE];
	uint8_t len;


	/* Check if limit reached after reading data buffer */
	virtual uint8_t checkLimit() {
		return len == pos;
	}

	virtual uint16_t getBytesAvailable() {
		return len - pos;
	}

	
	void init(uint8_t c) {
		type = c;
		id = 0;
		len = 0;
		pos = 0;
	}

	void cmd(uint8_t c) {
		id = c;
	}
	inline void reset() {
		len = 0;
		pos = 0;
	}
	
	
	virtual uint8_t readByte() {
		if(pos < len) {
			return data[pos++];
		} else {
			pos++;
			return 0;
		}
	}

	
	virtual void writeByte(uint8_t b)  {
		if(len < sizeof(data)) {
			data[len++] = b;
		}
	}

};


/* Need to be implemented in the main code */
class WOFL_ComObj {
public:
	// Return the number of bytes received in input buffer
	virtual uint16_t getBytesAvailable() = 0;
	// Read byte from the input stream
	virtual uint8_t readByte() = 0;
	// Write byte to the output stream
	virtual void writeByte(uint8_t b) = 0;
	// Return the space available in the output buffer. Return 0xFFFF if unknown.
	virtual uint16_t getOutEmptySpace() = 0;
};


/* Optimized version of a parser, that does not require a separate buffer for maintain the parsed data */
class WOFL_Parser {
	WOFL_ComObj *com_obj;
	enum {STATE_WAIT, STATE_GOT_SOH, STATE_GOT_TYPE, STATE_GOT_ID, STATE_GOT_LEN, STATE_GOT_STX, STATE_GOT_DATA } state;
	uint16_t len;
	uint8_t checksum;
	uint16_t parser_error_count;
	
	
public:
    WOFLCommand in_cmd; // received command is stored here
	
	inline void init(WOFL_ComObj *_com_obj) {
		com_obj = _com_obj;	
	    state = STATE_WAIT;
		parser_error_count = 0;
	}
	
	inline void onParseError(uint8_t error = WOFL_ERROR_PROTOCOL) { 
		parser_error_count++; 
	}
 
	/*
	 * Parses next character in a stream. 
	 * Returns 1 if command successfully parsed, 0 otherwise.
	 * Calls  onParseError() in case of errors
	 */
	inline uint8_t process_char(uint8_t c) {
		switch(state) {
			case STATE_WAIT:
				if(c == WOFL_PROTOCOL_SOH) {
					state = STATE_GOT_SOH;
				} else {
					//onParseError();
				}
				break;

			case STATE_GOT_SOH:
				if(c >= WOFL_TYPE_MAX){
					onParseError(WOFL_ERROR_PROTOCOL);
					state = STATE_WAIT;
				}else{
					in_cmd.init(c); // got command TYPE
					state = STATE_GOT_TYPE;
				}
				break;

			case STATE_GOT_TYPE:
				in_cmd.cmd(c);	// got command id
				state = STATE_GOT_ID;
				break;

			case STATE_GOT_ID:
				len = c;
				state = STATE_GOT_LEN;
				break;

			case STATE_GOT_LEN:
				if (c == WOFL_PROTOCOL_STX) { //
					if(len == 0)
						state = STATE_GOT_DATA;//no data
					else
						state = STATE_GOT_STX;
				} else {
					onParseError();
					state = STATE_WAIT;
				}
				break;

			case STATE_GOT_STX:
				in_cmd.data[in_cmd.len++] = c;
				if(in_cmd.len == len) state = STATE_GOT_DATA;
				break;

			case STATE_GOT_DATA:
				state = STATE_WAIT;

				if(c == WOFL_PROTOCOL_ETX) {
					return 1;
				} else {
					onParseError();
				}
				break;
		}

		return 0;
	}

	/* Parse and fill SerialCommand object SBGC_Parser.in_cmd;
	 * Returns 1 if command is parsed, 0 otherwise.
	 */
	inline int8_t read_cmd() {
		while(com_obj->getBytesAvailable()) {
			if(process_char(com_obj->readByte())) {
				return 1;
			}
		}

		return 0;
	}
	
	
	/*
	 * Formats and writes command to serial port.
	 * If wait=1, waits even if output buffer is full.
	 * If wait=0, writes only if output buffer has enough space to accept this command.
	 * Returns 0 on success, PARSER_ERR_xx on fail.
	 */
	uint8_t send_command(uint8_t cmd_id, uint8_t cmd_type, void *data, uint16_t size, uint8_t wait = 1) {
		if(com_obj != NULL && size <= (WOFL_CMD_MAX_BYTES - WOFL_CMD_NON_PAYLOAD_BYTES)) {
			if(wait || com_obj->getOutEmptySpace() >= size + WOFL_CMD_NON_PAYLOAD_BYTES) {
				com_obj->writeByte(WOFL_PROTOCOL_SOH); // protocol-specific start marker
				com_obj->writeByte(cmd_type); // command id
				com_obj->writeByte(cmd_id); // command id
				com_obj->writeByte(size); // data body length
				com_obj->writeByte(WOFL_PROTOCOL_STX); // header checksum

				// Write data
				for(uint8_t i = 0; i < size; i++) {
					com_obj->writeByte(((uint8_t*)data)[i]);
				}
				com_obj->writeByte(WOFL_PROTOCOL_ETX); // data checksum

				return 0;
			} else {
				return WOFL_ERROR_BUFFER_IS_FULL;
			}
		} else {
			return WOFL_ERROR_WRONG_CMD_SIZE;
		}
	}

	/* 
	* Send command from the SerialCommand object to serial port. See send_command() description.
	*/
	inline uint8_t send_cmd(WOFLCommand &cmd, uint8_t wait = 1) {
		return send_command(cmd.id, cmd.type, cmd.data, cmd.len, wait);
	}

	/*
	* Get parsing errors counter
	*/
	inline uint16_t get_parse_error_count() { return parser_error_count; }
	
	
	/*
	* Resets the state of a parser
	*/
	inline void reset() { 
		state = STATE_WAIT; 
	}
		

	/*
	* Returns the space available in the output buffer
	*/
	inline uint16_t get_out_empty_space() {
		return com_obj != NULL ? com_obj->getOutEmptySpace() : 0;
	}
	
};



#endif //__WOFL_parser__
