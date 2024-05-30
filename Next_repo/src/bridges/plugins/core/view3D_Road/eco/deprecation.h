// COMPANY:   Continental Automotive
// COMPONENT: Eco
#ifndef ECO_DEPRECATION_H_
#define ECO_DEPRECATION_H_

#if defined(__GNUC__) || defined(__clang__)
#if __GNUC__ > 5 || ((__GNUC__ == 5) && (__GNUC_MINOR__ >= 4))
#define DEPRECATED_BEGIN __attribute__((deprecated))
#define DEPRECATED_END
#else
#define DEPRECATED_BEGIN
#define DEPRECATED_END __attribute__((deprecated))
#endif
#elif defined(_MSC_VER)
#define DEPRECATED_BEGIN __declspec(deprecated)
#define DEPRECATED_END
#else
#define DEPRECATED_BEGIN
#define DEPRECATED_END
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#endif

#define DEPRECATED(SYMBOL) DEPRECATED_BEGIN SYMBOL DEPRECATED_END

#endif // ECO_DEPRECATION_H_
