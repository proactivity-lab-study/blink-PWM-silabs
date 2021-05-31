/**
 * @file blink_pwm_main.c
 * 
 * @brief LED regulation with PWM for Silabs Wireless Gecko platforms.
 *
 * Configures platform, starts logging to UART, starts kernel and keeps 
 * blinking LEDs.
 * 
 * LEDs are controlled by TIMER0 PWM functionality instead of the standard 
 * set/get LEDs function. PWM duty cycle is regularly changed when a LED 
 * transitions from ON to OFF (or visa versa), creating the effect that the 
 * LED fades to OFF or gradually brightens to ON.
 * 
 * @note On the tsb0 platform LEDs visually appear to have different brightness,
 * so PWM duty cycling is also used to bring all three LEDs to a similar brightness
 * level. LED0 (red) and LED2 (blue) are dimmed down to LED1 (green) level.
 * 
 * @author Johannes Ehala, ProLab.
 * @license MIT
 *
 * Copyright Thinnect Inc. 2019
 * Copyright ProLab, TTÜ. 15 April 2020
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"

#include "cmsis_os2.h"

#include "platform.h"

#include "SignatureArea.h"
#include "DeviceSignature.h"

#include "basic_rtos_logger_setup.h"

#include "timer_handler.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

static osMutexId_t m_led_mutex;

static void led0_timer_cb(void* argument)
{
    osMutexAcquire(m_led_mutex, osWaitForever);
    debug1("led0 timer");
    set_leds_pwm(get_leds_pwm() ^ BLINK_PWM_LED0_MASK);
    osMutexRelease(m_led_mutex);
}

static void led1_timer_cb(void* argument)
{
    osMutexAcquire(m_led_mutex, osWaitForever);
    debug1("led1 timer");
    set_leds_pwm(get_leds_pwm() ^ BLINK_PWM_LED1_MASK);
    osMutexRelease(m_led_mutex);
}

static void led2_timer_cb(void* argument)
{
    osMutexAcquire(m_led_mutex, osWaitForever);
    debug1("led2 timer");
    set_leds_pwm(get_leds_pwm() ^ BLINK_PWM_LED2_MASK);
    osMutexRelease(m_led_mutex);
}

// App loop - do setup and periodically print status
void app_loop ()
{
    m_led_mutex = osMutexNew(NULL);

    basic_rtos_logger_setup();
    
    osDelay(1000);

    osTimerId_t led0_timer = osTimerNew(&led0_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerId_t led1_timer = osTimerNew(&led1_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerId_t led2_timer = osTimerNew(&led2_timer_cb, osTimerPeriodic, NULL, NULL);

    debug1("t1 %p t2 %p t3 %p", led0_timer, led1_timer, led2_timer);

    timer0_cc_init();

    osTimerStart(led0_timer, 1000);
    osTimerStart(led1_timer, 2000);
    osTimerStart(led2_timer, 4000);
   
    for (;;)
    {
        osMutexAcquire(m_led_mutex, osWaitForever);
        info1("leds %u", get_leds_pwm());
        osMutexRelease(m_led_mutex);
        osDelay(1000);
        if(PLATFORM_ButtonGet())
        {
            info1("BUTTON");
        }
    }
}

//Use TIMER1 to regularly change PWM duty cycle and create LED fading effect
void dimmer_loop ()
{
    timer1_init();
    start_fading_leds_loop();
}

int main ()
{
    PLATFORM_Init();

    // LEDs
    PLATFORM_LedsInit(); //this also enables GPIO peripheral
    PLATFORM_ButtonPinInit();

    // Configure debug output
    basic_noos_logger_setup();

    info1("Blink-PWM "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Initialize OS kernel
    osKernelInitialize();

    // Create a thread
    const osThreadAttr_t app_thread_attr = { .name = "app" };
    osThreadNew(app_loop, NULL, &app_thread_attr);

    const osThreadAttr_t timer1_thread_attr = { .name = "timer1" };
    osThreadNew(dimmer_loop, NULL, &timer1_thread_attr);

    if (osKernelReady == osKernelGetState())
    {
        // Start the kernel
        osKernelStart();
    }
    else
    {
        err1("!osKernelReady");
    }

    for(;;);
}
