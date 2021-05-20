//version:1.3.3 (Motor Trim)
//Created by Reuben Colin Buckley on the 10th May 2021
//Last updated on the 21st May 2021


//About this program
//Early Versions of this ran off a Device state system, where the rover would "check off" a bunch of 'while' commands
//by changing a variable integer called 'devstate' everytime a certain criteria/goal was met (for example, when the
//vehicle recived an ir input, it would change the devstate and that would allow the next section of code to run) this
//would almost always set devstate to 'devstate'++ and thus the next while statement would read true the next time
//the code looped. This allowed the actual loop to run as many times per second as possible and meant the code had a
//near zero latency. Update 1.3 expanded on this todo list system, by having 90% of the orperations as a callable
//function (I.e, void MyFunction(){}, then called as: My Function();), meaning that now instead of only being able
//to run through once, the code becomes entirly modular and allows for quick expansion as well as allowing functions
//to be called multiple times, allowing more complex tasks to be done while using less device memory.

//#include <SPI.h>
//#include <SD.h>             //warning #include<SD.h> is 609 bytes!
#include <SoftwareSerial.h>
#include <IRremote.h>

int devstate = 0;

int Reset = 8;

String devversion = "1.3.3";

String batlvl = "N/a";

int sensedata = 0;

String devname = "Soil Moisture Puck";

int cmdln;

String command;

SoftwareSerial BT(3, 4);       //RX pin | TX pin

const int turntime = 5;          //trims how long rover takes to turn 1 degree

//Motor A
const int motorPin1  = 9;
const int motorPin2  = 10;
//Motor B
const int motorPin3  = 6;
const int motorPin4  = 5;
int EN1 = 11;
int EN2 = 12;

unsigned long StartTime = 0;        //resets the log clock
unsigned long DriveTime = 0;        //resets the Drive clock

const int RECV_PIN = 7;             //defines IR reciver pin
IRrecv irrecv(RECV_PIN);
decode_results results;

//File datalog;
//File devcache;

//Functions
void Mstp()                         //Stop Motors Function
{
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
}

void Mfwd()                         //Drive Forward Function
{
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  Mtrim();
}

void Mtrim()                        //Differential Steering Trim, helps the rover drive straight
{
  int counter = 0;
  int right = 10;
  int left = 20;
  
    delay(1);
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, HIGH);
    if(right > left){
      delay(left);
      digitalWrite(EN1, LOW);
      delay(right - left);
      digitalWrite(EN2, LOW);
    }
    else if(right == left){
      delay(right);
      digitalWrite(EN2, LOW);
      delay(left - right);
      digitalWrite(EN1, LOW);
    }
    else if(right < left){
      delay(right);
      digitalWrite(EN1, LOW);
      digitalWrite(EN2, LOW);
    }
}

void reboot()                       //Restarts the rover so it can get sent out freash. Had issues with this causing a reset when booting as there's no way to stop the arduino setting the pins to high breifly on startup, hence this is disconnected on the rover and only serves to stop the code at the moment.
{
  Serial.println("Restarting");
  BT.println("Restarting");
  delay(1000);
  digitalWrite(Reset, HIGH);
}

void get_name()                     //This function is for the use of advanced data recieving, which requires intergers decoded into strings of data. Possible future updates will make use of this function
{

}

void irinput()                      //When the rover recieves an IR signal, it runs this function.
{
  sensedata = results.value;
  Serial.println(sensedata, HEX);
  BT.println(sensedata, HEX);
  irrecv.resume();
  Serial.print("Device_STATE:");
  Serial.println(devstate);
  //BT.print("Device_STATE:");
  //BT.println(devstate);

  if (sensedata > 0) {
    Serial.read();
    BT.read();
    Serial.println("Data saved");
    BT.println("Data saved");

  }
}

