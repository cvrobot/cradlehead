#ifndef __SIMPLEBGC_H__
#define __SIMPLEBGC_H__
#ifdef __cplusplus  
       extern "C" {  
#endif  
#include "SBGC_lib/SBGC.h"
extern SBGC_Parser sbgc_parser; 
extern void simplebgc_init(void);
extern void simplebgc_process_inqueue(void);
extern void simplebgc_parse(void);//tmp use for debug
#ifdef __cplusplus  
			 }
#endif  
#endif
