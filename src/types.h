#ifndef TYPES_H
#define TYPES_H

/*
 * types.h — Fixed-Width Type Aliases
 * ─────────────────────────────────────────────────────────────────
 * Mirrors the convention used in embedded C (MISRA, STM32 HAL).
 * Every variable should use the smallest type that fits its range.
 */

#include <stdint.h>

typedef uint8_t   u8;    /* unsigned  8-bit :  0  to  255           */
typedef uint16_t  u16;   /* unsigned 16-bit :  0  to  65 535        */
typedef uint32_t  u32;   /* unsigned 32-bit :  0  to  4 294 967 295 */
typedef int8_t    i8;    /* signed    8-bit : -128 to  127          */
typedef int16_t   i16;   /* signed   16-bit : -32 768 to 32 767     */
typedef int32_t   i32;   /* signed   32-bit : standard loop/index   */
typedef float     f32;   /* 32-bit single-precision float            */

#endif /* TYPES_H */
