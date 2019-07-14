// UART1.h

#ifndef __UART1_H__
#define __UART1_H__ 1

void UART1_Init(void);

uint8_t UART1_InByte(void);

void UART1_OutByte(uint8_t byte);

unsigned short uart1RxFifo_Size(void);

#endif // __UART1_H__
