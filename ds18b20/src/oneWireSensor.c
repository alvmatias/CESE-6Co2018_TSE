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
* @fn static void oneWireSensorWriteBit(oneWireSensor_t *me, uint8_t bit)
* @brief Escritura de 1 bit del sensor 
* @param me  : Estructura de datos del sensor a escribir un bit.
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

/**
* @fn static uint8_t oneWireSensorReadBit(oneWireSensor_t *me)
* @brief Lectura de 1 bit del sensor 
* @param me : Estructura de datos del sensor a leer un bit.
* @return El bit leido.
* @note Funcion privada.
*/
static uint8_t oneWireSensorReadBit(oneWireSensor_t *me){
	uint8_t bit = 0;  /** bit : Bit leido del sensor */

	/* Configuracion del GPIO como OUTPUT */
	gpioConfig(me->gpio, GPIO_OUTPUT);		
	/* Seteado del bus en bajo por al menos 1us*/
	gpioWrite(me->gpio, false);
	delayInaccurateUs(10);
	/* Configuracion del GPIO como entrada para liberarlo */
	gpioConfig(me->gpio, GPIO_INPUT);		
	/* Se espera hasta que el valor en el bus es valido*/
	delayInaccurateUs(12);
	/* Lectura del GPIO */
	if(gpioRead(me->gpio)){
		bit = 1;   /* Se leyo un 1 */
	} 
	/* Else -> No hacer nada -> Un cero fue leido */
	/* Se espera a que el sensor libere el bus */
	delayInaccurateUs(50);

	return bit;
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

uint8_t oneWireSensorReadByte(oneWireSensor_t *me){
	uint8_t currentBit;  /** currentBit : Numero actual de bit leido */
	uint8_t byte = 0;    /** byte : Byte leido */
	/* Por cada bit dentro del byte */
	for (currentBit = 0; currentBit < 8; currentBit++){
		/* Se lee un bit y se hace un corrimiento */
		byte = byte | (oneWireSensorReadBit(me) << currentBit);
	}
	return byte;
}

/*==================[end of file]============================================*/
