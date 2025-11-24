# Software timer

A simple, overflow-safe timer library for embedded systems written in C.  
Provides basic timer functionality using an external clock source.

## Overview

This library implements a basic timer mechanism:

- Uses a user-supplied clock function to obtain a 32-bit tick count.
- Safe against 32-bit counter overflows.
- Simple interface to set, check, and query timers.

Ideal for bare-metal or RTOS-based embedded projects.

## Features

- **Overflow safety** through unsigned arithmetic  
- **Easy integration**: only two files (`software_timer.h` and `software_timer.c`)  
- **Lightweight**: minimal RAM and CPU overhead  
- **Portable**: only requires a function returning a `uint32_t` tick count  

## Requirements

- C99-compatible compiler  
- A function that returns a monotonically increasing 32-bit tick count (e.g., from a hardware timer or system tick)