void get_data()                    //This function recalls the data collected and displays it over the serial monitor and over the bluetooth terminal along with some presets that in future could be determined by the Soil moisture puck (name, battery level, etc)
{
  Serial.println("");
  Serial.println("Retrieving Latest Data...");
  Serial.println("");
  BT.println("");
  BT.println("Retrieving Latest Data...");
  BT.println("");
  delay(500);
  if (sensedata == 0) {
    Serial.println("NO_DATA!");
    BT.println("NO_DATA!");
  }
  else
  {
    get_name();
    Serial.println("");
    Serial.println("Data found");
    Serial.println("");
    Serial.println("-----------------------");
    Serial.println("| " + devname + "  |");
    Serial.print("| Battery level: ");
    Serial.print(batlvl);
    Serial.println("% |");
    Serial.print("|      Data: ");
    Serial.print(sensedata);
    Serial.println("     |");
    Serial.println("-----------------------");
    Serial.println(results.value, HEX);
    BT.println("");
    BT.println("Data found");
    BT.println("");
    BT.println("-----------------------");
    BT.println("| " + devname + "  |");
    BT.print("| Battery level: ");
    BT.print(batlvl);
    BT.println("% |");
    BT.print("|      Data: ");
    BT.print(sensedata);
    BT.println("     |");
    BT.println("-----------------------");
    BT.println(results.value, HEX);
  }
  
}

void pingconnected()                //Other than checking to see if the rover is still on talking terms with the bluetooth terminal, it doesn't do anything else
{
  Serial.println("");
  Serial.println("One day this will actually do something");
  Serial.println("");
  BT.println("");
  BT.println("One day this will actually do something");
  BT.println("");
}

void search()                       //When this is run, all the rover sees is GO GO GO! But seriously, this function tells the rover to start searching for the soil moisture puck
{
  devstate = 1;               //sets the device state to search mode
  Serial.println("Searching...");
  BT.println("Searching...");
  DriveTime = millis();        //resets the Drive clock
  Serial.println("");
  Serial.print("Device_STATE:");
  Serial.println(devstate);
  BT.println("");
  //BT.print("Device_STATE:");
  //BT.println(devstate);
}

void commandpromt()
{
  Serial.println("testing");
  if (BT.available())
  {
    Serial.println("tester");
    command = BT.readStringUntil('\n');

    if (command.equals("/Ardu Start")) {
      search();
    }
    else if (command.equals("/Ardu Reboot")) {
      reboot();
    }
    else if (command.equals("/Ardu Ping Me")) {
      pingconnected();
    }
    else if (command.equals("/Ardu Show Data")) {
      get_data();
      devstate = 5;
    }
    else if (command.equals("/Ardu Rest")) {
      devstate = 5;
    }
    else if (command.equals("/Ardu Name")) {
      BT.println(devname);
      Serial.println(devname);
    }
    else if (command.equals("/Ardu Rover Version")) {
      BT.println(devversion);
      Serial.println(devversion);
    }
    else {
      Serial.println("Invalid command:");
      Serial.println(command);
      Serial.println("");
      BT.println("Invalid command:");
      BT.println(command);
      BT.println("");
    }
  }
}

//Styles
void title()
{
  Serial.println("--------------------------------");
  Serial.println("|     ArduDATA Rover 1.3.3     |");
  Serial.println("| Project by Reuben C. Buckley |");
  Serial.println("--------------------------------");
  BT.println("--------------------------------");
  BT.println("|     ArduDATA Rover 1.3.3     |");
  BT.println("| Project by Reuben C. Buckley |");
  BT.println("--------------------------------");
}

void initializing()
{
  Serial.println("________________________________________________________________");
  Serial.println("--------------------------");
  Serial.print("| Initializing");
  Serial.print(".");
  BT.println("________________________________________________________________");
  BT.println("--------------------------");
  BT.print("| Initializing");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.print(".");
  BT.print(".");
  delay(500);
  Serial.println(". |");
  Serial.println("--------------------------");
  Serial.println("");
  BT.println(". |");
  BT.println("--------------------------");
  BT.println("");

  //  datalog = SD.open("datalog.txt", FILE_WRITE);

  //  if (datalog)
  //  {
  //    Serial.println("Logging...");
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  BT.println(ElapsedTime);
  BT.println("Initialized");
  BT.println("SYS_OK");
  BT.println("Awaiting Command");
  //BT.print("Device_STATE: ");
  //BT.println(devstate);
  Serial.println(ElapsedTime);
  Serial.println("SYS_OK");
  Serial.println("");
  Serial.println("Initialized");
  Serial.print("Device_STATE: ");
  Serial.println(devstate);
  //    datalog.close();
  Serial.println("Datalogged");
  Serial.println("");

  // }

}

