/**
 * @file    keys.h
 * @brief   4 路独立按键扫描与 20 ms 软件消抖
 */
#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>

#define KEY_K1  0x01u
#define KEY_K2  0x02u
#define KEY_K3  0x04u
#define KEY_K4  0x08u

void    keys_init(void);

/* 由 TIM3 的 1 ms 中断调用 */
void    keys_scan_1ms(void);

/* 主循环取走已消抖的按下事件（一次有效） */
uint8_t keys_take_events(void);

#endif /* KEYS_H */
