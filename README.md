# Blink-PWM

Blink is almost the simplest application you can build. It initializes the OS
and starts blinking three LEDs. Each LED is driven by a different timer, the
first running at 1Hz, the second at 0.5Hz and the third at 0.25Hz.

The application is intended to demonstrate how to set up peripheral TIMERs in
PWM mode and control LED brightness by changing PWM duty cycle. The three LEDs
are given different duty cycles and LED fading/brightening functionality is 
also implemented.

Additionally led changes are indicated in debug output.

The application is inspired byt the TinyOS Blink application.
https://github.com/tinyos/tinyos-main/tree/master/apps/Blink

# Platforms
The application has been developed for tsb0 platform, but should also work with 
the following platforms:
 * Thinnect TestSystemBoards (tsb0, tsb1, tsb2)
 * SiLabs Thunderboard Sense (thunderboard)
 * SiLabs Thunderboard Sense 2 (thunderboard2)
 * SiLabs BRD4001A + BRD4161A (brd4161a-mb)
 * SiLabs BRD4001A + BRD4162A (brd4162a-mb)
 * SiLabs BRD4001A + BRD4304A (brd4304a-mb)

TODO Series2 boards:
 * SiLabs BRD4001A + BRD4180A (brd4180a-mb)

WARNING! Be sure to assign correct LED pin locations to TIMER PWM output suitable 
for your board, otherwise you might end up regulatin unwanted pin voltage!

# Build
Standard build options apply, check the main [README](../../README.md).