void setup()
{
  Serial.begin(9600);
  BT.begin(9600);
  title();
  //  SD.begin(3);

  //  if (!SD.begin(3))
  //  {
  //    Serial.println("SD failed to initiatilze, please check the connection");
  //    while (1);
  //  }

  //  Serial.println("SD initialized");

  //  if (SD.exists("datalog.txt"))
  //  {
  //    Serial.print("Found file: ");
  //    Serial.println(datalog);
  //  }
  //  else {
  //    Serial.print("Creating file: ");
  //    Serial.println(datalog);
  //  }

  //  datalog = SD.open("datalog.txt", FILE_WRITE);

  //  if (datalog)
  //  {
  //    Serial.println("Logging...");
  //    unsigned long CurrentTime = millis();
  //    unsigned long ElapsedTime = CurrentTime - StartTime;
  //    datalog.println(ElapsedTime);
  //    datalog.println("Log_begin");
  //    Serial.println(ElapsedTime);
  //    Serial.println("Log_begin");
  //    datalog.close();
  //    Serial.println("Datalogged");
  //  }
  //Set pins as motor outputs
  pinMode(Reset, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  digitalWrite(Reset, LOW);
  irrecv.enableIRIn();
  //irrecv.blink13(true);

  delay(100);                 // Wait for 100 millisecond(s)

  initializing();


}

void loop()
{
  while (BT.available() == 0) { } // There really is nothing between the {} braces
  //if(cmdln == "/")
  //{
  commandpromt();

  //}
  while (devstate == 1)
  {
    //this will start motors
    Mfwd();

    if (irrecv.decode(&results)) {
      irinput();


      //this will stop motors
      Mstp();

      devstate = 2;
    }

  }
  while (devstate == 2)
  {
    unsigned long CurrentTime = millis();
    unsigned long ElapsedTime = CurrentTime - DriveTime;
    Serial.println(ElapsedTime);
    Serial.print("Device_STATE:");
    Serial.println(devstate);
    Serial.print("Driving ");
    Serial.print(ElapsedTime);
    Serial.println("  milliseconds to home location.");
    BT.println(ElapsedTime);
    //BT.print("Device_STATE:");
    //BT.println(devstate);
    BT.print("Driving ");
    BT.print(ElapsedTime);
    BT.println("  milliseconds to home location.");

    Serial.println("Turning to go home");
    BT.println("Turning to go home");
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, HIGH);
    delay(turntime * 180);        //turntime is the trim adjustment for how long it takes the rover to turn 1 degree in it's current configuration

    Serial.println("Homing...");
    BT.println("Homing...");
    Mfwd();
    delay(1000);

    //this will start the motors
    Mfwd();

    Serial.print("Device_STATE:");    //Checks in device state (System heartbeat)
    Serial.println(devstate);
    //BT.print("Device_STATE:");    //Checks in device state (System heartbeat)
    //BT.println(devstate);

    delay(ElapsedTime);           //this will drive the rover in a straight line for the same amount of time it took to get the data

    Serial.println("Homed");
    BT.println("Homed");
    Mstp();

    delay(1000);

    Serial.println("Resting here for data transfer");
    BT.println("Resting here for data transfer");
    devstate = 3;
    Serial.print("Device_STATE:");
    Serial.println(devstate);
    BT.print("Device_STATE:");
    BT.println(devstate);
  }
  while (devstate == 3)
  {
    commandpromt();             //sets the device to command promt mode
  }


  //while (devstate == 4)           //legacy state, not used from 1.3.1 onwards
  //{
  //  Serial.print("Device_STATE:"); //Reads out the data that was found (viewed through serial monitor)
  //  Serial.println(devstate);
  //  Serial.println("Retrieving Latest Data");
  //  delay(500);
  //  if (results.value > 0)
  //  {
  //
  //    Serial.read();
  //    Serial.println("Data found");
  //    Serial.println(results.value, HEX);
  //  }
  //  else {
  //    Serial.println("NO_DATA!");
  //  }
  //  devstate = 5;              //sets the device state to rest mode

  //}

  while (devstate == 5)
  {
    Serial.println("Device is at rest");
    Serial.println(devstate);
    Serial.println("press any key to see latest data");
    Serial.println("");
    BT.println("Device is at rest");
    BT.println("Use BT commands to do actions");
    BT.println("");
    delay(5000);

    commandpromt();

    //  if (BT.available())      //Repeats Data readout if something is sent over the serial monitor
    //  {
    //    BT.read();
    //    if (results.value > 0)
    //    {

    //      Serial.read();
    //      Serial.println("Data found");
    //      Serial.println(results.value, HEX);
    //    }
    //    else {
    //      Serial.println("NO_DATA!");
    //    }
    //  }

  }

}
