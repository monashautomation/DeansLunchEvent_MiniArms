#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef unsigned long ulong;
#define HIGH 1
#define LOW  0

static inline void Serial_println(const char *s) { (void)s; }
static inline void Serial_print(const char *s)   { (void)s; }
