/**
 * @file    board.c
 * @brief   GPIO、系统节拍与板载 LED
 */
#include "board.h"

static volatile uint32_t s_millis;

void board_init(void)
{
    /* I/O 时钟；AFIO 时钟在本工程中未使用重映射，但一并打开便于扩展 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN |
                    RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

    /* PA0~PA3：数码管位选，2 MHz 推挽输出 */
    DIG_GPIO->BSRR = DIG_MASK;                 /* 先写 1，默认关闭所有位 */
    DIG_GPIO->CRL  = (DIG_GPIO->CRL & ~0x0000FFFFu) | 0x00002222u;

    /* PB0~PB7：段选，2 MHz 推挽输出 */
#if DISPLAY_COMMON_CATHODE
    SEG_GPIO->BRR  = SEG_MASK;                 /* 共阴极：段低电平熄灭 */
#else
    SEG_GPIO->BSRR = SEG_MASK;                 /* 共阳极：段高电平熄灭 */
#endif
    SEG_GPIO->CRL  = 0x22222222u;

    /* PB12~PB15：按键输入，上拉 */
    KEY_GPIO->BSRR = KEY_MASK;
    KEY_GPIO->CRH  = (KEY_GPIO->CRH & ~0xFFFF0000u) | 0x88880000u;

    /* PC13：板载 LED，低电平点亮 */
    LED_GPIO->BSRR = (1u << LED_PIN);
    LED_GPIO->CRH  = (LED_GPIO->CRH & ~0x00F00000u) | 0x00200000u;

    SystemCoreClockUpdate();
}

uint32_t board_millis(void)
{
    return s_millis;
}

void board_tick_1ms(void)
{
    ++s_millis;
}

void board_delay_ms(uint32_t ms)
{
    uint32_t start = s_millis;

    while ((s_millis - start) < ms) {
        __WFI();
    }
}

void board_led_set(int on)
{
    if (on) {
        LED_GPIO->BRR  = (1u << LED_PIN);
    } else {
        LED_GPIO->BSRR = (1u << LED_PIN);
    }
}

void board_led_toggle(void)
{
    if (LED_GPIO->ODR & (1u << LED_PIN)) {
        board_led_set(1);
    } else {
        board_led_set(0);
    }
}
