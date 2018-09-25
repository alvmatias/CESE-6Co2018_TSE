#include "build/temp/_test_ds18b20.c"
#include "mock_delay.h"
#include "mock_gpio.h"
#include "oneWireSensor.h"
#include "unity.h"




oneWireSensor_t me;



void setUp(){

 oneWireSensorInit(&me, NINE_BITS_RESOLUTION, GPIO0);

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

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((NINE_BITS_RESOLUTION + i)), (UNITY_INT)(UNITY_INT32)((me.operation.resolution)), ((msg)), (UNITY_UINT)(36), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((oneWireSensorStep[i])) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((oneWireSensorStep[i]))), (UNITY_FLOAT)((UNITY_FLOAT)((me.operation.step))), (((msg))), (UNITY_UINT)((UNITY_UINT)(37)));

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((oneWireSensorDelay[i])), (UNITY_INT)(UNITY_INT32)((me.operation.delay)), ((msg)), (UNITY_UINT)(38), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((oneWireSensorMask[i])), (UNITY_INT)(UNITY_INT32)((me.operation.mask)), ((msg)), (UNITY_UINT)(39), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((GPIO0 + i)), (UNITY_INT)(UNITY_INT32)((me.gpio)), ((msg)), (UNITY_UINT)(40), UNITY_DISPLAY_STYLE_INT32);

 }

}



void testWriteOneByteToSensor(){

 uint8_t i;

 oneWireSensorCommand_t command = SKIP_ROM;

 for(i=0; i<8; i++){

  gpioConfig_CMockExpectAndReturn(48, GPIO0, GPIO_OUTPUT, 

 1

 );

  gpioWrite_CMockExpectAndReturn(49, GPIO0, 

 0

 , 

 1

 );

  if(command & 0x01){

   delayInaccurateUs_CMockExpect(51, 10);

   gpioConfig_CMockExpectAndReturn(52, GPIO0, GPIO_INPUT, 

  1

  );

   delayInaccurateUs_CMockExpect(53, 52);

  }

  else{

   delayInaccurateUs_CMockExpect(56, 60);

   gpioConfig_CMockExpectAndReturn(57, GPIO0, GPIO_INPUT, 

  1

  );

   delayInaccurateUs_CMockExpect(58, 2);

  }

  command = command >> 1;

 }

 oneWireSensorWriteByte(&me, SKIP_ROM);

}





void testReadOneByteFromSensor(){

 uint8_t i;

 for(i=0; i<8; i++){

  gpioConfig_CMockExpectAndReturn(69, GPIO0, GPIO_OUTPUT, 

 1

 );

  gpioWrite_CMockExpectAndReturn(70, GPIO0, 

 0

 , 

 1

 );

  delayInaccurateUs_CMockExpect(71, 10);

  gpioConfig_CMockExpectAndReturn(72, GPIO0, GPIO_INPUT, 

 1

 );

  delayInaccurateUs_CMockExpect(73, 12);

  if(i%2){

   gpioRead_CMockExpectAndReturn(75, GPIO0, 

  1

  );

  }

  else{

   gpioRead_CMockExpectAndReturn(78, GPIO0, 

  0

  );

  }

  delayInaccurateUs_CMockExpect(80, 50);

 }

 UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0xAA)), (UNITY_INT)(UNITY_INT8 )((oneWireSensorReadByte(&me))), (

((void *)0)

), (UNITY_UINT)(82), UNITY_DISPLAY_STYLE_HEX8);

}
