//Libraries:
#include <MPU9250.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>


//RGB LED Strip attributes
#define BUTTON_PIN   2
#define PIXEL_PIN    3  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 144  // Number of NeoPixels
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
boolean oldState = HIGH;
int     mode     = 0;    // Refers to color of the blade, every increment changes color
int i = 0;

//Sound Outputs
//Sound/Digital pin#/ Audio module pin#
//1.System-on/#4 /#0
//2.Power-on/#5 /#1
//3.Power-off/#7 /#3
//4.HUM,Idle/#6 /#2
//5.Clash/#8 /#4
//6.Swing/#9 /#5
//6.IMU Failure/#10 /#6

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire, 0x68);
int status;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //  status = IMU.begin(); //Initializes communication with MPU9250
  Serial.begin(115200);
  //  IMU_startUp();

  pinMode(4, OUTPUT);
  delay(100);
  pinMode(4, INPUT);
  delay(2000);

  while (digitalRead(BUTTON_PIN) != LOW) {
    Serial.println("Press the button to continue");
  }
  powerOn();
  Serial.println("POWER ON");
  delay(20);
}


void loop() {
  //  IMU.readSensor();
  /*
    Serial.print(IMU.getAccelX_mss(), 6);
    Serial.print("\t");
    Serial.print(IMU.getAccelY_mss(), 6);
    Serial.print("\t");
    Serial.print(IMU.getAccelZ_mss(), 6);
    Serial.println();
  */
  // Get current button state.
  delay(20);
  boolean newState = digitalRead(BUTTON_PIN);
  while ((newState == LOW) && (oldState == HIGH)) {
    i++;
    Serial.println(i);
    newState = digitalRead(BUTTON_PIN);
  }
  oldState = newState;
  delay(20);
  //Serial.println("Done Counting!!!");
  if (i < 2500 && i != 0) {
    cycleColors();
  } else if (i > 2500) {
    powerOff();
  }
  i = 0;

  swingCheck();
  Serial.println(IMU.getAccelX_mss(), 6);
}

void powerOn() {

  pixels.begin();
  Serial.begin(115200);

  pixels.fill(pixels.Color(0, 0, 0));
  pixels.show();
  boolean poweron = false;
  while (poweron == false) {
    for (int i = 0; i < PIXEL_COUNT; i++) {
      Serial.println("condition met");
      pixels.setPixelColor(i, 0, 0, 255);
      pixels.setBrightness(50);
      pinMode(5, OUTPUT);
      pixels.show();

      if (PIXEL_COUNT > 10) { //Ignition Sound
        pinMode(5, OUTPUT);
        poweron = true;
        Serial.print("Condition Met");
      }
    }
    pinMode(5, INPUT);
    delay(500);
    pinMode(6, OUTPUT); //HUM SOUND
  }
}

void powerOff() {
  delay(20);
  pinMode(6, INPUT);
  int j = 0;
  while (j <= 144) {
    Serial.println(j);
    pixels.setPixelColor(144 - j, 0, 0, 0);
    //pixels.setBrightness(0);
    pixels.show();
    // Serial.print("POWER OFF");
    if (144 - j < 130) { //Power off sound
      pinMode(7, OUTPUT);
    }
    j++;
  }
  pinMode(7, INPUT);
}

void IMU_startUp() {
  // status = IMU.begin(); //Initializes communication with MPU9250
  // Serial.begin(115200);

  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1) {}
  }
  Serial.println("IMU START UP SUCESSFUL");
  // setting the accelerometer full scale range to +/-8G
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  // setting the gyroscope full scale range to +/-500 deg/s
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
  // setting DLPF bandwidth to 20 Hz
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);
  // setting SRD to 19 for a 50 Hz update rate
  IMU.setSrd(19);
}

void cycleColors() {
  delay(20);
  if (++mode > 9) mode = 0; // Advance to next mode, wrap around after #8
  switch (mode) {          // Start the new animation...
    case 0: //red
      pixels.fill(pixels.Color(155, 0, 0));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 1: //green
      pixels.fill(pixels.Color(0, 155, 0));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 2: //yellow
      pixels.fill(pixels.Color(255, 222, 0));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 3: //orange
      pixels.fill(pixels.Color(255, 15, 0));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 4: //cyan
      pixels.fill(pixels.Color(0, 247, 255));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 5: //purple
      pixels.fill(pixels.Color(171, 0, 255));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 6: //blue
      pixels.fill(pixels.Color(0, 0, 255));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 7: //white
      pixels.fill(pixels.Color(150, 150, 150));
      pixels.setBrightness(50);
      pixels.show();
      //Serial.println("Color Change");
      break;
    case 8: //light orange(tan)
      pixels.fill(pixels.Color(250, 99, 14));
      pixels.setBrightness(50);
      //Serial.println("Color Change");
      pixels.show();
      break;
    case 9: //yellow-orange
      pixels.fill(pixels.Color(255, 45, 0));
      pixels.setBrightness(50);
      //Serial.println("Color Change");
      pixels.show();
  }
}

void swingCheck() {
  if (abs(IMU.getAccelX_mss()) >= 1) {
    pinMode(6, INPUT);
    pinMode(9, OUTPUT);
    delay(600);
    pinMode(6, OUTPUT);
    pinMode(9, INPUT);
  }
}
