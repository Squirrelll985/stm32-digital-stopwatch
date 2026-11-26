/**
 * @file    keys.c
 * @brief   按键 1 ms 采样、20 ms 消抖
 *
 * 计数式消抖：按下时计数加 1，释放时计数减 1。
 * 计数达到 20（约 20 ms）才产生一次按下事件；
 * 松开后计数回到 0，再次按下才会产生新事件。
 */
#include "keys.h"
#include "board.h"

#define KEY_DEBOUNCE_MS  20u

static uint8_t s_count[4];
static volatile uint8_t s_events;

void keys_init(void)
{
    uint8_t i;

    for (i = 0; i < 4u; ++i) {
        s_count[i] = 0;
    }
    s_events = 0;
}

void keys_scan_1ms(void)
{
    uint32_t idr = KEY_GPIO->IDR;
    uint8_t  i;

    for (i = 0; i < 4u; ++i) {
        uint8_t pressed;

#if KEY_ACTIVE_LOW
        pressed = ((idr & (1u << (KEY_SHIFT + i))) == 0u) ? 1u : 0u;
#else
        pressed = ((idr & (1u << (KEY_SHIFT + i))) != 0u) ? 1u : 0u;
#endif
        if (pressed) {
            if (s_count[i] < KEY_DEBOUNCE_MS) {
                ++s_count[i];
                if (s_count[i] == KEY_DEBOUNCE_MS) {
                    s_events |= (uint8_t)(1u << i);
                }
            }
        } else if (s_count[i] > 0u) {
            --s_count[i];
        }
    }
}

uint8_t keys_take_events(void)
{
    uint32_t primask = __get_PRIMASK();
    uint8_t  events;

    __disable_irq();
    events = s_events;
    s_events = 0;
    if (!primask) {
        __enable_irq();
    }
    return events;
}
