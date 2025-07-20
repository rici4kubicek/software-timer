/**
 * @file test_timer.c
 * @brief Unit tests for timer module using Unity framework
 *
 * Comprehensive test suite for timer functionality including:
 * - Basic timer operations
 * - Edge cases and boundary conditions
 * - Clock overflow scenarios
 * - Multiple timer instances
 * - Remaining time calculations
 */

#include "unity.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "timer.h"

/* Test fixture data */
static uint32_t mock_time = 0;

/**
 * @brief Mock clock function for testing
 * @return Current mock time value
 */
static uint32_t mock_clock_time(void)
{
    return mock_time;
}

/**
 * @brief Advances mock time by specified amount
 * @param ticks Number of ticks to advance
 */
static void advance_time(uint32_t ticks)
{
    mock_time += ticks;
}

/**
 * @brief Reset mock time and initialize timer module
 */
static void reset_timer_system(void)
{
    mock_time = 0;
    Timer_Init(mock_clock_time);
}

void test_Timer_Init_Basic(void)
{
    Timer timer;
    memset(&timer, 0xFF, sizeof(timer)); // Fill with garbage

    // Timer should be inactive initially (not set)
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));
}

void test_Timer_Set_BasicOperation(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 100);

    // Timer should be active but not expired immediately
    TEST_ASSERT_EQUAL(0, timer.start);
    TEST_ASSERT_EQUAL(100, timer.interval);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));
}

void test_Timer_IsExpired_BasicFlow(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 100);

    // Should not expire before interval
    advance_time(50);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Should expire exactly at interval
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_Remaining_BasicFlow(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 100);

    // Initially should have full interval remaining
    TEST_ASSERT_EQUAL(100, Timer_Remaining(&timer));

    // After 25 ticks, should have 75 remaining
    advance_time(25);
    TEST_ASSERT_EQUAL(75, Timer_Remaining(&timer));

    // After 50 ticks total, should have 50 remaining
    advance_time(25);
    TEST_ASSERT_EQUAL(50, Timer_Remaining(&timer));

    // After 100 ticks total, should have 0 remaining (expired)
    advance_time(50);
    TEST_ASSERT_EQUAL(0, Timer_Remaining(&timer));

    // After expiration, should still return 0
    advance_time(50);
    TEST_ASSERT_EQUAL(0, Timer_Remaining(&timer));
}

void test_Timer_Remaining_ZeroInterval(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 0);

    // Should return 0 immediately with zero interval
    TEST_ASSERT_EQUAL(0, Timer_Remaining(&timer));
}

void test_Timer_Remaining_ClockOverflow(void)
{
    Timer timer;
    reset_timer_system();

    // Set time near overflow
    mock_time = UINT32_MAX - 50;
    Timer_Set(&timer, 100);

    // Should have full interval remaining initially
    TEST_ASSERT_EQUAL(100, Timer_Remaining(&timer));

    // Advance to overflow point
    advance_time(75); // This will overflow mock_time to 24
    TEST_ASSERT_EQUAL(25, Timer_Remaining(&timer));

    // Should expire after total interval (25 more ticks)
    advance_time(25);
    TEST_ASSERT_EQUAL(0, Timer_Remaining(&timer));
}

void test_Timer_Remaining_MaxInterval(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, UINT32_MAX);

    // Should have max interval remaining
    TEST_ASSERT_EQUAL(UINT32_MAX, Timer_Remaining(&timer));

    // Should not expire even after large time advance
    advance_time(UINT32_MAX - 1);
    TEST_ASSERT_EQUAL(1, Timer_Remaining(&timer));

    // Should expire after exactly max interval
    advance_time(1);
    TEST_ASSERT_EQUAL(0, Timer_Remaining(&timer));
}

void test_Timer_Remaining_ConsistencyWithIsExpired(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 1000);

    // Test at various points
    for (uint32_t i = 0; i < 1000; i += 100) {
        advance_time(100);

        bool expired = Timer_IsExpired(&timer);
        uint32_t remaining = Timer_Remaining(&timer);

        if (expired) {
            TEST_ASSERT_EQUAL(0, remaining);
        } else {
            TEST_ASSERT_GREATER_THAN(0, remaining);
        }
    }

    // Final check - should be expired
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
    TEST_ASSERT_EQUAL(0, Timer_Remaining(&timer));
}

