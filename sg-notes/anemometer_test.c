#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../gpio.h"

#include <sys/time.h>

/**
 * Returns the current time in microseconds.
 */
long get_micro_time(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

char* GPIO_DIRECTION_MAP[] = {"INPUT","OUTPUT"};
char* GPIO_VALUE_MAP[] = {"LOW","HIGH"};

int main()
{
    gpio_pin gpio60;
    gpio60.number = 60;

    // Initialize
    GPIO_DIRECTION dir = gpio_set_direction(&gpio60,GPIO_DIR_INPUT);
    GPIO_VALUE val = gpio_get_value(&gpio60);
    printf("Reading initial pin 67 value: %s\n",GPIO_VALUE_MAP[val]);

    GPIO_VALUE last_val = GPIO_HIGH;
    GPIO_VALUE current_val = GPIO_HIGH;

    long last_time_us = get_micro_time();
    long current_time_us = last_time_us;
    long elapsed_time = 0;

    int num_transitions = 0;
    float flow_rate;

    while(1)
    {
        // Main loop
        current_val = gpio_get_value(&gpio60);
        if((last_val == GPIO_LOW)&&(current_val == GPIO_HIGH))
        {
            // Rising edge, catch current time in us
            num_transitions++;
            current_time_us = get_micro_time();
            elapsed_time = current_time_us - last_time_us;
            last_time_us = current_time_us;

            flow_rate = 1e6/((float)elapsed_time);
            printf("Current rate (Hz): %f",flow_rate);
        }
        last_val = current_val;

    }


    return 0;
}




