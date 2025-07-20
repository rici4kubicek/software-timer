/**
 * @file timer.h
 * @brief Simple timer module for embedded systems
 *
 * This module provides basic timer functionality for measuring time intervals
 * and detecting timeout conditions in embedded applications.
 *
 * Features:
 * - One-shot timers that automatically deactivate after expiration
 * - External clock source abstraction via callback
 * - Overflow-safe arithmetic using unsigned integer operations
 * - Lightweight implementation suitable for resource-constrained systems
 *
 * Usage example:
 * @code
 * Timer myTimer;
 * Timer_Init(System_Millis);
 * Timer_Set(&myTimer, 1000); // 1 second timeout
 *
 * // Later in main loop
 * if (Timer_IsExpired(&myTimer)) {
 *     // Handle timeout
 * }
 *
 * // Or check remaining time
 * uint32_t remaining = Timer_Remaining(&myTimer);
 * if (remaining > 0) {
 *     // Timer still running
 * }
 * @endcode
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Timer structure containing timer state
 */
typedef struct {
    uint32_t start; /**< Start timestamp when timer was set */
    uint32_t interval; /**< Timer interval in clock ticks */
    bool evaluated;  /**< Timer flag which signalize complete evaluation */
} Timer;

/**
 * @brief Function pointer type for clock time provider
 * @return Current time in system ticks (typically milliseconds)
 */
typedef uint32_t (*Timer_ClockTime)(void);

/**
 * @brief Initializes the timer module with clock source
 *
 * This function must be called before using any timer functionality.
 * The clock source function will be called to obtain current time.
 *
 * @param clock Function pointer to clock time provider
 */
void Timer_Init(Timer_ClockTime clock);

/**
 * @brief Sets and starts a timer with specified interval
 *
 * Configures the timer to expire after the specified interval.
 * The timer becomes active and starts counting from current time.
 *
 * @param timer Pointer to timer structure
 * @param interval Timer interval in clock ticks
 */
void Timer_Set(Timer * timer, uint32_t interval);

/**
 * @brief Checks if timer has expired
 *
 * Tests whether the timer interval has elapsed since it was set.
 *
 * @param timer Pointer to timer structure
 * @return true if timer has expired, false otherwise
 *
 * @note This function uses overflow-safe arithmetic, so it works
 *       correctly even when the system clock wraps around.
 */
bool Timer_IsExpired(Timer * timer);

/**
 * @brief Returns remaining time until timer expiration
 *
 * Calculates how many clock ticks remain until the timer expires.
 * If the timer has already expired, returns 0.
 *
 * @param timer Pointer to timer structure
 * @return Number of clock ticks remaining, or 0 if expired
 *
 * @note This function uses overflow-safe arithmetic, so it works
 *       correctly even when the system clock wraps around.
 */
uint32_t Timer_Remaining(Timer * timer);
/**
 * @brief Checks if timer has expired
 *
 * Tests whether the timer interval has elapsed since it was set.
 * If the timer has expired, it is automatically deactivated.
 * Inactive timers always return false.
 *
 * @param timer Pointer to timer structure
 * @return true if timer has expired, false otherwise
 *
 * @note This function uses overflow-safe arithmetic, so it works
 *       correctly even when the system clock wraps around.
 */
bool Timer_IsExpiredEvaluatedOnce(Timer * timer);

#ifdef __cplusplus
}
#endif

#endif // TIMER_H