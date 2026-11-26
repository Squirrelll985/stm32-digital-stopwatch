/**
 * @file    board.h
 * @brief   主机侧单元测试用的板级桩，不包含任何 STM32 头文件
 */
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

static inline uint32_t __get_PRIMASK(void)
{
    return 0u;
}

static inline void __disable_irq(void)
{
}

static inline void __enable_irq(void)
{
}

static inline void __WFI(void)
{
}

#endif /* BOARD_H */
