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



void setExpectedDataToWriteOneByteFunction(oneWireSensorCommand_t command){

 uint8_t i;

 for(i=0; i<8; i++){

  gpioConfig_CMockExpectAndReturn(20, GPIO0, GPIO_OUTPUT, 

 1

 );

  gpioWrite_CMockExpectAndReturn(21, GPIO0, 

 0

 , 

 1

 );

  if(command & 0x01){

   delayInaccurateUs_CMockExpect(23, 10);

   gpioConfig_CMockExpectAndReturn(24, GPIO0, GPIO_INPUT, 

  1

  );

   delayInaccurateUs_CMockExpect(25, 52);

  }

  else{

   delayInaccurateUs_CMockExpect(28, 60);

   gpioConfig_CMockExpectAndReturn(29, GPIO0, GPIO_INPUT, 

  1

  );

   delayInaccurateUs_CMockExpect(30, 2);

  }

  command = command >> 1;

 }

}



void setExpectedDataToReadOneByteFunction(uint8_t readByte){

 uint8_t i;

 for(i=0; i<8; i++){

  gpioConfig_CMockExpectAndReturn(39, GPIO0, GPIO_OUTPUT, 

 1

 );

  gpioWrite_CMockExpectAndReturn(40, GPIO0, 

 0

 , 

 1

 );

  delayInaccurateUs_CMockExpect(41, 10);

  gpioConfig_CMockExpectAndReturn(42, GPIO0, GPIO_INPUT, 

 1

 );

  delayInaccurateUs_CMockExpect(43, 12);

  if(readByte & 0x01){

   gpioRead_CMockExpectAndReturn(45, GPIO0, 

  1

  );

  }

  else{

   gpioRead_CMockExpectAndReturn(48, GPIO0, 

  0

  );

  }

  delayInaccurateUs_CMockExpect(50, 50);

  readByte = readByte >> 1;

 }

}



void setExpectedDataToResetFunction(uint8_t presencePulse){

 gpioConfig_CMockExpectAndReturn(56, GPIO0, GPIO_OUTPUT, 

1

);

 gpioWrite_CMockExpectAndReturn(57, GPIO0, 

0

, 

1

);

 delayInaccurateUs_CMockExpect(58, 480);

 gpioConfig_CMockExpectAndReturn(59, GPIO0, GPIO_INPUT, 

1

);

 delayInaccurateUs_CMockExpect(60, 80);

 if(!presencePulse){

  gpioRead_CMockExpectAndReturn(62, GPIO0, 

 0

 );

 }

 else{

  gpioRead_CMockExpectAndReturn(65, GPIO0, 

 1

 );

 }

 delayInaccurateUs_CMockExpect(67, 400);

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

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((NINE_BITS_RESOLUTION + i)), (UNITY_INT)(UNITY_INT32)((me.operation.resolution)), ((msg)), (UNITY_UINT)(88), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((oneWireSensorStep[i])) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((oneWireSensorStep[i]))), (UNITY_FLOAT)((UNITY_FLOAT)((me.operation.step))), (((msg))), (UNITY_UINT)((UNITY_UINT)(89)));

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((oneWireSensorDelay[i])), (UNITY_INT)(UNITY_INT32)((me.operation.delay)), ((msg)), (UNITY_UINT)(90), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((oneWireSensorMask[i])), (UNITY_INT)(UNITY_INT32)((me.operation.mask)), ((msg)), (UNITY_UINT)(91), UNITY_DISPLAY_STYLE_INT32);

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((GPIO0 + i)), (UNITY_INT)(UNITY_INT32)((me.gpio)), ((msg)), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_INT32);

 }

}



void testWriteOneByteToSensor(){

 setExpectedDataToWriteOneByteFunction(SKIP_ROM);

 oneWireSensorWriteByte(&me, SKIP_ROM);

}





void testReadOneByteFromSensor(){

 setExpectedDataToReadOneByteFunction(0xAA);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0xAA)), (UNITY_INT)(UNITY_INT8 )((oneWireSensorReadByte(&me))), (

((void *)0)

), (UNITY_UINT)(104), UNITY_DISPLAY_STYLE_HEX8);

}



void testResetSensor(){

 setExpectedDataToResetFunction(

                               0

                                    );

 UnityAssertEqualNumber((UNITY_INT)((

1

)), (UNITY_INT)((oneWireSensorReset(&me))), (

((void *)0)

), (UNITY_UINT)(109), UNITY_DISPLAY_STYLE_INT);

 setExpectedDataToResetFunction(

                               1

                                   );

 UnityAssertEqualNumber((UNITY_INT)((

0

)), (UNITY_INT)((oneWireSensorReset(&me))), (

((void *)0)

), (UNITY_UINT)(111), UNITY_DISPLAY_STYLE_INT);

}
