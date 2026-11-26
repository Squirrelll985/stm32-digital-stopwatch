/**
 * @file    display.h
 * @brief   4 位共阴/共阳数码管动态扫描驱动
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/* 段码位定义：bit0=a, bit1=b, ..., bit6=g, bit7=dp */
#define SEG_A   0x01u
#define SEG_B   0x02u
#define SEG_C   0x04u
#define SEG_D   0x08u
#define SEG_E   0x10u
#define SEG_F   0x20u
#define SEG_G   0x40u
#define SEG_DP  0x80u

void    display_init(void);

/**
 * @brief 设置一帧显示内容（4 个字节的段码，bit7 为小数点）
 * @note  采用一次 32 位写入，主循环与扫描中断之间不会出现半帧撕裂。
 */
void    display_set_frame(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
void    display_blank(void);

/* 取 0~9 的段码，dp=1 时附加小数点 */
uint8_t display_code(uint8_t digit, uint8_t dp);

/* 由 TIM3 的 1 ms 中断调用，每次点亮一位 */
void    display_scan(void);

#endif /* DISPLAY_H */
