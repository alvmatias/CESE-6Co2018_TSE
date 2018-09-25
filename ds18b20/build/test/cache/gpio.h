#include "peripheral_map.h"










typedef enum {

   GPIO_INPUT, GPIO_OUTPUT,

   GPIO_INPUT_PULLUP, GPIO_INPUT_PULLDOWN,

   GPIO_INPUT_PULLUP_PULLDOWN,

   GPIO_ENABLE

} gpioInit_t;





_Bool 

    gpioConfig( gpioMap_t pin, gpioInit_t config );



_Bool 

    gpioWrite( gpioMap_t pin, 

                              _Bool 

                                   value );



_Bool 

    gpioRead( gpioMap_t pin );
