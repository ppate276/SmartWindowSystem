

// constants won't change. They're used here to set pin numbers:
const int button1Pin = 2;     // the number of the pushbutton1 pin
const int button2Pin = 4;     // the number of the pushbutton2 pin
const int relay1Pin =  7;      // the number of the Realy1 pin
const int relay2Pin =  8;      // the number of the Relay2 pin
const int irPin = 10;         // the number of the IR pin
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
const int ledOpen = 11;       // the number of the open LED
const int ledClose = 12;      // the number of the close LED



// variables will change:
int button1State = 0;             // variable for reading the pushbutton status
int button2State = 0;         // variable for reading the pushbutton status
int rainFlag = 0;             // variable for sensing rain status
int windowFlag = 0;           // variable for global checking if windows is open or closed
int irSensorState = 1;          // variable for reading the IR sensor
int lastState = 0;            // variable for checking IR sensor connection
int btState = -1;              // variable for bluetooth comms
int btSet = -1;                // variable for comparing and prioritizing sensor info
int coState = 0;              // variable for CO state
int irSet = 0;                // variable for opening window for obstruction

void setup() {
  //start serial connection
  Serial.begin(9600);

  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  //initialize the IR pin as a input:
  pinMode(irPin, INPUT);
  digitalWrite(irPin, HIGH); // turn on the pullup

  // initialize the relay pin as an output:
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);

  //initialize the LED pins as an output:

  pinMode(ledOpen, OUTPUT);
  pinMode(ledClose, OUTPUT);

}


void loop() {
  // read the state of the pushbutton values:
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);

  //********************************************************************************************
  // read the sensor on analog A0:
  int sensorReading = analogRead(A0);
  // map the sensor range (four options):
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

  // range value:
  switch (range) {
    case 0:    // Sensor getting wet
      
      if (windowFlag == 1) {
        Serial.println("Flood");
        rainFlag = 1;
        //Serial.println(rainFlag);
      }
      else {

        rainFlag = 0;

      }
      break;

    case 1:    // Sensor getting wet
      
      if (windowFlag == 1) {
        //Serial.println("Rain Warning");
        rainFlag = 1;

      }
      else {

        rainFlag = 0;

      }
      break;

    case 2:    // Sensor dry
      //Serial.println("Not Raining");
      rainFlag = 0;
      break;
  }
  //********************************************************************************************
  // check if the pushbutton1 is pressed.
  // if it is, the buttonState is HIGH:
  // we also ensure that the other button is not pushed to avoid conflict
  // Open Window (Button 1)
  if ((button1State == HIGH && button2State == LOW) || coState > 400 || btSet == 1 || irSet == 1)
  {
    if (windowFlag == 0) {
      digitalWrite(ledOpen, HIGH);
      Serial.println("Button1Pressed: Opening Window");
      // turn relay1 on:
      digitalWrite(relay1Pin, LOW);
      //delay(57000);
      delay(10000);
      digitalWrite(relay1Pin, HIGH);
      windowFlag = 1;
      btSet = -1;
      btState = -1;
      irSet = 0;
      digitalWrite(ledOpen, LOW);
    }

  }

  // repeat the same procedure for the second pushbutton
  // Close Window (Button 2)
  if ((button1State == LOW && button2State == HIGH) || btSet == 0 || rainFlag == 1 ) {
    if (windowFlag == 1) {
      digitalWrite(ledClose, HIGH);
      Serial.println("Button2Pressed: Closing window");
      // turn relay2 on:
      digitalWrite(relay2Pin, LOW);
      //delay(55000);
      delay(1000);

      int n = 1605;

      while (n > 0) {

        // read the state of the ir value:
        n--;
        irSensorState = digitalRead(irPin);

        if (irSensorState == LOW) {

          digitalWrite(ledClose, LOW);
          irSet = 1;
          break;
          

        }

        Serial.println(n);

      }
      digitalWrite(ledClose, LOW);
      windowFlag = 0;

    }


    digitalWrite(relay2Pin, HIGH);
    windowFlag = 0;
    btSet = -1;
    btState = -1;
  }

  //********************************************************************************************



  // read the state of the ir value:
  irSensorState = digitalRead(irPin);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:

  if (irSensorState && !lastState) {
    //Serial.println("Unbroken");
  }
  if (!irSensorState && lastState) {
    //Serial.println("Broken");
  }




  //********************************************************************************************
  // CO sensor
  coState = analogRead(A1);
  Serial.println(coState, DEC);



  //********************************************************************************************

  if (Serial.available() > 0) {

    btState = Serial.read();

  }

  if (btState == '0') {

    if (windowFlag == 1) {
      btSet = 0;
    }
  }

  if (btState == '1') {

    if (windowFlag == 0) {
      btSet = 1;
    }


  }

  //Serial.println(btSet);

}

