#include "peripheral_map.h"
#include <stdbool.h>

#ifndef _GPIO_H_
#define _GPIO_H_


typedef enum {
   GPIO_INPUT, GPIO_OUTPUT,
   GPIO_INPUT_PULLUP, GPIO_INPUT_PULLDOWN,
   GPIO_INPUT_PULLUP_PULLDOWN,
   GPIO_ENABLE
} gpioInit_t;

bool gpioConfig( gpioMap_t pin, gpioInit_t config );
bool gpioWrite( gpioMap_t pin, bool value );

#endif /* #ifndef GPIO_H_ */