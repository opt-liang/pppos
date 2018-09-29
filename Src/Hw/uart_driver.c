#include "stm32f1xx_hal.h"
#include "cycle_queue.h"
#include "cmsis_os.h"
extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;
SeqCQueue   seqCQueue;
#define DEBUG_INFO(fmt,args...)  printf(fmt, ##args)

/*-------------------private variable----------------------------------*/
DataType cache;
USART_RECEIVETYPE UsartType;

/*-------------------public function----------------------------------*/

void MX_UART_Config( UART_HandleTypeDef *huart, int baud ){
    DEBUG_INFO("Change the baud rate:%d\r\n", baud );
    UART_REINIT:
    HAL_UART_DeInit( huart );
    huart->Init.BaudRate = baud;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(huart) != HAL_OK)
    {
        DEBUG_INFO("initialization failed\r\n" );
        osDelay( 1000 );      
        goto UART_REINIT;
    }

    HAL_UART_Receive_DMA( huart, seqCQueue.currentCache, LEFTRAMSIZE );

    __HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);

    if( huart->Instance == USART3 ){
      
        /* UART3_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);           //add code
        HAL_NVIC_EnableIRQ(USART3_IRQn);

        USART3->SR;

        HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 5, 0); //add code
        HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

        HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);   //add code
        HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);            
    }else{
        printf("ERROR PARA");
    }
}

void InitPeripherals( void ){
      QueueInitiate( &seqCQueue );
}

void UartSendData( UART_HandleTypeDef* huart, uint8_t *pdata, uint16_t Length ){
    
    if( HAL_UART_Transmit( huart, pdata, Length, 100 ) != HAL_OK ){
        HAL_UART_DMAResume( huart );
        HAL_UART_Receive_DMA( huart, seqCQueue.currentCache, LEFTRAMSIZE );
    }
}

void UartIdleReceiveData( UART_HandleTypeDef* huart, SeqCQueue *Queue ){
    
    if( ( __HAL_UART_GET_FLAG( huart,UART_FLAG_IDLE ) != RESET) ){
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(huart);
        uint32_t temp = huart->hdmarx->Instance->CNDTR;
		if( ( LEFTRAMSIZE - temp ) != 0x00 && ( LEFTRAMSIZE - temp ) < LEFTRAMSIZE ){
            cache.size  = ( LEFTRAMSIZE - temp ) ;
            cache.index = Queue->currentCache;
            if( QueueAppend( Queue, cache ) ){
                Queue->leftram += cache.size;
                if( ( RECEIVEBUFLEN - Queue->leftram ) >= LEFTRAMSIZE ){
                    Queue->currentCache += cache.size;
                }else{
                    Queue->leftram = 0;
                    Queue->currentCache = Queue->heapcache;
                }
            }
		}else{
            HAL_UART_DMAResume( huart );
        }
		
		HAL_UART_Receive_DMA( huart, Queue->currentCache, LEFTRAMSIZE );
    }
}

/********************************************************************************/


