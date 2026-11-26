/**
 * @file    ui.c
 * @brief   显示格式与状态指示
 *
 * 4 位数码管的显示约定：
 *   0.00 ~ 59.99 s : SS.CC，分辨力 0.01 s（与实物照片 08.08 一致）
 *   60.00 ~ 599.99 s: M.SS.C，分辨力 0.1 s（第一、三位小数点亮）
 *   查看记录时    : 先显示 r1/r2/r3 约 0.7 s，再闪烁显示该组时间
 */
#include "ui.h"
#include "board.h"
#include "display.h"
#include "stopwatch.h"

#define RECORD_INDEX_MS  700u
#define BLINK_HALF_MS    250u

static int8_t   s_last_view = -1;
static uint32_t s_view_start;

static void format_time(uint32_t cs, uint8_t out[4], uint8_t *dp_mask)
{
    if (cs < 6000u) {
        uint32_t sec = cs / 100u;
        uint32_t cc  = cs % 100u;

        out[0] = (uint8_t)(sec / 10u);
        out[1] = (uint8_t)(sec % 10u);
        out[2] = (uint8_t)(cc / 10u);
        out[3] = (uint8_t)(cc % 10u);
        *dp_mask = 0x02u;                 /* 第 2 位后的小数点 */
    } else {
        uint32_t min  = cs / 6000u;
        uint32_t rem  = cs % 6000u;
        uint32_t sec  = rem / 100u;
        uint32_t tenth = (rem % 100u) / 10u;

        out[0] = (uint8_t)min;
        out[1] = (uint8_t)(sec / 10u);
        out[2] = (uint8_t)(sec % 10u);
        out[3] = (uint8_t)tenth;
        *dp_mask = 0x05u;                 /* 第 1、3 位后的小数点 */
    }
}

static void show_time(uint32_t cs)
{
    uint8_t d[4];
    uint8_t dp;

    format_time(cs, d, &dp);
    display_set_frame(display_code(d[0], (uint8_t)(dp & 0x01u)),
                      display_code(d[1], (uint8_t)(dp & 0x02u)),
                      display_code(d[2], (uint8_t)(dp & 0x04u)),
                      display_code(d[3], (uint8_t)(dp & 0x08u)));
}

void ui_init(void)
{
    display_blank();
    board_led_set(0);
    s_last_view = -1;
    s_view_start = board_millis();
}

void ui_update(void)
{
    uint32_t now   = board_millis();
    uint8_t  view  = stopwatch_view_index();
    uint8_t  blink = (uint8_t)((now / BLINK_HALF_MS) & 1u);

    if (view != STOPWATCH_VIEW_NONE) {
        if ((int8_t)view != s_last_view) {
            s_last_view = (int8_t)view;
            s_view_start = now;
        }

        if ((now - s_view_start) < RECORD_INDEX_MS) {
            /* 先显示组号：r1、r2、r3 */
            display_set_frame(0x50u,                    /* 'r' = e+g */
                              display_code((uint8_t)(view + 1u), 0),
                              0x00u, 0x00u);
        } else if (blink) {
            display_blank();
        } else {
            show_time(stopwatch_record_value(view));
        }

        board_led_set(blink);
    } else {
        s_last_view = -1;

        if (stopwatch_is_overflow() && blink) {
            display_blank();
        } else {
            show_time(stopwatch_value());
        }
        board_led_set(stopwatch_is_running());
    }
}
