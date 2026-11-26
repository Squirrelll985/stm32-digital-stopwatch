/**
 * @file    stopwatch.h
 * @brief   秒表核心状态机：计时、启动/停止、清零、计次与翻阅
 */
#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <stdint.h>

#define STOPWATCH_MAX_CS      59999u   /* 9 min 59.99 s，单位 10 ms */
#define STOPWATCH_RECORD_MAX  3u
#define STOPWATCH_VIEW_NONE   0xFFu

void     stopwatch_init(void);

/* 由 TIM2 的 10 ms 中断调用 */
void     stopwatch_tick_10ms(void);

void     stopwatch_toggle(void);      /* K1：启动 / 停止 */
void     stopwatch_clear(void);       /* K2：停止并清零 */
void     stopwatch_record(void);      /* K3：记录一组中间时间 */
void     stopwatch_view_next(void);   /* K4：翻阅记录，第 4 次回到实时值 */

uint32_t stopwatch_value(void);       /* 当前值，单位 10 ms */
uint8_t  stopwatch_is_running(void);
uint8_t  stopwatch_is_overflow(void);
uint8_t  stopwatch_view_index(void);  /* 0~2，或 STOPWATCH_VIEW_NONE */
uint8_t  stopwatch_record_count(void);
uint32_t stopwatch_record_value(uint8_t index);

#endif /* STOPWATCH_H */
