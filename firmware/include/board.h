/**
 * @file    board.h
 * @brief   STM32F103C8T6 最小系统板级支持
 *
 * 引脚分配（与报告表 3.2 一致）：
 *   PB0~PB7  -> 数码管段 a~g、dp（经 510Ω 限流）
 *   PA0~PA3  -> 数码管四位公共端 D1~D4（共阴极时低电平有效）
 *   PB12~PB15-> 按键 K1~K4（低电平有效，内部上拉）
 *   PC13     -> 板载 LED（低电平点亮）
 */
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "stm32f1xx.h"

/**
 * 1 = 共阴极数码管：段高电平点亮，位选低电平有效
 * 0 = 共阳极数码管：段低电平点亮，位选高电平有效
 * 3461AS 一般为共阴极；若实物为 3461BS，请改为 0。
 */
#ifndef DISPLAY_COMMON_CATHODE
#define DISPLAY_COMMON_CATHODE 1
#endif

/**
 * 1 = 按键按下时接地（低电平有效）
 * 0 = 按键模块输出高电平有效
 */
#ifndef KEY_ACTIVE_LOW
#define KEY_ACTIVE_LOW 1
#endif

#define SEG_GPIO            GPIOB
#define SEG_MASK            0x00FFu

#define DIG_GPIO            GPIOA
#define DIG_MASK            0x000Fu

#define KEY_GPIO            GPIOB
#define KEY_MASK            0xF000u
#define KEY_SHIFT           12u

#define LED_GPIO            GPIOC
#define LED_PIN             13u

void     board_init(void);
uint32_t board_millis(void);
void     board_delay_ms(uint32_t ms);
void     board_led_set(int on);
void     board_led_toggle(void);

/* 由 TIM3 的 1 ms 中断调用 */
void     board_tick_1ms(void);

#endif /* BOARD_H */
