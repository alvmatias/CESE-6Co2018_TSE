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
* @fn static uint8_t oneWireSensorCheckCRC(uint8_t crc, uint8_t data)
* @brief Funcion que calcula el CRC de los datos del SCRATCHPAD del sensor.
* @param crc : Valor de CRC actual.
* @param data : byte de datos proveniente del SCRATCHPAD del sensor.
* @return Valor de crc.
* @note Funcion privada.
*/
static uint8_t oneWireSensorCheckCRC(uint8_t crc, uint8_t data){
	uint8_t i;		/** i : Indice del for */
	/* crc XOR data */
	crc = crc ^ data;
	/* Por cada bit dentro del byte */
	for (i = 0; i < 8; i++){
		/* Si es un '1', se hace un corrimiento a derecha y un XOR con el polinomio */
	    if (crc & BIT_MASK)
	        crc = (crc >> 1) ^ CRC_POLYNOMIAL;
	    else /* Si no, solamente se hace el corrimiento */
	        crc >>= 1;
	}

	return crc;
}

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

oneWireSensorError_t oneWireSensorReset(oneWireSensor_t *me){
	oneWireSensorError_t error;	/** error : Estado del sensor */
	
	/* Configuracion del GPIO como OUTPUT */
	gpioConfig(me->gpio, GPIO_OUTPUT);		
	/* Seteo del bus en bajo */
	gpioWrite(me->gpio, false);
	/* Liberacion del bus despues de 480us */
	delayInaccurateUs(480);
	/* Configuracion del GPIO como entrada para liberar el bus */
	gpioConfig(me->gpio, GPIO_INPUT);		
	/* Espera de 80us para detectar el pulso de presencia del sensor */
	delayInaccurateUs(80);
	/* Lectura del bus para verificar si el sensor esta funcionando */
	if(!gpioRead(me->gpio)){ /* Si el sensor seteo el bus en bajo es porque esta funcionando */
		error = ONE_WIRE_SENSOR_WORKING;
	}
	else{ /* Si el bus sigue en alto producto de la resistencia de pullup es porque NO esta funcionando */
		error = ONE_WIRE_SENSOR_NOT_WORKING;
	}
	/* Espera de 400us para que el sensor libere el bus */
	delayInaccurateUs(400);
	return error;
}

oneWireSensorError_t oneWireSensorFillScratchpad(oneWireSensor_t *me){	
	uint8_t currentByte;  					/** currentByte : Byte leido en cada momento*/
	uint8_t crc = 0;						/** crc : Valor del calculo de CRC */
	/* Reset Sensor */
	if(ONE_WIRE_SENSOR_WORKING == oneWireSensorReset(me)){
		/* Skip ROM */
		oneWireSensorWriteByte(me, SKIP_ROM);
		/* Conversion Temperatura */
		oneWireSensorWriteByte(me, CONVERT_T);
		/* Espera tiempo conversion */
		delay(me->operation.delay);
		/* Reset Sensor */
		if(ONE_WIRE_SENSOR_WORKING == oneWireSensorReset(me)){
			/* Skip ROM */
			oneWireSensorWriteByte(me, SKIP_ROM);
			/* Lectura SRAM SCRATCHPAD */
			oneWireSensorWriteByte(me, READ_SCRATCHPAD);
			/* Llenado del SCRATCHPAD y calculo CRC */
			for(currentByte = 0; currentByte < SCRATCHPAD_LENGTH - 1; currentByte++){
				me->scratchpad[currentByte] = oneWireSensorReadByte(me);
				crc = oneWireSensorCheckCRC(crc, me->scratchpad[currentByte]);
			}
			/* Obtencion CRC del SCRATCHPAD */
			me->scratchpad[SCRATCHPAD_CRC_BYTE] = oneWireSensorReadByte(me);
			/* Si el CRC calculado y el CRC del SCRATCHPAD son iguales, la lectura fue correcta */
			if(crc == me->scratchpad[SCRATCHPAD_CRC_BYTE]){
				return ONE_WIRE_SENSOR_WORKING;
			}
			else return ONE_WIRE_SENSOR_NOT_WORKING;
		}
		else return ONE_WIRE_SENSOR_NOT_WORKING;
	}
	else return ONE_WIRE_SENSOR_NOT_WORKING;
}

float oneWireSensorCalcTempValue(oneWireSensor_t *me){
	/* Retorno del calculo de temperatura */
	return (float) ( ( ( me->scratchpad[SCRATCHPAD_TEMPERATURE_MSB]&0x0f ) <<4 ) +
						 (( me->scratchpad[SCRATCHPAD_TEMPERATURE_LSB]&0xf0 ) >>4) ) +
						 (float)( ( me->scratchpad[SCRATCHPAD_TEMPERATURE_LSB]&me->operation.mask )*
						 me->operation.step );
}
/*==================[end of file]============================================*/
