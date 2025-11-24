# Software timer

![CodeRabbit Pull Request Reviews](https://img.shields.io/coderabbit/prs/github/rici4kubicek/software-timer?utm_source=oss&utm_medium=github&utm_campaign=rici4kubicek%2Fsoftware-timer&labelColor=171717&color=FF570A&link=https%3A%2F%2Fcoderabbit.ai&label=CodeRabbit+Reviews)
[![pre-commit.ci status](https://results.pre-commit.ci/badge/github/rici4kubicek/software-timer/main.svg)](https://results.pre-commit.ci/latest/github/rici4kubicek/software-timer/main)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/rici4kubicek/library/SoftwareTimer.svg)](https://registry.platformio.org/libraries/rici4kubicek/SoftwareTimer)

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
