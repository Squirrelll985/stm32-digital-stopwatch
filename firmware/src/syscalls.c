/**
 * @file    syscalls.c
 * @brief   裸机环境下的 newlib 最小系统调用桩
 *
 * 本工程不使用 printf / malloc，保留这些桩函数是为了让链接器
 * 完整解析 libc_nano，避免出现 "_write is not implemented" 警告。
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

extern char _end;
extern char _estack;

static char *s_heap_end;

void *_sbrk(ptrdiff_t incr)
{
    char *prev;
    char *next;

    if (s_heap_end == NULL) {
        s_heap_end = &_end;
    }

    prev = s_heap_end;
    next = prev + incr;

    /* 栈从 _estack 向下生长，堆与栈之间保留 1 KB 安全区 */
    if ((uintptr_t)next > ((uintptr_t)&_estack - 1024u)) {
        errno = ENOMEM;
        return (void *)-1;
    }

    s_heap_end = next;
    return prev;
}

int _write(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    return len;
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

void _exit(int status)
{
    (void)status;
    for (;;) {
        __asm volatile ("nop");
    }
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void)
{
    return 1;
}
