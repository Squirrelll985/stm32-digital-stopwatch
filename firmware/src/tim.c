/**
 * @file    tim.c
 * @brief   TIM2 10 ms 时基、TIM3 1 ms 显示与按键节拍
 *
 * TIM2: 72 MHz / (7199+1) = 10 kHz，计数到 99 溢出 -> 100 Hz（10 ms）
 * TIM3: 72 MHz / (71+1)   = 1 MHz，计数到 999 溢出 -> 1 kHz（1 ms）
 */
#include "tim.h"
#include "stm32f1xx.h"
#include "board.h"
#include "display.h"
#include "keys.h"
#include "stopwatch.h"

void tim_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;

    /* ---- TIM2：10 ms 计时时基 ---- */
    TIM2->PSC  = 7199u;
    TIM2->ARR  = 99u;
    TIM2->EGR  = TIM_EGR_UG;       /* 立即装入预分频值 */
    TIM2->SR   = 0u;
    TIM2->DIER = TIM_DIER_UIE;
    NVIC_SetPriority(TIM2_IRQn, 0u);
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1  = TIM_CR1_CEN;

    /* ---- TIM3：1 ms 显示/按键节拍 ---- */
    TIM3->PSC  = 71u;
    TIM3->ARR  = 999u;
    TIM3->EGR  = TIM_EGR_UG;
    TIM3->SR   = 0u;
    TIM3->DIER = TIM_DIER_UIE;
    NVIC_SetPriority(TIM3_IRQn, 1u);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1  = TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = ~TIM_SR_UIF;
        stopwatch_tick_10ms();
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR = ~TIM_SR_UIF;
        board_tick_1ms();
        display_scan();
        keys_scan_1ms();
    }
}
