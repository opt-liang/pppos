#ifndef __HARDWARE__INTERFACE__H__
#define __HARDWARE__INTERFACE__H__
#include "stm32f1xx_hal.h"
#include <stdbool.h>
/*************************************************/

extern void _Error_Handler(char *, int);
extern void test( void );

extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern UART_HandleTypeDef huart3;

void MX_UART_Config( UART_HandleTypeDef *huart, int baud );

void xHAL_UART_TxCpltCallback( void );

void InitPeripherals( void );

void UartSendData( UART_HandleTypeDef* huart, uint8_t *pdata, uint16_t Length );
#endif

