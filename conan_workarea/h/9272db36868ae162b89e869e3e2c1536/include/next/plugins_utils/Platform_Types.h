#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#ifdef WIN32
typedef unsigned char boolean;     /* [PLATFORM026],[PLATFORM027] */
typedef signed char sint8;         /* [PLATFORM016] */
typedef unsigned char uint8;       /* [PLATFORM013] */
typedef signed short sint16;       /* [PLATFORM017] */
typedef unsigned short uint16;     /* [PLATFORM014] */
typedef signed long sint32;        /* [PLATFORM018] */
typedef unsigned long uint32;      /* [PLATFORM015] */
typedef unsigned int uint8_least;  /* [PLATFORM020],[PLATFORM005] */
typedef unsigned int uint16_least; /* [PLATFORM021],[PLATFORM005] */
typedef unsigned int uint32_least; /* [PLATFORM022],[PLATFORM005] */
typedef signed int sint8_least;    /* [PLATFORM023],[PLATFORM005] */
typedef signed int sint16_least;   /* [PLATFORM024],[PLATFORM005] */
typedef signed int sint32_least;   /* [PLATFORM025],[PLATFORM005] */
typedef unsigned long long uint64; /* 0 .. 18446744073709551615   */
typedef float float32;             /* [PLATFORM041] */
typedef double float64;            /* [PLATFORM042] */
typedef signed long long sint64;

#else

#include <stdint.h>
typedef uint8_t boolean; /* [PLATFORM026],[PLATFORM027] */
typedef int8_t sint8;    /* [PLATFORM016] */
typedef int16_t sint16;  /* [PLATFORM017] */
typedef int32_t sint32;  /* [PLATFORM018] */
typedef int64_t sint64;
typedef uint8_t uint8;               /* [PLATFORM013] */
typedef uint16_t uint16;             /* [PLATFORM014] */
typedef uint32_t uint32;             /* [PLATFORM015] */
typedef uint64_t uint64;             /* 0 .. 18446744073709551615   */
typedef uint_least8_t uint8_least;   /* [PLATFORM020],[PLATFORM005] */
typedef uint_least16_t uint16_least; /* [PLATFORM021],[PLATFORM005] */
typedef uint_least32_t uint32_least; /* [PLATFORM022],[PLATFORM005] */
typedef uint_least64_t uint64_least; /* [PLATFORM022],[PLATFORM005] */
typedef int_least8_t sint8_least;    /* [PLATFORM023],[PLATFORM005] */
typedef int_least16_t sint16_least;  /* [PLATFORM024],[PLATFORM005] */
typedef int_least32_t sint32_least;  /* [PLATFORM025],[PLATFORM005] */
typedef int_least64_t sint64_least;
typedef float float32;  /* [PLATFORM041] */
typedef double float64; /* [PLATFORM042] */

#endif

#ifndef TRUE
#define TRUE ((boolean)1)
#endif
#ifndef FALSE
#define FALSE ((boolean)0)
#endif

#endif /* #ifndef PLATFORM_TYPES_H */
