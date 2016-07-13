#ifndef _SERIAL_H
#define _SERIAL_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
extern void serialInit(void);
extern void serialWrite(unsigned char ch);
extern void serialPrint(const char *str);
extern unsigned short serialAvailable(void);
extern unsigned char serialRead(void);
extern void serialSetConstants(void);

extern void serialXWrite(int serial, unsigned char ch);
extern void serialXPrint(int serial,const char *str);
extern unsigned short serialXAvailable(int serial);
extern unsigned char serialXRead(int serial);
#ifdef __cplusplus  
        }  
#endif
#endif
