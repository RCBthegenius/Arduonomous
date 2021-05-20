# Arduonomous
Arduino Controlled Autonomous Rover
Created by Reuben Buckley on the 21st May 2021
For Version V1.3.3

How to use:
1) Download the files
2) Open the .ino file in the Arduino IDE (If you don't have the Arduino IDE, you can get it here: https://www.arduino.cc/en/software)
3) When the file opens, it will ask if you want it to create a folder with [.ino filename] as it's name, click yes as arduino requires this.
4) You will need to download the IRremote library if you haven't already got it (SoftwareSerial is downloaded by default).
   At the top of the Arduino window, go to: Sketch, Include Library, then scroll to the top of the menu, and click Manage Libraries...
   wait for the Library manager to open, then type in the search box at the top of the library manager: IRremote, then press enter to search.
   Find IRremote, then install it. When it's installed, close the Library manager.
5) Upload the code to your Arduino controlled rover. Use pinout.txt for the pin usage info. Use Commandlist.txt for a list of commands that Ardu rover accepts
