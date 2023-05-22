#ifndef _AD_H_
#define _AD_H_

#define AD_BUF_LEN 4
#define AD_THRESHOLD 0x0020
#define AD_LO 0x0020
#define AD_HI 0x0FE0
void ADInit(void);
void ADStart(void);

#endif
