#ifndef __WOFL_H__
#define __WOFL_H__
#ifdef __cplusplus  
       extern "C" {  
#endif  
#include "WOFL_lib/wofl.h"
extern WOFL_Parser wofl_parser; 
extern void simplewofl_init(void);
extern void simplewofl_process_inqueue(void);
#ifdef __cplusplus  
			 }
#endif  
#endif
