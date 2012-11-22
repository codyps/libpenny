#ifndef PENNY_PENNY_H_
#define PENNY_PENNY_H_

#include <stddef.h>

typedef unsigned long long ull;

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))
#define FIELD_SIZE(s,f) (sizeof((s *)0->f))
#define typeof_field(s, f) typeof((s *)0->f)

/* offsetof is defined in stddef.h */
#if 1
/* works in gdb. */
#define container_of(item, type, member) \
		((type *)((char *)(item) - offsetof(type, member)))
#else
/* causes some extra checking */
#define container_of(item, type, member) ({				\
		typeof_field(type, member) *__container_of__item = item; \
		((type *)((char *)(__container_of__item)		\
			- offsetof(type, member)));			\
		})
#endif

#define ACCESS_ONCE(x)  (*(volatile typeof(x) *) &(x))
#define barrier() __asm__ __volatile__ ("":::"memory")

/* prefetch for reading */
#define prefetch(x)  __builtin_prefetch(x)

/* prefetch for writing */
#define prefetchw(x) __builtin_prefetch(x,1)

#define _STR(x) #x
#define STR(x) _STR(x)
#define LINE_STR STR(__LINE__)

/* to use, call the defined function is a condition that should never be true.
 * If the call is optimized out, no warning will be omitted.
 * Otherwise, it will be unhappy with you & create a nasty multiline
 * warning/error.
 *
 * Notes: marking this as noinline is required. So is the barrier. My guess is
 *        that the "inlining" discards the warning message.
 *
 * Warning: The use of 'barrier()' will (potentially) create less efficient code
 *        if the warning is emitted (not a concern for the error, as code won't
 *        be emitted in that case).
 */

#ifndef CONFIG_COMPILETIME_LONG_MSG
#define DEFINE_COMPILETIME_WARNING(name, msg) __attribute__((warning(msg),noinline)) static void name(void) { barrier(); }
#define DEFINE_COMPILETIME_ERROR(name, msg) __attribute__((error(msg),noinline)) static void name(void) { barrier(); }
#else
#define DEFINE_COMPILETIME_WARNING(name, msg) __attribute__((warning("\n"__FILE__ ":" LINE_STR ": " msg),noinline)) static void name(void) { barrier(); }
#define DEFINE_COMPILETIME_ERROR(name, msg) __attribute__((error("\n" __FILE__ ":" LINE_STR ": " msg),noinline)) static void name(void) { barrier(); }
#endif

/* useful in combination with the compiletime errors/warnings above. */
#define is_constant(expr)      __builtin_constant_p(expr)
#define constant_or_zero(expr) __builtin_choose_expr(__builtin_constant_p(expr), expr, 0)

#ifndef htonll
# ifdef _BIG_ENDIAN
#  define htonll(x)   (x)
#  define ntohll(x)   (x)
# else
#  define htonll(x)   ((((uint64_t)htonl(x)) << 32) + htonl(x >> 32))
#  define ntohll(x)   ((((uint64_t)ntohl(x)) << 32) + ntohl(x >> 32))
# endif
#endif

#define EXPORT(sym) __attribute__((externally_visible)) sym
#define expect_eq(x, y) __builtin_expect(x, y)
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(x,0)
#define must_check  __attribute__((warn_unused_result))
#define unused      __attribute__((unused))
#define noreturn    __attribute__((__noreturn__))

#define CAT2(a, b) a##b
#define CAT3(a, b, c) a##b##c

#endif