void test_Timer_ZeroInterval(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 0);

    // Should expire immediately with zero interval
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_ClockOverflow(void)
{
    Timer timer;
    reset_timer_system();

    // Set time near overflow
    mock_time = UINT32_MAX - 50;
    Timer_Set(&timer, 100);

    // Should not expire before overflow
    advance_time(75); // This will overflow mock_time to 24
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Should expire after total interval (25 more ticks)
    advance_time(25);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_MultipleTimers(void)
{
    Timer timer1, timer2, timer3;
    reset_timer_system();

    Timer_Set(&timer1, 50);
    Timer_Set(&timer2, 100);
    Timer_Set(&timer3, 150);

    // No timers should expire initially
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer1));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer2));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer3));

    // First timer expires
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer1));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer2));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer3));

    // Second timer expires
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer1));
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer2));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer3));

    // Third timer expires
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer1));
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer2));
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer3));
}

void test_Timer_Reset(void)
{
    Timer timer;
    reset_timer_system();

    // Set initial timer
    Timer_Set(&timer, 100);
    advance_time(50);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Reset timer with new interval
    Timer_Set(&timer, 200);
    advance_time(50); // Total 100 from original start, but timer was reset
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Should expire after new interval from reset time
    advance_time(150);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_MaxInterval(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, UINT32_MAX);

    // Should not expire even after large time advance
    advance_time(UINT32_MAX - 1);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Should expire after exactly max interval
    advance_time(1);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_BoundaryConditions(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 1000);

    // Should not expire 1 tick before
    advance_time(999);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Should expire exactly at boundary
    advance_time(1);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_RepeatedCalls(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 100);

    // Multiple calls before expiration should return false
    advance_time(50);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Expire timer
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));

    // Multiple calls after expiration
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_LargeInterval(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 0x80000000UL); // Large interval

    advance_time(0x7FFFFFFFUL);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    advance_time(1);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_ConsecutiveOperations(void)
{
    Timer timer;
    reset_timer_system();

    // First operation
    Timer_Set(&timer, 50);
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));

    // Second operation with same timer
    advance_time(25); // Now at time 75
    Timer_Set(&timer, 100);
    advance_time(100);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));

    // Third operation
    Timer_Set(&timer, 200);
    advance_time(199);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));
    advance_time(1);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void test_Timer_StateConsistency(void)
{
    Timer timer;
    reset_timer_system();

    Timer_Set(&timer, 100);

    // Verify initial state
    TEST_ASSERT_EQUAL(0, timer.start);
    TEST_ASSERT_EQUAL(100, timer.interval);

    // Advance time but don't expire
    advance_time(50);
    TEST_ASSERT_FALSE(Timer_IsExpired(&timer));

    // Expire and check state change
    advance_time(50);
    TEST_ASSERT_TRUE(Timer_IsExpired(&timer));
}

void setUp(void)
{
    // Reset test environment before each test
    reset_timer_system();
}

void tearDown(void)
{
    // Clean up after each test
    mock_time = 0;
}

int runUnityTests(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_Timer_Init_Basic);
    RUN_TEST(test_Timer_Set_BasicOperation);
    RUN_TEST(test_Timer_IsExpired_BasicFlow);
    RUN_TEST(test_Timer_Remaining_BasicFlow);
    RUN_TEST(test_Timer_Remaining_ZeroInterval);
    RUN_TEST(test_Timer_Remaining_ClockOverflow);
    RUN_TEST(test_Timer_Remaining_MaxInterval);
    RUN_TEST(test_Timer_Remaining_ConsistencyWithIsExpired);
    RUN_TEST(test_Timer_ZeroInterval);
    RUN_TEST(test_Timer_ClockOverflow);
    RUN_TEST(test_Timer_MultipleTimers);
    RUN_TEST(test_Timer_Reset);
    RUN_TEST(test_Timer_MaxInterval);
    RUN_TEST(test_Timer_BoundaryConditions);
    RUN_TEST(test_Timer_RepeatedCalls);
    RUN_TEST(test_Timer_LargeInterval);
    RUN_TEST(test_Timer_ConsecutiveOperations);
    RUN_TEST(test_Timer_StateConsistency);

    return UNITY_END();
}

int main(void)
{
    return runUnityTests();
}