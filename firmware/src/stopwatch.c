/**
 * @file    stopwatch.c
 * @brief   秒表核心逻辑
 *
 * 计时单位为 10 ms（1 个百分秒）。TIM2 每 10 ms 进入一次中断，
 * 只有 s_running 为 1 时才累加，因此启动/停止不会丢失或重复计数。
 */
#include "stopwatch.h"
#include "board.h"

static volatile uint32_t s_cs;          /* 0 ~ 59999，单位 10 ms */
static volatile uint8_t  s_running;
static volatile uint8_t  s_overflow;    /* 到达 9:59.99 后自动停止 */
static volatile uint8_t  s_record_count;
static volatile int8_t   s_view;        /* -1 表示实时显示，0~2 表示记录号 */
static uint32_t          s_records[STOPWATCH_RECORD_MAX];

static uint32_t crit_enter(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void crit_exit(uint32_t primask)
{
    if (!primask) {
        __enable_irq();
    }
}

void stopwatch_init(void)
{
    uint32_t primask = crit_enter();

    s_cs = 0;
    s_running = 0;
    s_overflow = 0;
    s_record_count = 0;
    s_view = -1;

    crit_exit(primask);
}

void stopwatch_tick_10ms(void)
{
    if (!s_running) {
        return;
    }

    if (s_cs < STOPWATCH_MAX_CS) {
        ++s_cs;
        if (s_cs >= STOPWATCH_MAX_CS) {
            s_cs = STOPWATCH_MAX_CS;
            s_running = 0;
            s_overflow = 1;
        }
    } else {
        s_running = 0;
        s_overflow = 1;
    }
}

void stopwatch_toggle(void)
{
    uint32_t primask = crit_enter();

    /* 在记录查看状态下按 K1：先回到实时值，再执行启动/停止 */
    s_view = -1;

    if (s_running) {
        s_running = 0;
    } else {
        if (s_cs >= STOPWATCH_MAX_CS) {
            s_cs = 0;
            s_overflow = 0;
        }
        s_running = 1;
    }

    crit_exit(primask);
}

void stopwatch_clear(void)
{
    uint32_t primask = crit_enter();

    s_cs = 0;
    s_running = 0;
    s_overflow = 0;
    s_record_count = 0;
    s_view = -1;

    crit_exit(primask);
}

void stopwatch_record(void)
{
    uint32_t primask = crit_enter();

    if (s_record_count < STOPWATCH_RECORD_MAX) {
        s_records[s_record_count] = s_cs;
        ++s_record_count;
    }
    s_view = -1;   /* 记录后回到实时显示 */

    crit_exit(primask);
}

void stopwatch_view_next(void)
{
    uint32_t primask = crit_enter();

    if (s_record_count == 0u) {
        s_view = -1;
    } else if (s_view < 0) {
        s_view = 0;
    } else if ((uint8_t)(s_view + 1) < s_record_count) {
        ++s_view;
    } else {
        s_view = -1;
    }

    crit_exit(primask);
}

uint32_t stopwatch_value(void)
{
    return s_cs;
}

uint8_t stopwatch_is_running(void)
{
    return s_running;
}

uint8_t stopwatch_is_overflow(void)
{
    return s_overflow;
}

uint8_t stopwatch_view_index(void)
{
    return (s_view < 0) ? STOPWATCH_VIEW_NONE : (uint8_t)s_view;
}

uint8_t stopwatch_record_count(void)
{
    return s_record_count;
}

uint32_t stopwatch_record_value(uint8_t index)
{
    if (index >= STOPWATCH_RECORD_MAX) {
        return 0;
    }
    return s_records[index];
}
