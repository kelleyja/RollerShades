#include <WEMOS_Motor.h>
#include <Wire.h>
#include <Homie.h>
int pwm = 100;
int numericValue = 0;
int timer = 0;
int timer2 = 0;
int shadeState = 0;
int shade2State = 0;
//Motor shiled I2C Address: 0x30
//PWM frequency: 1000Hz(1kHz)
Motor M1(0x30,_MOTOR_A, 1000);//Motor A
Motor M2(0x30,_MOTOR_B, 1000);//Motor B

const int PIN_RELAY = 16;
const int PIN_LED = 2;
const int PIN_BUTTON = 0;
//const int sclPin = 1;
//const int sdaPin = 3;
int currPosition = 0;
int diffPosition;
int curr2Position = 0;
int diff2Position;
HomieNode shuttersNode("shutters", "Shutters", "shutters");

bool shuttersLevelHandler(const HomieRange& range, const String& value) {
 Homie.getLogger() << "Property input handler. Receveived value: " << value;
 Serial.println(value);
 float recValue = value.toFloat();  //change the string from homie to a floating point number 
 Serial.println(recValue); // print the decimal 
 recValue = (recValue * 100); // make the decimal a whole number 
 Serial.println(recValue); // print the whole number
 diffPosition = (currPosition - recValue);
 if (diffPosition < 1) {M1.setmotor( _CW, pwm);
 Homie.getLogger() << "Starting motor CW rotation " << endl;
 shadeState = 1;
 }
 else {M1.setmotor( _CCW, pwm);
 Homie.getLogger() << "Starting motor CCW rotation " << endl;
 shadeState = 1;
 }
 diffPosition = (diffPosition * 450); // we'll pass this value to the motor run time delay, X1000 to convert MS to Seconds
 diffPosition = abs(diffPosition);  // abs() so delay is always positive
 Homie.getLogger() << "motor will run for : " << diffPosition << endl;
 timer = millis();
 //if ((millis() - timer) > diffPosition){
 //M1.setmotor(_SHORT_BRAKE);
 //Homie.getLogger() << "Stopping motor " << diffPosition;
 currPosition = recValue;
 shuttersNode.setProperty("Leftlevel").send(String(currPosition));
 return true;
  }

  bool shutters2LevelHandler(const HomieRange& range, const String& value) {
 Homie.getLogger() << "Property input handler. Receveived value: " << value;
 Serial.println(value);
 float recValue = value.toFloat();  //change the string from homie to a floating point number 
 Serial.println(recValue); // print the decimal 
 recValue = (recValue * 100); // make the decimal a whole number 
 Serial.println(recValue); // print the whole number
 diff2Position = (curr2Position - recValue);
 if (diff2Position < 1) {M2.setmotor( _CW, pwm);
 Homie.getLogger() << "Starting 2 motor CW rotation " << endl;
 shade2State = 1;
 }
 else {M2.setmotor( _CCW, pwm);
 Homie.getLogger() << "Starting 2 motor CCW rotation " << endl;
 shade2State = 1;
 }
 diff2Position = (diff2Position * 450); // we'll pass this value to the motor run time delay, X1000 to convert MS to Seconds
 diff2Position = abs(diff2Position);  // abs() so delay is always positive
 Homie.getLogger() << "motor 2 will run for : " << diff2Position << endl;
 timer2 = millis();

 curr2Position = recValue;
 shuttersNode.setProperty("Rightlevel").send(String(curr2Position));
 return true;
  }

  

//  numericValue = value.toInt();
//  numericValue = (numericValue * 100);
 // if (numericValue > 100) return false; // wanted value is valid
//  if (numericValue == currPosition) return true; // nothing to do

//  M1.setmotor( _CW, pwm);
//  Serial.println("Motor M1 START");
//  delay(5000);
//  Serial.println("MOTOR_SHORT_BRAKE");
//  M1.setmotor(_SHORT_BRAKE);
//  currPosition = numericValue;
//  Serial.println(currPosition);
 // shuttersNode.setProperty("level").send(String(currPosition));
 // Homie.getLogger() << "Shade Level is " << currPosition << ": " << value  << endl;

//  return true;
// }

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial << endl << endl;
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  Homie_setFirmware("shades2", "1.2.0");
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  //switchNode.advertise("on").setName("On").setDatatype("boolean").settable(switchOnHandler);


  shuttersNode.advertise("Leftlevel").setName("Left").setDatatype("Percent").setFormat("0:100").settable(shuttersLevelHandler);
  shuttersNode.advertise("Rightlevel").setName("Right").setDatatype("Percent").setFormat("0:100").settable(shutters2LevelHandler);


  Homie.setup();

  // Motorshield setup
 // Serial.println("Motor Shield Testing...");
//
//  while (motor.PRODUCT_ID != PRODUCT_ID_I2C_MOTOR) //wait motor shield ready.
//  {
//    motor.getInfo();
//  }

}
void loop() {
  Homie.loop();
if ((shadeState == 1) && (millis() - timer > diffPosition)){
 M1.setmotor(_SHORT_BRAKE);
 Homie.getLogger() << "Stopping motor after " << diffPosition << "seconds." << endl;
 timer=0;
 shadeState = 0;
 }

 if ((shade2State == 1) && (millis() - timer2 > diff2Position)){
 M2.setmotor(_SHORT_BRAKE);
 Homie.getLogger() << "Stopping motor 2 after " << diff2Position << "seconds." << endl;
 timer2=0;
 shade2State = 0;
 }
 
}