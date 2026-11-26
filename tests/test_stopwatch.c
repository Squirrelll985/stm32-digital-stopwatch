/**
 * @file    test_stopwatch.c
 * @brief   秒表核心逻辑的主机侧单元测试
 *
 * 编译（在工程根目录）：
 *   gcc -std=c11 -Wall -Wextra -Itests/stub -Iinclude \
 *       tests/test_stopwatch.c src/stopwatch.c -o build/test_stopwatch
 */
#include <stdio.h>
#include <stdint.h>
#include "stopwatch.h"

static int g_fail;

#define CHECK(cond) do {                                                    \
    if (!(cond)) {                                                          \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);              \
        ++g_fail;                                                           \
    }                                                                       \
} while (0)

static void tick_ms(unsigned n)
{
    unsigned i;

    for (i = 0; i < n; ++i) {
        stopwatch_tick_10ms();
    }
}

int main(void)
{
    stopwatch_init();
    CHECK(stopwatch_value() == 0u);
    CHECK(stopwatch_is_running() == 0u);
    CHECK(stopwatch_record_count() == 0u);

    /* 启动后 1.00 s */
    stopwatch_toggle();
    CHECK(stopwatch_is_running() == 1u);
    tick_ms(100u);
    CHECK(stopwatch_value() == 100u);

    /* 停止后计时值保持不变 */
    stopwatch_toggle();
    CHECK(stopwatch_is_running() == 0u);
    tick_ms(50u);
    CHECK(stopwatch_value() == 100u);

    /* 第 1 组计次 */
    stopwatch_record();
    CHECK(stopwatch_record_count() == 1u);
    CHECK(stopwatch_record_value(0u) == 100u);

    /* 继续计时到 1.50 s，记录第 2 组 */
    stopwatch_toggle();
    tick_ms(50u);
    CHECK(stopwatch_value() == 150u);
    stopwatch_record();
    CHECK(stopwatch_record_count() == 2u);
    CHECK(stopwatch_record_value(1u) == 150u);

    /* 继续计时到 2.00 s，记录第 3 组 */
    tick_ms(50u);
    stopwatch_record();
    CHECK(stopwatch_record_count() == 3u);
    CHECK(stopwatch_record_value(2u) == 200u);

    /* 第 4 次记录应被忽略 */
    stopwatch_record();
    CHECK(stopwatch_record_count() == 3u);

    /* 翻阅：1 -> 2 -> 3 -> 实时 */
    stopwatch_view_next();
    CHECK(stopwatch_view_index() == 0u);
    stopwatch_view_next();
    CHECK(stopwatch_view_index() == 1u);
    stopwatch_view_next();
    CHECK(stopwatch_view_index() == 2u);
    stopwatch_view_next();
    CHECK(stopwatch_view_index() == STOPWATCH_VIEW_NONE);

    /* 清零 */
    stopwatch_clear();
    CHECK(stopwatch_value() == 0u);
    CHECK(stopwatch_is_running() == 0u);
    CHECK(stopwatch_record_count() == 0u);
    CHECK(stopwatch_view_index() == STOPWATCH_VIEW_NONE);

    /* 上限 9:59.99 自动停止 */
    stopwatch_toggle();
    tick_ms(60000u);
    CHECK(stopwatch_value() == STOPWATCH_MAX_CS);
    CHECK(stopwatch_is_running() == 0u);
    CHECK(stopwatch_is_overflow() == 1u);

    /* 到上限后再按 K1：从 0 重新启动 */
    stopwatch_toggle();
    CHECK(stopwatch_is_running() == 1u);
    CHECK(stopwatch_is_overflow() == 0u);
    CHECK(stopwatch_value() == 0u);
    tick_ms(10u);
    CHECK(stopwatch_value() == 10u);

    /* 运行中清零 */
    stopwatch_clear();
    CHECK(stopwatch_value() == 0u);
    CHECK(stopwatch_is_running() == 0u);

    if (g_fail == 0) {
        printf("ALL TESTS PASSED\n");
        return 0;
    }
    printf("%d TEST(S) FAILED\n", g_fail);
    return 1;
}
