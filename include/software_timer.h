/**
 * @file software_timer.h
 * @brief Public API for a simple, overflow-safe timer library for embedded systems
 * @author Richard Kubíček
 * @version 1.0.2
 *
 * This header exposes a small C API to manage software timers with external
 * clock source abstraction. The library provides basic timer functionality
 * for measuring time intervals and detecting timeout conditions in embedded
 * applications.
 *
 * Design highlights
 * - The timer uses unsigned 32-bit arithmetic which is overflow-safe, allowing
 *   the timer to work correctly even when the system clock wraps around.
 * - External clock source is provided via callback function pointer, making
 *   the library portable across different platforms.
 * - Two timer modes: continuous check (@ref SoftwareTimer_IsExpired) and
 *   one-shot evaluation (@ref SoftwareTimer_IsExpiredEvaluatedOnce).
 * - Minimal memory footprint: each timer instance uses only 12 bytes (on 32-bit systems).
 *
 * Key features
 * - One-shot timers that automatically deactivate after expiration
 * - External clock source abstraction via callback
 * - Overflow-safe arithmetic using unsigned integer operations
 * - Lightweight implementation suitable for resource-constrained systems
 *
 * Usage example:
 * @code
 * SoftwareTimer myTimer;
 * SoftwareTimer_Init(System_Millis);
 * SoftwareTimer_Set(&myTimer, 1000); // 1 second timeout
 *
 * // Later in main loop
 * if (SoftwareTimer_IsExpired(&myTimer)) {
 *     // Handle timeout
 * }
 *
 * // Or check remaining time
 * uint32_t remaining = SoftwareTimer_Remaining(&myTimer);
 * if (remaining > 0) {
 *     // Timer still running
 * }
 * @endcode
 *
 * @see software_timer.c for implementation details
 */

