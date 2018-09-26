/** 
* @file oneWireSensor.h
* @brief Archivo de cabecera del archivo fuente oneWireSensor.c.
* @brief Manejo de sensor tipo OneWire .
* @note Sensor utilizado Maxim ds18b20. Se considera al sensor conectado con una resistencia de pullup de 4,7K.
* @note Copyright 2018 - Ing Matias Alvarez.
*/

#ifndef _ONE_WIRE_SENSOR_H_
#define _ONE_WIRE_SENSOR_H_

/*==================[inclusions]=============================================*/
#include "gpio.h"
#include <stdint.h>
/*==================[macros]=================================================*/
/** 
* @def MAX_RESOLUTIONS
* @brief Maxima cantidad de resoluciones soportadas por el sensor OneWire.
*/
#define MAX_RESOLUTIONS					4

/**
* @def SCRATCHPAD_LENGTH
* @brief Tamaño del SCRATCHPAD del sensor OneWire.
*/
#define SCRATCHPAD_LENGTH 				9

/**
* @def CRC_POLYNOMIAL
* @brief Polinomio utilizado para calcular el CRC del sensor OneWire.
*/
#define CRC_POLYNOMIAL					0x8C
/*==================[typedef]================================================*/
/**
* @enum oneWireSensorError_t
* @brief Estado funcionamiento del sensor OneWire.
*/
typedef enum{
	ONE_WIRE_SENSOR_NOT_WORKING = 0, 	/**< Sensor OneWire No Funcionando */
	ONE_WIRE_SENSOR_WORKING = 1			/**< Sensor OneWire Funcionando */
}oneWireSensorError_t;

/**
* @enum oneWireSensorCommand_t
* @brief Comandos para el sensor OneWire ds18b20.
*/
typedef enum{
	CONVERT_T = 0x44,   		/**< Conversion de temperatura */
	SKIP_ROM = 0xCC, 			/**< Descarte de ROM */
	READ_SCRATCHPAD = 0xBE		/**< Lectura de SCRATCHPAD */
} oneWireSensorCommand_t;

/**
* @enum scratchpadByte_t
* @brief Descripcion de cada byte del SCRATCHPAD
*/
typedef enum{
	SCRATCHPAD_TEMPERATURE_LSB = 0,  		/**< LSB del valor de la temperatura */
	SCRATCHPAD_TEMPERATURE_MSB = 1,			/**< MSB del valor de la temperatura */
	SCRATCHPAD_TH_REGISTER = 2,				/**< Registro de alarma de temperatura alta */
	SCRATCHPAD_TL_REGISTER = 3,				/**< Registro de alarma de temperatura baja */
	SCRATCHPAD_CONFIGURATION_REGISTER = 4,	/**< Registro de configuracion */
	SCRATCHPAD_RESERVED_1 = 5,				/**< Reservado 1 */
	SCRATCHPAD_RESERVED_2 = 6,				/**< Reservado 2 */
	SCRATCHPAD_RESERVED_3 = 7,  			/**< Reservado 3 */
	SCRATCHPAD_CRC_BYTE = 8					/**< CRC8 */
}scratchpadByte_t;

/**
* @enum oneWireSensorResolution_t
* @brief Resoluciones posbiles en bits del sensor OneWire.
*/
typedef enum{
	NINE_BITS_RESOLUTION = 9, 			/**< 9 bits de resolucion */
	TEN_BITS_RESOLUTION = 10,			/**< 10 bits de resolucion */
	ELEVEN_BITS_RESOLUTION = 11,		/**< 11 bits de resolucion */
	TWELVE_BITS_RESOLUTION = 12			/**< 12 bits de resolucion */
}oneWireSensorResolution_t;

/**
* @enum oneWireSensorResolutionStep_t
* @brief Resoluciones posbiles en grados centigrados(°C) del sensor OneWire.
*/
typedef enum{	
	NINE_BITS_RESOLUTION_STEP = 5, 			/**< 0.5°C de resolucion */
	TEN_BITS_RESOLUTION_STEP = 25, 			/**< 0.25°C de resolucion */
	ELEVEN_BITS_RESOLUTION_STEP = 125,		/**< 0.125°C de resolucion */
	TWELVE_BITS_RESOLUTION_STEP = 625		/**< 0.0625°C de resolucion */
}oneWireSensorResolutionStep_t;

/**
* @enum oneWireSensorDelay_t
* @brief Delay de conversion de temperatura asociado a cada resolucion posible del sensor OneWire.
*/
typedef enum{
	NINE_BITS_RESOLUTION_DELAY = 94, 			/**< 94ms de delay */
	TEN_BITS_RESOLUTION_DELAY = 188,			/**< 188ms de delay */
	ELEVEN_BITS_RESOLUTION_DELAY = 375,			/**< 375ms de delay */
	TWELVE_BITS_RESOLUTION_DELAY = 750			/**< 750ms de delay */
}oneWireSensorDelay_t;

