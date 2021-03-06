#include "unity.h"
#include "oneWireSensor.h"
#include "mock_gpio.h"
#include "mock_delay.h"
#include <stdbool.h>


oneWireSensor_t me;

void resetTest(void);

void setUp(void){
	oneWireSensorInit(&me, TWELVE_BITS_RESOLUTION, GPIO0);
}

void cleanup(void){

}

void setExpectedDataToWriteOneByteFunction(oneWireSensorCommand_t command){
	uint8_t i;
	for(i=0; i<8; i++){
		gpioConfig_ExpectAndReturn(GPIO0, GPIO_OUTPUT, true);
		gpioWrite_ExpectAndReturn(GPIO0, false, true);
		if(command & 0x01){
			delayInaccurateUs_Expect(10);
			gpioConfig_ExpectAndReturn(GPIO0, GPIO_INPUT, true);
			delayInaccurateUs_Expect(52);
		}
		else{
			delayInaccurateUs_Expect(60);
			gpioConfig_ExpectAndReturn(GPIO0, GPIO_INPUT, true);
			delayInaccurateUs_Expect(2);
		}
		command = command >> 1;	
	}
}

void setExpectedDataToReadOneByteFunction(uint8_t readByte){
	uint8_t i;
	for(i=0; i<8; i++){
		gpioConfig_ExpectAndReturn(GPIO0, GPIO_OUTPUT, true);
		gpioWrite_ExpectAndReturn(GPIO0, false, true);
		delayInaccurateUs_Expect(10);
		gpioConfig_ExpectAndReturn(GPIO0, GPIO_INPUT, true);
		delayInaccurateUs_Expect(12);
		if(readByte & 0x01){
			gpioRead_ExpectAndReturn(GPIO0, true);
		}
		else{
			gpioRead_ExpectAndReturn(GPIO0, false);
		}
		delayInaccurateUs_Expect(50);	
		readByte = readByte >> 1;
	}
}

void setExpectedDataToResetFunction(uint8_t presencePulse){
	gpioConfig_ExpectAndReturn(GPIO0, GPIO_OUTPUT, true);		
	gpioWrite_ExpectAndReturn(GPIO0, false, true);
	delayInaccurateUs_Expect(480);
	gpioConfig_ExpectAndReturn(GPIO0, GPIO_INPUT, true);		
	delayInaccurateUs_Expect(80);
	if(!presencePulse){ 
		gpioRead_ExpectAndReturn(GPIO0, false);
	}
	else{ 
		gpioRead_ExpectAndReturn(GPIO0, true);
	}
	delayInaccurateUs_Expect(400);
}

void testSensorReadyToWorkAfterInit(){
	oneWireSensor_t me;
	uint8_t i;
	char msg[20];
	float oneWireSensorStep[MAX_RESOLUTIONS] = {0.5, 0.25, 0.125, 0.0625};
	uint32_t oneWireSensorDelay[MAX_RESOLUTIONS] = { NINE_BITS_RESOLUTION_DELAY,
													TEN_BITS_RESOLUTION_DELAY,
													ELEVEN_BITS_RESOLUTION_DELAY,
													TWELVE_BITS_RESOLUTION_DELAY };

	uint32_t oneWireSensorMask[MAX_RESOLUTIONS] = {  NINE_BITS_RESOLUTION_MASK,
												    TEN_BITS_RESOLUTION_MASK,
												    ELEVEN_BITS_RESOLUTION_MASK,
													TWELVE_BITS_RESOLUTION_MASK };

	for(i=0; i<MAX_RESOLUTIONS; i++){
		sprintf(msg, "Resolucion: %d", NINE_BITS_RESOLUTION + i);
		oneWireSensorInit(&me, NINE_BITS_RESOLUTION + i, GPIO0 + i);
		TEST_ASSERT_EQUAL_INT32_MESSAGE(NINE_BITS_RESOLUTION + i, me.operation.resolution, msg);
		TEST_ASSERT_EQUAL_FLOAT_MESSAGE(oneWireSensorStep[i], me.operation.step, msg);
		TEST_ASSERT_EQUAL_INT32_MESSAGE(oneWireSensorDelay[i], me.operation.delay, msg);
		TEST_ASSERT_EQUAL_INT32_MESSAGE(oneWireSensorMask[i], me.operation.mask, msg);
		TEST_ASSERT_EQUAL_INT32_MESSAGE(GPIO0 + i, me.gpio, msg);
	}
}

