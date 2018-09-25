/*==================[inclusions]=============================================*/
#include "oneWireSensor.h"
#include "delay.h"
/*==================[macros and definitions]=================================*/
/**
* @def BIT_MASK
* @brief Mascara de un bit.
*/
#define BIT_MASK						0x01
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
/**
* @fn static void oneWireSensorWriteBit(uint8_t bit)
* @brief Escritura de 1 bit del sensor 
* @param bit : Bit a escribir.
* @return Nada.
* @note Funcion privada.
*/
static void oneWireSensorWriteBit(oneWireSensor_t *me, uint8_t bit){
	/* Configuracion de GPIO como OUTPUT */
	gpioConfig(me->gpio, GPIO_OUTPUT);	
	/* Seteado del bus en bajo*/
	gpioWrite(me->gpio, false);
	if (bit){ /* Escribir un '1' */
		/* Liberacion del bus despues de 10us */
		delayInaccurateUs(10);
		gpioConfig(me->gpio, GPIO_INPUT);
		/* Esperar fin slot escritura + 2us de recuperacion entre bits sucesivos */
		delayInaccurateUs(52);
	}
	else{ /* Escribir un '0' */
		/* Mantener en bajo el bus durante todo el slot de escritura */
		delayInaccurateUs(60);
		/* Liberacion del bus*/
		gpioConfig(me->gpio, GPIO_INPUT);
		/* 2us de recuperacion entre bits sucesivos */
		delayInaccurateUs(2);
	}
}

/*==================[external functions definition]==========================*/
void oneWireSensorInit(oneWireSensor_t *me, oneWireSensorResolution_t resolution, gpioMap_t gpio){
	me->operation.resolution = operation[resolution - NINE_BITS_RESOLUTION].resolution;
	me->operation.step = operation[resolution - NINE_BITS_RESOLUTION].step;
	me->operation.delay = operation[resolution - NINE_BITS_RESOLUTION].delay;
	me->operation.mask = operation[resolution - NINE_BITS_RESOLUTION].mask;
	me->gpio = gpio;
}

void oneWireSensorWriteByte(oneWireSensor_t *me, oneWireSensorCommand_t command){
	uint8_t currentBit;  /** currentBit : Bit que en cada momento se esta escribiendo */
	/* Por cada bit dentro del comando, se hace un AND con la mascara de un bit(0x01) y un corrimiento del comando a derecha */
	for (currentBit = 0; currentBit < 8; currentBit++){
		oneWireSensorWriteBit(me, command & BIT_MASK);
		command = command >> 1;	
	}
}

/*==================[end of file]============================================*/
