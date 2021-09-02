#include "aes.h"
#include "tim.h"
#include "usart.h"
#include <stddef.h>

extern uint8_t aes_key;
extern uint8_t encrpt_msg;
uint8_t decrypt_block[32];
uint32_t measur_time;

void decryp_measur(void)
{
    /* starting the timer */
    measur_time = 0;
    LL_TIM_ClearFlag_UPDATE(TIM1);
    LL_TIM_SetCounter(TIM1, 0);
    LL_TIM_EnableIT_UPDATE(TIM1);
    LL_TIM_EnableCounter(TIM1);
    /**********************/

    /* starting decryption */
    AES_ECB_decrypt(&encrpt_msg, &aes_key, decrypt_block, sizeof(encrpt_msg) / sizeof(uint8_t));
    /***********************/

    /* getting the time and disable timer */
    measur_time += LL_TIM_GetCounter(TIM1);
    LL_TIM_DisableCounter(TIM1);
    LL_TIM_ClearFlag_UPDATE(TIM1);
    LL_TIM_SetCounter(TIM1, 0);
    LL_TIM_DisableIT_UPDATE(TIM1);
    /**************************************/

    /* UART package */
    for (size_t i = 0; i < sizeof(decrypt_block) / sizeof(uint8_t); i++) {
        LL_USART_TransmitData8(USART1, decrypt_block[i]); //transmit data
    }

    uint8_t measur_uart[4];
    measur_uart[0] = (uint8_t)measur_time;
    measur_uart[1] = (uint8_t)(measur_time >> 8);
    measur_uart[2] = (uint8_t)(measur_time >> 16);
    measur_uart[3] = (uint8_t)(measur_time >> 24);

    for (size_t i = 0; i < 4; i++) {
        LL_USART_TransmitData8(USART1, measur_uart[i]); //transmit time
    }
    /****************/
}