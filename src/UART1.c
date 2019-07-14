// UART1.c

#include "Master.h"

AddIndexFifo(uart1Tx, UART_FIFO_SIZE, uint8_t, UART_SUCCESS, UART_FAILURE)
AddIndexFifo(uart1Rx, UART_FIFO_SIZE, uint8_t, UART_SUCCESS, UART_FAILURE)

void UART1_Init(void){

    SYSCTL_RCGCGPIO_R |= 0x02;                                   // Clock on for Ports B
    SYSCTL_RCGCUART_R |= 0x02;                                   // Clock on for UART1

    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;                           // Unlock Port B
    GPIO_PORTB_CR_R = 0xFF;                                      // Enable Commits on Port B
    GPIO_PORTB_AMSEL_R &= ~0x03;                                 // Disable Analog Mode on PB0 and PB1
    GPIO_PORTB_AFSEL_R |= 0x03;                                  // Enable Alternate Functions on PB0, PB1
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0x000000FF) | 0x00000011; // Set PB1 and PB0 to UART1
    GPIO_PORTB_DIR_R &= ~0x03;                                   // Set Port B as out
    GPIO_PORTB_DEN_R |= 0x03;                                    // Enable Digital Operations on PB0 and PB1

    uart1TxFifo_Init();
    uart1RxFifo_Init();

    UART1_CTL_R &= ~UART_CTL_UARTEN;
    UART1_CC_R = UART_CC_CS_PIOSC;    // Use 16Mhz PIOSC Clock;
    UART1_IBRD_R = 1666;
    UART1_FBRD_R = 43;
    UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    UART1_IFLS_R = (UART_IFLS_TX1_8|UART_IFLS_RX1_8);
    UART1_IM_R = (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);
    UART1_CTL_R |= 0x301;
    NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF00FF)|0x00004000;
    NVIC_EN0_R |= NVIC_EN0_INT6;

}

static void UART1_CopyHardwareToSoftware(void) {
    char byte = 0;
    while(((UART1_FR_R&UART_FR_RXFE) == 0) && (uart1RxFifo_Size() < (UART_FIFO_SIZE-1))) {
        byte = UART1_DR_R;
        uart1RxFifo_Put(byte);
    }
}

static void UART1_CopySoftwareToHardware(void) {
    uint8_t byte = 0;
    while(((UART1_FR_R&UART_FR_TXFF) == 0) && (uart1TxFifo_Size() > 0)) {
        uart1TxFifo_Get(&byte);
        UART1_DR_R = byte;
    }
}

uint8_t UART1_InByte(void) {
    uint8_t byte;
    while(uart1RxFifo_Get(&byte) == UART_FAILURE);
    return (byte);
}

void UART1_OutByte(uint8_t byte) {
    while(uart1TxFifo_Put(byte) == UART_FAILURE);
    UART1_IM_R &= ~UART_IM_TXIM;
    UART1_CopySoftwareToHardware();
    UART1_IM_R |= UART_IM_TXIM;
}

void UART1_Handler(void) {
    if(UART1_RIS_R&UART_RIS_TXRIS) {
        UART1_ICR_R = UART_ICR_TXIC;
        UART1_CopySoftwareToHardware();
        if(uart1TxFifo_Size() == 0) {
            UART1_IM_R &= ~UART_IM_TXIM;
        }
    }
    if(UART1_RIS_R&UART_RIS_RXRIS) {
        UART1_ICR_R = UART_ICR_RXIC;
        UART1_CopyHardwareToSoftware();
    }
    if(UART1_RIS_R&UART_RIS_RTRIS) {
        UART1_ICR_R = UART_ICR_RTIC;
        UART1_CopyHardwareToSoftware();
    }
}