#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup software_timer_core Core API
 * @brief Core structures and functions for software timer management
 *
 * This module provides the main timer functionality including initialization,
 * timer configuration, expiration checking, and remaining time queries.
 * @{
 */

/**
 * @struct SoftwareTimer
 * @brief Timer state structure
 *
 * Contains all necessary state information for a single software timer instance.
 * The structure uses 32-bit unsigned integers for overflow-safe arithmetic.
 */
typedef struct {
    uint32_t start; /**< Start timestamp captured when timer was set using the clock source from @ref SoftwareTimer_Init */
    uint32_t interval; /**< Timer interval duration in clock ticks. Timer expires when (current_time - start) >= interval */
    bool evaluated; /**< One-shot evaluation flag. When true, @ref SoftwareTimer_IsExpiredEvaluatedOnce has already detected expiration */
} SoftwareTimer;

/**
 * @typedef SoftwareTimer_ClockTime
 * @brief Function pointer type for clock time provider callback
 *
 * This callback function should return a monotonically increasing clock value
 * in ticks. The tick unit (milliseconds, microseconds, etc.) is application-defined
 * but must be consistent across all timer operations.
 *
 * @return Current time value in ticks
 *
 * @note The clock function must never return decreasing values (except during overflow).
 * @note Clock overflow at 32-bit boundary (UINT32_MAX -> 0) is handled safely.
 *
 * Example implementation for milliseconds:
 * @code
 * uint32_t millis(void) {
 *     return HAL_GetTick(); // or equivalent platform function
 * }
 * @endcode
 */
typedef uint32_t (*SoftwareTimer_ClockTime)(void);

/**
 * @brief Initializes the software timer module with clock source
 *
 * This function must be called once during system initialization before using
 * any timer functionality. The provided clock source function will be used by
 * all timer operations to obtain the current time.
 *
 * @param[in] clock Function pointer to clock time provider. Must not be NULL.
 *                   The function should return monotonically increasing values.
 *
 * @pre clock must point to a valid function
 * @post All subsequent timer operations will use the provided clock source
 *
 * @note This function is NOT thread-safe. Call it only during initialization
 *       before any concurrent access occurs.
 *
 * @see SoftwareTimer_ClockTime
 * @see SoftwareTimer_Set
 *
 * Example:
 * @code
 * SoftwareTimer_Init(HAL_GetTick);
 * @endcode
 */
void SoftwareTimer_Init(SoftwareTimer_ClockTime clock);

/**
 * @brief Sets and starts a software timer with specified interval
 *
 * Captures the current time from the clock source and configures the timer
 * to expire after the specified interval. The timer becomes active immediately
 * and starts counting from the current time. Any previous timer state is
 * overwritten, including the evaluated flag.
 *
 * @param[in,out] timer Pointer to timer structure to configure. Must not be NULL.
 * @param[in] interval Timer interval in clock ticks. The unit depends on the
 *                     clock source provided to @ref SoftwareTimer_Init.
 *                     Value 0 means the timer expires immediately.
 *
 * @pre @ref SoftwareTimer_Init must have been called
 * @pre timer must point to a valid SoftwareTimer structure
 * @post timer->start is set to current clock time
 * @post timer->interval is set to the provided interval
 * @post timer->evaluated is reset to false
 *
 * @note Can be called at any time to restart/reconfigure an existing timer
 * @note Thread-safety depends on the clock source implementation
 *
 * @see SoftwareTimer_Init
 * @see SoftwareTimer_IsExpired
 * @see SoftwareTimer_Remaining
 *
 * Example:
 * @code
 * SoftwareTimer myTimer;
 * SoftwareTimer_Set(&myTimer, 1000); // Set for 1000 ticks
 * @endcode
 */
void SoftwareTimer_Set(SoftwareTimer * timer, uint32_t interval);

/**
 * @brief Checks if software timer has expired
 *
 * Tests whether the timer interval has elapsed since it was set. This function
 * can be called repeatedly and will continue to return true after the timer
 * has expired, until the timer is reset via @ref SoftwareTimer_Set.
 *
 * @param[in] timer Pointer to timer structure to check. Must not be NULL.
 *
 * @return true if the elapsed time >= interval
 * @return false if timer is still running
 *
 * @pre @ref SoftwareTimer_Init must have been called
 * @pre timer must have been initialized via @ref SoftwareTimer_Set
 *
 * @note This function uses overflow-safe unsigned arithmetic, so it works
 *       correctly even when the system clock wraps around at UINT32_MAX.
 * @note Unlike @ref SoftwareTimer_IsExpiredEvaluatedOnce, this function does
 *       not modify timer state and will continue returning true after expiration.
 *
 * @see SoftwareTimer_Set
 * @see SoftwareTimer_Remaining
 * @see SoftwareTimer_IsExpiredEvaluatedOnce
 *
 * Example:
 * @code
 * if (SoftwareTimer_IsExpired(&myTimer)) {
 *     // Handle timeout - will trigger every check after expiration
 * }
 * @endcode
 */
bool SoftwareTimer_IsExpired(SoftwareTimer * timer);

/**
 * @brief Returns remaining time until software timer expiration
 *
 * Calculates how many clock ticks remain until the timer expires by computing
 * (interval - elapsed_time). If the timer has already expired, returns 0.
 * This provides more granular information than @ref SoftwareTimer_IsExpired.
 *
 * @param[in] timer Pointer to timer structure to check. Must not be NULL.
 *
 * @return Number of clock ticks remaining until expiration
 * @retval 0 if timer has already expired
 *
 * @pre @ref SoftwareTimer_Init must have been called
 * @pre timer must have been initialized via @ref SoftwareTimer_Set
 *
 * @note This function uses overflow-safe unsigned arithmetic, so it works
 *       correctly even when the system clock wraps around at UINT32_MAX.
 * @note The returned value decreases over time until it reaches 0 at expiration.
 *
 * @see SoftwareTimer_Set
 * @see SoftwareTimer_IsExpired
 *
 * Example:
 * @code
 * uint32_t remaining = SoftwareTimer_Remaining(&myTimer);
 * if (remaining > 100) {
 *     // More than 100 ticks remaining
 * } else if (remaining > 0) {
 *     // Less than 100 ticks remaining
 * } else {
 *     // Timer has expired
 * }
 * @endcode
 */
uint32_t SoftwareTimer_Remaining(SoftwareTimer * timer);
/**
 * @brief Checks if software timer has expired (one-shot evaluation)
 *
 * Tests whether the timer interval has elapsed since it was set. Unlike
 * @ref SoftwareTimer_IsExpired, this function returns true only ONCE when
 * the timer expires. Subsequent calls return false until the timer is reset
 * via @ref SoftwareTimer_Set. This is useful for one-shot timer events.
 *
 * The function works by setting an internal evaluated flag when expiration
 * is detected. Once set, the function returns false on all subsequent calls.
 *
 * @param[in,out] timer Pointer to timer structure to check. Must not be NULL.
 *                      The evaluated flag may be modified by this function.
 *
 * @return true if timer just expired (first check after expiration)
 * @return false if timer is still running OR has already been evaluated
 *
 * @pre @ref SoftwareTimer_Init must have been called
 * @pre timer must have been initialized via @ref SoftwareTimer_Set
 * @post If return value is true, timer->evaluated is set to true
 *
 * @note This function uses overflow-safe unsigned arithmetic, so it works
 *       correctly even when the system clock wraps around at UINT32_MAX.
 * @note This function modifies timer state (evaluated flag), unlike
 *       @ref SoftwareTimer_IsExpired which is read-only.
 * @note To reset the one-shot behavior, call @ref SoftwareTimer_Set again.
 *
 * @see SoftwareTimer_Set
 * @see SoftwareTimer_IsExpired
 *
 * Example:
 * @code
 * // One-shot timer - triggers only once
 * if (SoftwareTimer_IsExpiredEvaluatedOnce(&myTimer)) {
 *     // This code runs exactly once when timer expires
 *     printf("Timer expired!\n");
 * }
 * // Subsequent calls return false until SoftwareTimer_Set() is called
 * @endcode
 */
bool SoftwareTimer_IsExpiredEvaluatedOnce(SoftwareTimer * timer);

/** @} */ // end of software_timer_core group

#ifdef __cplusplus
}
#endif

#endif // SOFTWARE_TIMER_H