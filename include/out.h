#pragma once
#ifndef KXD_OUT_H_
#define KXD_OUT_H_

#include <stdarg.h>

#ifdef _WIN32
typedef void* OSFD;
#else
typedef int OSFD;
#endif

#define MEMBUF(buf, cap) {buf, cap, 0, -1, 0}
#define FDBUF(fd, buf, cap) {buf, cap, 0, fd, 0}

struct buf {
    unsigned char *buf;
    int cap;
    int len;
    OSFD fd;
    _Bool error;
};

#define APPEND_STR(b, s) append(b, (unsigned char *)(s), sizeof(s)-1)

void append(struct buf *b, unsigned char *src, int len);
void appendf(struct buf *b, unsigned char *fmt, ...);
void appendv(struct buf *b, unsigned char *fmt, va_list args);
void append_byte(struct buf *b, unsigned char uc);
void flush(struct buf *b);

#endif // KXD_OUT_H_
