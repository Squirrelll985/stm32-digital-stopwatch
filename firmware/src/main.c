/**
 * @file    main.c
 * @brief   基于 STM32F103C8T6 的数字秒表
 *
 * 操作：
 *   K1  PB12  启动 / 停止（再按一次停止）
 *   K2  PB13  清零（停止并清除全部计次）
 *   K3  PB14  记录中间时间，最多 3 组
 *   K4  PB15  翻阅记录：依次显示第 1~3 组，最后一组之后回到实时值
 */
#include "stm32f1xx.h"
#include "board.h"
#include "display.h"
#include "keys.h"
#include "stopwatch.h"
#include "tim.h"
#include "ui.h"

int main(void)
{
    board_init();
    display_init();
    keys_init();
    stopwatch_init();
    ui_init();
    tim_init();

    __enable_irq();

    for (;;) {
        uint8_t events = keys_take_events();

        if (events & KEY_K1) {
            stopwatch_toggle();
        }
        if (events & KEY_K2) {
            stopwatch_clear();
        }
        if (events & KEY_K3) {
            stopwatch_record();
        }
        if (events & KEY_K4) {
            stopwatch_view_next();
        }

        ui_update();

        /* 没有事件时进入低功耗等待，TIM3 每 1 ms 唤醒一次 */
        __WFI();
    }
}
