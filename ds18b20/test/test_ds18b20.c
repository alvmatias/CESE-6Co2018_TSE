#include "unity.h"
#include "oneWireSensor.h"
#include "gpio.h"


void setup(){

}

void cleanup(){

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