void testWriteOneByteToSensor(){
	setExpectedDataToWriteOneByteFunction(SKIP_ROM);
	oneWireSensorWriteByte(&me, SKIP_ROM);
}


void testReadOneByteFromSensor(){
	setExpectedDataToReadOneByteFunction(0xAA);
	TEST_ASSERT_EQUAL_HEX8(0xAA, oneWireSensorReadByte(&me));
}

void testResetSensor(){
	setExpectedDataToResetFunction(false);
	TEST_ASSERT_EQUAL(true, oneWireSensorReset(&me));
	setExpectedDataToResetFunction(true);
	TEST_ASSERT_EQUAL(false, oneWireSensorReset(&me));
}

void testFillSensorScratchpad(){
	uint8_t i;
	uint8_t scratchpad[]= {85, 1, 75, 70, 127, 255, 12, 16, 190};

	/* Primer caso: Primer RESET falla */
	setExpectedDataToResetFunction(true);
	TEST_ASSERT_EQUAL_MESSAGE(false, oneWireSensorFillScratchpad(&me), "Primer Caso");
	resetTest();

	/* Segundo caso: Segundo RESET falla */
	setExpectedDataToResetFunction(false);
	setExpectedDataToWriteOneByteFunction(SKIP_ROM);
	setExpectedDataToWriteOneByteFunction(CONVERT_T);
	delay_Expect(me.operation.delay);
	setExpectedDataToResetFunction(true);
	TEST_ASSERT_EQUAL_MESSAGE(false, oneWireSensorFillScratchpad(&me), "Segundo Caso");
	resetTest();

	/* Tercer caso: CRC leido incorrecto */
	setExpectedDataToResetFunction(false);
	setExpectedDataToWriteOneByteFunction(SKIP_ROM);
	setExpectedDataToWriteOneByteFunction(CONVERT_T);
	delay_Expect(me.operation.delay);
	setExpectedDataToResetFunction(false);
	setExpectedDataToWriteOneByteFunction(SKIP_ROM);
	setExpectedDataToWriteOneByteFunction(READ_SCRATCHPAD);

	for(i=0; i<SCRATCHPAD_LENGTH; i++){
		setExpectedDataToReadOneByteFunction(0xAA);
	}

	TEST_ASSERT_EQUAL_MESSAGE(false, oneWireSensorFillScratchpad(&me), "Tercer Caso");
	resetTest();

	/* Cuarto caso: Todo funciona + CRC leido correctamente */
	setExpectedDataToResetFunction(false);
	setExpectedDataToWriteOneByteFunction(SKIP_ROM);
	setExpectedDataToWriteOneByteFunction(CONVERT_T);
	delay_Expect(me.operation.delay);
	setExpectedDataToResetFunction(false);
	setExpectedDataToWriteOneByteFunction(SKIP_ROM);
	setExpectedDataToWriteOneByteFunction(READ_SCRATCHPAD);

	for(i=0; i<SCRATCHPAD_LENGTH; i++){
		setExpectedDataToReadOneByteFunction(scratchpad[i]);
	}

	TEST_ASSERT_EQUAL_MESSAGE(true, oneWireSensorFillScratchpad(&me), "Cuarto Caso");

}

void testTempCalc(){
	TEST_ASSERT_EQUAL_FLOAT(21.3125, oneWireSensorCalcTempValue(&me));
}

