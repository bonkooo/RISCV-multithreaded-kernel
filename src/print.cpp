//
// Created by marko on 20.4.22..
//

#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
//#include "../lib/console.h"

void _printString(char const *string)
{
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
}

void _printInteger(uint64 integer)
{
    static char digits[] = "0123456789";
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if (integer < 0)
    {
        neg = 1;
        x = -integer;
    } else
    {
        x = integer;
    }

    i = 0;
    do
    {
        buf[i++] = digits[x % 10];
    } while ((x /= 10) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0) { putc(buf[i]); }
}


void printHex(uint64 value) {
    static const char digits[] = "0123456789ABCDEF";
    char buf[2 + 16 + 1]; // "0x" + 16 hex digits + '\0'
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 16; i++) {
        int shift = (15 - i) * 4;
        buf[2 + i] = digits[(value >> shift) & 0xF];
    }
    buf[18] = '\0';
    _printString(buf);
}

void newLine(){
    putc('\n');
}
