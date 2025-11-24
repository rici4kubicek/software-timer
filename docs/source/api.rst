API reference
=============

This section documents the public C API of the library. The documentation is
extracted from header comments and grouped using Doxygen.

Overview
--------

- Core structures and functions are defined in the main module.
- Main header file: ``include/software_timer.h``.

Main structure
--------------

.. doxygenstruct:: SoftwareTimer
   :project: SoftwareTimer
   :members:

Type definitions
----------------

.. doxygentypedef:: SoftwareTimer_ClockTime
   :project: SoftwareTimer

Functions
---------

.. doxygenfunction:: SoftwareTimer_Init
   :project: SoftwareTimer

.. doxygenfunction:: SoftwareTimer_Set
   :project: SoftwareTimer

.. doxygenfunction:: SoftwareTimer_IsExpired
   :project: SoftwareTimer

.. doxygenfunction:: SoftwareTimer_Remaining
   :project: SoftwareTimer

.. doxygenfunction:: SoftwareTimer_IsExpiredEvaluatedOnce
   :project: SoftwareTimer

Header file
-----------

.. doxygenfile:: include/software_timer.h
   :project: SoftwareTimer
