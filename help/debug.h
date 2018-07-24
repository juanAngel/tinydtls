#ifndef DEBUG_H
#define DEBUG_H


#ifdef ENABLE_TRACE
# warning "ENABLE_TRACE enable"
# ifndef DEBUG_OUTPUT
#  define DEBUG_OUTPUT
# endif // ifndef DEBUG_OUTPUT
# define TRACE(...)                                        \
    do {                                                   \
        debug_printf("[T %s:%u]", __FUNCTION__, __LINE__); \
        debug_printf(__VA_ARGS__);                         \
        debug_printf("\n");                                \
    } while (0)
# define TRACE_DUMP(name, data, size)                                                    \
    debug_printf("[T %s:%u]%s", __FUNCTION__, __LINE__, name);                           \
    {                                                                                    \
        int i;                                                                           \
        for (i = 0; i < (size); i++) debug_printf(" %02X", ((unsigned char *)data)[i]);  \
    }                                                                                    \
    debug_printf("\n")
#else // ifdef ENABLE_TRACE
# define TRACE(...)
# define TRACE_DUMP(name, data, size)
#endif // ifdef ENABLE_TRACE
#define ERROR(...)                                         \
    do {                                                   \
        debug_printf("[E %s:%u]", __FUNCTION__, __LINE__); \
        debug_printf(__VA_ARGS__);                         \
        debug_printf("\n");                                \
    } while (0)

#ifdef DEBUG_OUTPUT

    # define debug_printf(...) printf("" __VA_ARGS__)
#else // ifdef DEBUG
    # define debug_printf(...)
#endif // ifdef DEBUG

#define EXP_BOOL(e)                                          \
    do {                                                     \
        debug_printf("%s = %s\n",# e, e ? "true" : "false"); \
    } while (0);

#endif // DEBUG_H
