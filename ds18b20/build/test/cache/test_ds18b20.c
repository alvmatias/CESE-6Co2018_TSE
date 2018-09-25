#include "build/temp/_test_ds18b20.c"
#include "gpio.h"
#include "oneWireSensor.h"
#include "unity.h"




void setup(){



}



void cleanup(){



}





void testSensorReadyToWorkAfterInit(){

 oneWireSensor_t me;

 uint8_t i;

 char msg[20];

 float oneWireSensorStep[4] = {0.5, 0.25, 0.125, 0.0625};

 uint32_t oneWireSensorDelay[4] = { NINE_BITS_RESOLUTION_DELAY,

             TEN_BITS_RESOLUTION_DELAY,

             ELEVEN_BITS_RESOLUTION_DELAY,

             TWELVE_BITS_RESOLUTION_DELAY };



 uint32_t oneWireSensorMask[4] = { NINE_BITS_RESOLUTION_MASK,

                TEN_BITS_RESOLUTION_MASK,

                ELEVEN_BITS_RESOLUTION_MASK,

             TWELVE_BITS_RESOLUTION_MASK };



 for(i=0; i<4; i++){

  sprintf(msg, "Resolucion: %d", NINE_BITS_RESOLUTION + i);

  oneWireSensorInit(&me, NINE_BITS_RESOLUTION + i, GPIO0 + i);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((NINE_BITS_RESOLUTION + i)), (UNITY_INT)(UNITY_INT32)((me.operation.resolution)), ((msg)), (UNITY_UINT)(33), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((oneWireSensorStep[i])) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((oneWireSensorStep[i]))), (UNITY_FLOAT)((UNITY_FLOAT)((me.operation.step))), (((msg))), (UNITY_UINT)((UNITY_UINT)(34)));

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((oneWireSensorDelay[i])), (UNITY_INT)(UNITY_INT32)((me.operation.delay)), ((msg)), (UNITY_UINT)(35), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((oneWireSensorMask[i])), (UNITY_INT)(UNITY_INT32)((me.operation.mask)), ((msg)), (UNITY_UINT)(36), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((GPIO0 + i)), (UNITY_INT)(UNITY_INT32)((me.gpio)), ((msg)), (UNITY_UINT)(37), UNITY_DISPLAY_STYLE_INT32);

 }

}
