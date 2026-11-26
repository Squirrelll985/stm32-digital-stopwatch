/**
 * @file    display.c
 * @brief   4 位数码管动态扫描
 *
 * 扫描节拍 1 kHz，每位点亮 1 ms，整帧 4 ms（250 Hz），
 * 高于人眼临界闪烁频率，显示稳定无闪烁。
 */
#include "display.h"
#include "board.h"

/* 共阴极 0~9 段码：a b c d e f g */
static const uint8_t k_seg_code[10] = {
    0x3Fu, /* 0 */
    0x06u, /* 1 */
    0x5Bu, /* 2 */
    0x4Fu, /* 3 */
    0x66u, /* 4 */
    0x6Du, /* 5 */
    0x7Du, /* 6 */
    0x07u, /* 7 */
    0x7Fu, /* 8 */
    0x6Fu  /* 9 */
};

static volatile uint32_t s_frame;   /* 4 个段码，低字节为第 1 位 */
static uint8_t s_scan_index;

void display_init(void)
{
    s_frame = 0;
    s_scan_index = 0;
#if DISPLAY_COMMON_CATHODE
    DIG_GPIO->BSRR = DIG_MASK;
    SEG_GPIO->BRR  = SEG_MASK;
#else
    DIG_GPIO->BRR  = DIG_MASK;
    SEG_GPIO->BSRR = SEG_MASK;
#endif
}

uint8_t display_code(uint8_t digit, uint8_t dp)
{
    uint8_t code = (digit < 10u) ? k_seg_code[digit] : 0x00u;

    if (dp) {
        code |= SEG_DP;
    }
    return code;
}

void display_set_frame(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    /* Cortex-M3 的 32 位对齐写入是原子的，扫描中断不会读到半帧 */
    s_frame = (uint32_t)d0 |
              ((uint32_t)d1 << 8) |
              ((uint32_t)d2 << 16) |
              ((uint32_t)d3 << 24);
}

void display_blank(void)
{
    s_frame = 0;
}

void display_scan(void)
{
    uint32_t frame = s_frame;
    uint8_t  code  = (uint8_t)(frame >> (s_scan_index * 8u));

#if DISPLAY_COMMON_CATHODE
    /* 先关闭全部位，避免换位时产生鬼影 */
    DIG_GPIO->BSRR = DIG_MASK;
    /* 段码：1 置位，0 复位；只操作 PB0~PB7，不能影响 PB12~PB15 的上拉 */
    SEG_GPIO->BSRR = (uint32_t)code |
                     ((uint32_t)(~code & 0xFFu) << 16);
    /* 再打开当前位：低电平有效 */
    DIG_GPIO->BRR  = (uint32_t)(1u << s_scan_index);
#else
    DIG_GPIO->BRR  = DIG_MASK;
    /* 共阳极：段码取反 */
    SEG_GPIO->BSRR = (uint32_t)(~code & 0xFFu) |
                     ((uint32_t)(code & 0xFFu) << 16);
    DIG_GPIO->BSRR = (uint32_t)(1u << s_scan_index);
#endif

    s_scan_index = (uint8_t)((s_scan_index + 1u) & 0x03u);
}
