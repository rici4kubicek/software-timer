/**
 * @file timer.c
 * @brief Timer module implementation
 *
 * Implementation of simple timer functionality for embedded systems.
 * Uses external clock source for time measurement and provides
 * overflow-safe timer operations.
 */

#include "timer.h"

/** Static clock time provider function pointer */
static Timer_ClockTime clockTime;

/**
 * @brief Initializes the timer module with clock source
 *
 * Stores the clock function pointer for later use by timer operations.
 * This function must be called before any timer functionality is used.
 *
 * @param clock Function pointer to system clock provider
 */
void Timer_Init(Timer_ClockTime clock)
{
    clockTime = clock;
}

/**
 * @brief Sets and activates a timer with specified interval
 *
 * Captures the current time as start reference and configures
 * the timer to expire after the specified interval. The timer
 * will be monitored by Timer_IsExpired().
 *
 * @param timer Pointer to timer structure to configure
 * @param interval Timeout interval in clock ticks
 */
void Timer_Set(Timer * timer, uint32_t interval)
{
    timer->start = clockTime();
    timer->interval = interval;
}

/**
 * @brief Checks if timer has expired and deactivates it
 *
 * Compares elapsed time since timer start with the configured interval.
 * Uses unsigned arithmetic which is overflow-safe for 32-bit counters.
 * When timer expires, it is automatically deactivated to prevent
 * multiple expiration notifications.
 *
 * @param timer Pointer to timer structure to check
 * @return true if timer was active and has expired, false otherwise
 */
bool Timer_IsExpired(Timer * timer)
{
    if (clockTime() - timer->start >= timer->interval) {
        return true;
    }
    return false;
}

/**
 * @brief Returns remaining time until timer expiration
 *
 * Calculates how many clock ticks remain until the timer expires.
 * Uses unsigned arithmetic which is overflow-safe for 32-bit counters.
 * If the timer has already expired, returns 0.
 *
 * @param timer Pointer to timer structure to check
 * @return Number of clock ticks remaining, or 0 if expired
 */
uint32_t Timer_Remaining(Timer * timer)
{
    uint32_t elapsed = clockTime() - timer->start;

    if (elapsed >= timer->interval) {
        return 0; // Timer has expired
    }

    return timer->interval - elapsed;
}