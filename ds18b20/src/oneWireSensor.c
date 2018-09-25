/*==================[inclusions]=============================================*/
#include "oneWireSensor.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal data declaration]==============================*/
/**
* @var static const oneWireSensorOperation_t operation[MAX_RESOLUTIONS]
* @brief Datos internos con las configuraciones posibles de las distintas resoluciones soportas por el sensor OneWire.
*/
static const oneWireSensorOperation_t operation[MAX_RESOLUTIONS] = {
	{NINE_BITS_RESOLUTION, (float)NINE_BITS_RESOLUTION_STEP/10, NINE_BITS_RESOLUTION_DELAY, NINE_BITS_RESOLUTION_MASK},
	{TEN_BITS_RESOLUTION, (float)TEN_BITS_RESOLUTION_STEP/100, TEN_BITS_RESOLUTION_DELAY, TEN_BITS_RESOLUTION_MASK},
	{ELEVEN_BITS_RESOLUTION, (float)ELEVEN_BITS_RESOLUTION_STEP/1000, ELEVEN_BITS_RESOLUTION_DELAY, ELEVEN_BITS_RESOLUTION_MASK},
	{TWELVE_BITS_RESOLUTION, (float)TWELVE_BITS_RESOLUTION_STEP/10000, TWELVE_BITS_RESOLUTION_DELAY, TWELVE_BITS_RESOLUTION_MASK}
};

/*==================[internal functions declaration]=========================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void oneWireSensorInit(oneWireSensor_t *me, oneWireSensorResolution_t resolution, gpioMap_t gpio){
	me->operation.resolution = operation[resolution - NINE_BITS_RESOLUTION].resolution;
	me->operation.step = operation[resolution - NINE_BITS_RESOLUTION].step;
	me->operation.delay = operation[resolution - NINE_BITS_RESOLUTION].delay;
	me->operation.mask = operation[resolution - NINE_BITS_RESOLUTION].mask;
	me->gpio = gpio;
}

/*==================[end of file]============================================*/
