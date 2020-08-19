#include <time.h>
#include <stdlib.h>

/**
 * \brief Converts a struct tm into a human-readable string.
 *
 * The format followed is YYYYMMDD-HH:MM:SS, 17 characters
 */
#ifdef __cplusplus
extern "C" {
#endif

#define TIMESTAMP_LEN 17
#define TIMESTAMP_LEN_WITH_MILLIS 21
#define TIMESTAMP_LEN_WITH_MICROS 24

    int tm_to_str(const struct tm *tm, char *buf, size_t len);

    int tm_to_str_millis(const struct tm *tm, unsigned int millis,
                         char *buf, size_t len);

    int tm_to_str_micros(const struct tm *tm, unsigned int micros,
                         char *buf, size_t len);


#ifdef __cplusplus
}
#endif
