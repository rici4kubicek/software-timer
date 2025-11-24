/**
 * @file software_timer_config_template.h
 * @brief Configuration template for SoftwareTimer library
 *
 * This file provides examples of how to configure the SoftwareTimer library
 * for your specific needs. Copy this file to your project and customize it.
 *
 * To use custom configuration:
 * 1. Copy this file to your project as software_timer_config.h
 * 2. Uncomment and customize the desired options
 * 3. Include your config file before including software_timer.h or software_timer.c
 */

#ifndef SOFTWARE_TIMER_CONFIG_H
#define SOFTWARE_TIMER_CONFIG_H

/* ============================================================================
 * Custom Assertion Handler Configuration
 * ============================================================================
 * By default, the library uses standard assert() in debug builds and compiles
 * assertions out in release builds. You can override this behavior.
 */

/* Example 1: Custom assertion with error handler
 * Uncomment to use a custom error handler instead of assert()
 */
/*
#define SOFTWARETIMER_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            error_handler(__FILE__, __LINE__, #expr); \
        } \
    } while(0)
*/

/* Example 2: Always-on assertions (even in release builds)
 * Uncomment to keep assertions active in release builds
 */
/*
#include <assert.h>
#define SOFTWARETIMER_ASSERT(expr) assert(expr)
*/

/* Example 3: Custom embedded system error handling
 * Uncomment for embedded systems with custom error reporting
 */
/*
#define SOFTWARETIMER_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            system_fault_handler(); \
            while(1); \
        } \
    } while(0)
*/

/* Example 4: Disable all assertions (not recommended)
 * Uncomment to completely disable parameter validation
 */
/*
#define SOFTWARETIMER_ASSERT(expr) ((void)0)
*/

/* Example 5: Log-only assertions
 * Uncomment to log assertion failures without halting execution
 */
/*
#define SOFTWARETIMER_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            log_error("Assertion failed: %s at %s:%d", #expr, __FILE__, __LINE__); \
        } \
    } while(0)
*/

#endif /* SOFTWARE_TIMER_CONFIG_H */
