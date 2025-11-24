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
 * the static clockTime variable.
 */
void SoftwareTimer_Init(SoftwareTimer_ClockTime clock)
{
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
 */
void SoftwareTimer_Set(SoftwareTimer * timer, uint32_t interval)
{
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
 */
bool SoftwareTimer_IsExpired(SoftwareTimer * timer)
{
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
 */
uint32_t SoftwareTimer_Remaining(SoftwareTimer * timer)
{
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
 */
bool SoftwareTimer_IsExpiredEvaluatedOnce(SoftwareTimer * timer)
{
    if (timer->evaluated)
        return false;

    if (clockTime() - timer->start >= timer->interval) {
        timer->evaluated = true;
        return true;
    }
    return false;
}

/** @} */