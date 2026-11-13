#include "out.h"

#ifdef _WIN32
#include <windows.h>
#define VALID_OSFD(fd) ((fd) != INVALID_HANDLE_VALUE)
#else
#include <unistd.h>
#define VALID_OSFD(fd) ((fd) >= 0)
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void appendf(struct buf *b, unsigned char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    appendv(b, fmt, args);
    va_end(args);
}

void appendv(struct buf *b, unsigned char *fmt, va_list args) {
    unsigned char buf[4096];

    int n = vsnprintf((char *)buf, sizeof(buf), (char *)fmt, args);

    assert((unsigned int)n < sizeof(buf));
    append(b, buf, n);
}

static bool os_write(OSFD fd, void *buf, int n) {
    if (n < 0) return false;
    if (n == 0) return true;

#ifdef _WIN32
    DWORD sz      = (DWORD)n;
    DWORD written = 0;
    return WriteFile(fd, buf, sz, &written, NULL);
#else
    size_t sz = (size_t)n;
    return write(fd, buf, sz) > 0;
#endif
}

void flush(struct buf *b) {
    b->error |= !VALID_OSFD(b->fd);
    if (!b->error && b->len) {
        b->error |= !os_write(b->fd, b->buf, b->len);
        b->len = 0;
    }
}

void append(struct buf *b, unsigned char *src, int len) {
    unsigned char *end = src + len;
    while (!b->error && src < end) {
        int left   = end - src;
        int avail  = b->cap - b->len;
        int amount = avail < left ? avail : left;

        for (int i = 0; i < amount; i++) {
            b->buf[b->len + i] = src[i];
        }
        b->len += amount;
        src += amount;

        if (amount < left) {
            flush(b);
        }
    }
}

void append_byte(struct buf *b, unsigned char uc) {
    append(b, &uc, 1); //
}

void line_break(struct buf *b) {
    append_byte(b, '\n'); //
}

void append_f32(struct buf *b, float f) {
    append_f32_prec(b, f, 2); //
}

void append_f32_prec(struct buf *b, float f, int precision) {
    char str[64];
    int n = snprintf(str, sizeof(str), "%.*f", precision, f);
    assert((unsigned)n < sizeof(str));
    append(b, (unsigned char *)str, n);
}