/**
* @enum oneWireSensorMask_t
* @brief Mascara utilizada para calcular la temperatura asociada a cada resolucion posible del sensor OneWire.
*/
typedef enum{
	NINE_BITS_RESOLUTION_MASK = 0x08,		/**< Mascara 1000 */
	TEN_BITS_RESOLUTION_MASK = 0x0a,		/**< Mascara 1100 */
	ELEVEN_BITS_RESOLUTION_MASK = 0x0e,		/**< Mascara 1110 */
	TWELVE_BITS_RESOLUTION_MASK = 0x0f		/**< Mascara 1111 */
}oneWireSensorMask_t;

/**
* @struct oneWireSensorOperation_t
* @brief Estructura de datos relacionada con la operacion propiamente dicha del sensor OneWire.
*/
typedef struct{
	oneWireSensorResolution_t 		resolution;		/**< Resolucion en bits del sensor */
	float						 	step;			/**< Resolucion en °C del sensor */
	oneWireSensorDelay_t 			delay;			/**< Delay de conversion de temperatura del sensor */
	oneWireSensorMask_t				mask;			/**< Mascara del caluclo de temperatura del sensor */
}oneWireSensorOperation_t;

/**
* @struct oneWireSensor_t
* @brief Estructura de datos del sensor OneWire.
*/
typedef struct{
	oneWireSensorOperation_t operation; 			/**< Estructura de datos de operacion del sensor */
	gpioMap_t gpio;									/**< GPIO asociado al bus de datos utilizados por el sensor */
	uint8_t scratchpad[SCRATCHPAD_LENGTH];			/**< SRAM scratchpad asociada al sensor */
}oneWireSensor_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
* @fn void oneWireSensorInit(oneWireSensor_t *me, oneWireSensorResolution_t resolution, gpioMap_t gpio)
* @brief Inicializacion de la estructura de datos del sensor oneWire.
* @param me 		: Estructura de datos del sensor a inicializar.
* @param resolution : Resolucion en bits a utilizar.
* @param gpio 		: GPIO asociado al bus de datos utilizado por el sensor.
* @return Nada.
*/
void oneWireSensorInit(oneWireSensor_t *me, oneWireSensorResolution_t resolution, gpioMap_t gpio);

/**
* @fn void oneWireSensorWriteByte(oneWireSensor_t *me, oneWireSensorCommand_t command)
* @brief Escritura de un byte al sensor
* @param me 	 : Estructura de datos del sensor a escribir un byte.
* @param command : Comando a enviar.
* @return Nada.
*/
void oneWireSensorWriteByte(oneWireSensor_t *me, oneWireSensorCommand_t command);

/**
* @fn uint8_t oneWireSensorReadByte(oneWireSensor_t *me)
* @param me : Estructura de datos del sensor a leer un byte.
* @brief Lectura de 1 byte del sensor. 
* @return El byte leido.
*/
uint8_t oneWireSensorReadByte(oneWireSensor_t *me);

/**
* @fn sensorFunctionalState_t oneWireSensorReset()
* @brief Reseteo y deteccion del estado del sensor
* @param me : Estructura de datos del sensor a resetear.
* @return SENSOR_WORKING si el sensor contesto con el pulso de presencia, SENSOR_NOT_WORKING caso contrario.
*/
oneWireSensorError_t oneWireSensorReset(oneWireSensor_t *me);

/**
* @fn oneWireSensorError_t oneWireSensorFillScratchpad()
* @brief Carga del SCRATCHPAD asociado al sensor a partir de los datos recibidos por parte del sensor.
* @param me : Estructura de datos del sensor a rellenar su SCRATCHPAD
* @return ONE_WIRE_SENSOR_NOT_WORKING si hubo un error en el llenado del scratchpad, ONE_WIRE_SENSOR_WORKING caso contrario.
*/
oneWireSensorError_t oneWireSensorFillScratchpad(oneWireSensor_t *me);

/**
* @fn float oneWireSensorReadTempValue()
* @brief Calculo y lectura de la temperatura del scratchpad.
* @param me : Estructura de datos del sensor a calcular su temperatura
* @return Valor de la temperatura.
* @note Utilizar despues de llenar el SCRATCHPAD correctamente.
*/
float oneWireSensorCalcTempValue(oneWireSensor_t *me);
/*==================[end of file]============================================*/
#endif /* #ifndef _ONE_WIRE_SENSOR_H_ */
