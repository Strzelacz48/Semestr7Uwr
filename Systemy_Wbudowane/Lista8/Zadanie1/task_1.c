#include "FreeRTOS.h"
#include "task.h"
#include <avr/io.h>
#include <stdint.h>

// Marcin Czapulak

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/

#define mainLED_TASK_PRIORITY   2
#define mainBUTTON_TASK_PRIORITY 1

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB

#define BUFFER_SIZE 100

/******************************************************************************
 * Private function prototypes.
 ******************************************************************************/

static void vRunningLight(void* pvParameters);
static void vButtonMonitor(void* pvParameters);

/******************************************************************************
 * Private variables.
 ******************************************************************************/

static uint8_t button_states[BUFFER_SIZE] = {0};

/******************************************************************************
 * Public function definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn int main(void)
 *
 * \brief Main function.
 *
 * \return
 ******************************************************************************/
int main(void)
{
    // Configure button pin as input with pull-up
    BTN_PORT |= _BV(BTN);
    
    // Configure LED pin as output
    LED_DDR |= _BV(LED);

    // Create tasks
    xTaskHandle running_light_handle, button_monitor_handle;

    xTaskCreate
        (
         vRunningLight,
         "RunningLight",
         configMINIMAL_STACK_SIZE,
         NULL,
         mainLED_TASK_PRIORITY,
         &running_light_handle
        );

    xTaskCreate
        (
         vButtonMonitor,
         "ButtonMonitor",
         configMINIMAL_STACK_SIZE,
         NULL,
         mainBUTTON_TASK_PRIORITY,
         &button_monitor_handle
        );

    // Start scheduler
    vTaskStartScheduler();

    return 0;
}

void vApplicationIdleHook(void)
{
    // Do nothing, just idle.
}

static void vRunningLight(void* pvParameters)
{
    // Configure all PORTD pins as output
    DDRD |= 0xFF;

    uint8_t i = 0;

    for (;;)
    {
        // Shift LED light to the right
        for(int i = 0; i < 8; i++){
            PORTD = _BV(i);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        // Shift LED light to the left
        for(int i = 7; i >= 0; i--){
            PORTD = _BV(i);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

static void vButtonMonitor(void* pvParameters)
{
    uint8_t index = 0;

    for (;;)
    {
        // Update LED state based on button state
        if (button_states[index])
            LED_PORT &= ~_BV(LED);
        else
            LED_PORT |= _BV(LED);

        // Read button state and store in buffer
        button_states[index] = BTN_PIN & _BV(BTN);
        index = (index + 1) % BUFFER_SIZE;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
