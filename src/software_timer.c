/**
 * @file software_timer.c
 * @brief Software timer module implementation
 * @author Richard Kubíček
 * @version 1.0.2
 *
 * Implementation of simple timer functionality for embedded systems.
 * Uses external clock source for time measurement and provides
 * overflow-safe timer operations.
 *
 * This file contains the implementation of all functions declared in
 * software_timer.h. The module uses a single static clock source that
 * is shared across all timer instances.
 *
 * @see software_timer.h for API documentation
 */

#include "software_timer.h"
#include <stddef.h>

/**
 * @def SOFTWARETIMER_ASSERT
 * @brief Configurable assertion macro for parameter validation
 *
 * Users can define their own assertion handler by defining SOFTWARETIMER_ASSERT
 * before including this file. If not defined, the default behavior is:
 * - In debug builds (NDEBUG not defined): use standard assert()
 * - In release builds (NDEBUG defined): compile to empty statement
 *
 * Example of custom assertion:
 * @code
 * #define SOFTWARETIMER_ASSERT(expr) if(!(expr)) my_error_handler()
 * #include "software_timer.c"
 * @endcode
 */
#ifndef SOFTWARETIMER_ASSERT
    #ifdef NDEBUG
        #define SOFTWARETIMER_ASSERT(expr) ((void) 0)
    #else
        #include <assert.h>
        #define SOFTWARETIMER_ASSERT(expr) assert(expr)
    #endif
#endif

/**
 * @addtogroup software_timer_core
 * @{
 */

/**
 * @var clockTime
 * @brief Static clock time provider function pointer
 *
 * This static variable holds the clock source function pointer that was
 * provided via @ref SoftwareTimer_Init. All timer instances share this
 * single clock source.
 *
 * @note This variable is initialized by @ref SoftwareTimer_Init and must
 *       never be NULL during timer operations.
 * @note Sharing a single clock source across all timers ensures consistency
 *       and reduces memory footprint.
 */
static SoftwareTimer_ClockTime clockTime;

/**
 * Stores the clock function pointer for later use by timer operations.
 * This function must be called before any timer functionality is used.
 *
 * Implementation simply stores the provided clock function pointer in
 * the static clockTime variable. Includes defensive check for NULL pointer
 * in debug builds.
 */
void SoftwareTimer_Init(SoftwareTimer_ClockTime clock)
{
    SOFTWARETIMER_ASSERT(clock != NULL);
    clockTime = clock;
}

/**
 * Captures the current time as start reference and configures
 * the timer to expire after the specified interval. The timer
 * will be monitored by SoftwareTimer_IsExpired().
 *
 * Implementation details:
 * - Calls clockTime() to capture current timestamp
 * - Stores interval value
 * - Resets evaluated flag to false for one-shot mode
 * - Includes defensive checks in debug builds
 */
void SoftwareTimer_Set(SoftwareTimer * timer, uint32_t interval)
{
    SOFTWARETIMER_ASSERT(timer != NULL);
    SOFTWARETIMER_ASSERT(clockTime != NULL);
    timer->start = clockTime();
    timer->interval = interval;
    timer->evaluated = false;
}

/**
 * Compares elapsed time since timer start with the configured interval.
 * Uses unsigned arithmetic which is overflow-safe for 32-bit counters.
 *
 * Implementation uses the property of unsigned integer arithmetic where
 * (current - start) >= interval works correctly even during overflow.
 * Includes defensive checks in debug builds.
 */
bool SoftwareTimer_IsExpired(SoftwareTimer * timer)
{
    SOFTWARETIMER_ASSERT(timer != NULL);
    SOFTWARETIMER_ASSERT(clockTime != NULL);
    if (clockTime() - timer->start >= timer->interval) {
        return true;
    }
    return false;
}

/**
 * Calculates how many clock ticks remain until the timer expires.
 * Uses unsigned arithmetic which is overflow-safe for 32-bit counters.
 * If the timer has already expired, returns 0.
 *
 * Implementation:
 * - Calculates elapsed time using unsigned subtraction
 * - Returns 0 if elapsed >= interval (timer expired)
 * - Otherwise returns remaining time (interval - elapsed)
 * - Includes defensive checks in debug builds
 */
uint32_t SoftwareTimer_Remaining(SoftwareTimer * timer)
{
    SOFTWARETIMER_ASSERT(timer != NULL);
    SOFTWARETIMER_ASSERT(clockTime != NULL);
    uint32_t elapsed = clockTime() - timer->start;

    if (elapsed >= timer->interval) {
        return 0; // Timer has expired
    }

    return timer->interval - elapsed;
}

/**
 * Compares elapsed time since timer start with the configured interval.
 * Uses unsigned arithmetic which is overflow-safe for 32-bit counters.
 * When timer expires, it is automatically deactivated to prevent
 * multiple expiration notifications.
 *
 * Implementation:
 * - First checks if already evaluated (one-shot flag)
 * - If not evaluated, checks if timer expired
 * - If expired, sets evaluated flag and returns true
 * - Subsequent calls return false until timer is reset via SoftwareTimer_Set
 * - Includes defensive checks in debug builds
 */
bool SoftwareTimer_IsExpiredEvaluatedOnce(SoftwareTimer * timer)
{
    SOFTWARETIMER_ASSERT(timer != NULL);
    SOFTWARETIMER_ASSERT(clockTime != NULL);
    if (timer->evaluated)
        return false;

    if (clockTime() - timer->start >= timer->interval) {
        timer->evaluated = true;
        return true;
    }
    return false;
}

/** @} */