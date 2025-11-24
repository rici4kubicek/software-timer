Getting started
===============

This page walks you through the basic usage of the library.

Installation and integration into a project
------------------------------------------

The library is written in C and consists of the header ``include/software_timer.h``
and the implementation in ``src/software_timer.c``. The library is simple and
self-contained with no configuration files needed.

Below are the recommended installation methods, similar to the PlatformIO
Installation page:
https://registry.platformio.org/libraries/rici4kubicek/SoftwareTimer/installation

- PlatformIO (recommended)
    Add the dependency to your ``platformio.ini``:

     .. code-block:: ini

        [env:your_env]
        platform = ...
        board = ...
        framework = ...
        lib_deps =
            rici4kubicek/SoftwareTimer@^1.0.2

     Note: you can pin the version as needed. The latest version is listed on
     the library page in the PlatformIO registry.

- PlatformIO (install directly from the registry via CLI)

  Run in the project root:

  .. code-block:: bash

     pio lib install "rici4kubicek/SoftwareTimer"

- CMake / manual integration (without PlatformIO)

  1. Add the source files to your build system:

     - Add ``src/software_timer.c`` to your compilation.
     - Make sure the ``include`` directory is on the include path.

     Example for CMake:

     .. code-block:: cmake

        add_library(SoftwareTimer STATIC
            ${CMAKE_CURRENT_LIST_DIR}/src/software_timer.c)
        target_include_directories(SoftwareTimer PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/include)

        # optional: your targets that use the library
        target_link_libraries(my_app PRIVATE SoftwareTimer)

- As a git submodule

  .. code-block:: bash

     git submodule add https://github.com/rici4kubicek/software-timer.git external/software-timer

  Then add the source and include paths (see the CMake example above) to your
  build.

Quick usage example
-------------------

The following example demonstrates basic timer usage with a simple millisecond
clock function:

.. code-block:: c

   #include <software_timer.h>
   #include <stdio.h>
   #include <time.h>

   // Simple millisecond clock implementation
   static uint32_t millis(void) {
       struct timespec ts;
       clock_gettime(CLOCK_MONOTONIC, &ts);
       return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
   }

   int main(void) {
       SoftwareTimer myTimer;

       // Initialize the timer module with clock source
       SoftwareTimer_Init(millis);

       // Set timer to expire after 1000ms (1 second)
       SoftwareTimer_Set(&myTimer, 1000);

       // Main loop
       while (1) {
           if (SoftwareTimer_IsExpired(&myTimer)) {
               printf("Timer expired!\n");
               // Reset timer for another 2 seconds
               SoftwareTimer_Set(&myTimer, 2000);
           }

           // Check remaining time
           uint32_t remaining = SoftwareTimer_Remaining(&myTimer);
           if (remaining > 0) {
               printf("Time remaining: %u ms\n", remaining);
           }
       }

       return 0;
   }

One-shot timer example
----------------------

For timers that should only trigger once, use the evaluated-once variant:

.. code-block:: c

   #include <software_timer.h>
   #include <stdio.h>

   int main(void) {
       SoftwareTimer myTimer;

       SoftwareTimer_Init(millis);
       SoftwareTimer_Set(&myTimer, 5000);  // 5 second timeout

       while (1) {
           // This will return true only once when timer expires
           if (SoftwareTimer_IsExpiredEvaluatedOnce(&myTimer)) {
               printf("Timer expired once!\n");
               // Timer won't trigger again until you call Set() again
           }
       }

       return 0;
   }

Key features
------------

- **Overflow-safe**: Uses unsigned arithmetic that works correctly even when
  the system clock wraps around at 32-bit boundary.
- **Lightweight**: Minimal memory footprint, suitable for embedded systems.
- **Flexible**: Works with any monotonic clock source via callback function.
- **Simple API**: Only 5 functions to learn.
- **Configurable assertions**: Optional parameter validation with customizable
  error handling.

Configuring assertion behavior
-------------------------------

The library includes optional parameter validation using assertions. By default:

- **Debug builds** (NDEBUG not defined): Uses standard ``assert()``
- **Release builds** (NDEBUG defined): Assertions are compiled out (zero overhead)

You can customize the assertion behavior by defining ``SOFTWARETIMER_ASSERT``
before compiling the library. This is useful for embedded systems with custom
error handling.

Example - Custom error handler:

.. code-block:: c

   // In your build configuration or before including the source:
   #define SOFTWARETIMER_ASSERT(expr) \
       do { \
           if (!(expr)) { \
               error_handler(__FILE__, __LINE__, #expr); \
           } \
       } while(0)

Example - Always-on assertions:

.. code-block:: c

   // Keep assertions even in release builds:
   #include <assert.h>
   #define SOFTWARETIMER_ASSERT(expr) assert(expr)

Example - Embedded system with fault handler:

.. code-block:: c

   // For embedded systems without standard library:
   #define SOFTWARETIMER_ASSERT(expr) \
       do { \
           if (!(expr)) { \
               system_fault_handler(); \
               while(1); /* halt execution */ \
           } \
       } while(0)

See ``include/software_timer_config_template.h`` for more configuration examples.

Notes
-----

- The path to the Doxygen XML is configured in ``docs/source/conf.py`` via
  ``breathe_projects`` and points to ``../build/doxygen/xml`` (relative to
  ``docs/source``).
- The documentation theme is ``sphinx_rtd_theme``.

Further reading
---------------

- :doc:`api` – detailed API reference including all functions and structures
- ``example/src/main.c`` – complete working example
- ``test/test.c`` – comprehensive unit tests demonstrating all features
