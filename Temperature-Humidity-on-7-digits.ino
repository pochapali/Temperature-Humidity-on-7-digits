//YOU HAVE TO INSTALL THIS LIBRARY: https://github.com/claucambra/Arduino_KYX5461AS


#include <KYX5461AS.h> 
KYX5461AS screen1(12, 8, 5, 3, 2, 11, 6, 4, 13, 10, 9, 7); // Creating object with which I will control the display, the arguments are numbers of pins connected to specific pins of the display
// for reference: https://github.com/claucambra/Arduino_KYX5461AS
unsigned long previousMillis = 0UL;
unsigned long interval = 5000UL; // This is an interval that defines how often the Arduino will check temp/humidity and update display
boolean isTemp = true; //defines if it's time to display the temperature or Humidity

int DHpin = 14; // input/output pin for temperature/humidity sensor
int dat[5];
byte read_data()
{
  byte i = 0;
  byte result = 0;
  for (i = 0; i < 8; i++) {
    while (digitalRead(DHpin) == LOW); // wait 50us
    delayMicroseconds(30); //The duration of the high level is judged to determine whether the data is '0' or '1'
    if (digitalRead(DHpin) == HIGH)
      result |= (1 << (8 - i)); //High in the former, low in the post
    while (digitalRead(DHpin) == HIGH); //Data '1', waiting for the next bit of reception
  }
  return result;
}
void start_test()
{
  digitalWrite(DHpin, LOW); //Pull down the bus to send the start signal
  delay(30); //The delay is greater than 18 ms so that DHT 11 can detect the start signal
  digitalWrite(DHpin, HIGH);
  delayMicroseconds(40); //Wait for DHT11 to respond
  pinMode(DHpin, INPUT);
  while (digitalRead(DHpin) == HIGH);
  delayMicroseconds(80); //The DHT11 responds by pulling the bus low for 80us;

  if (digitalRead(DHpin) == LOW)
    delayMicroseconds(80); //DHT11 pulled up after the bus 80us to start sending data;
  for (int i = 0; i < 5; i++) //Receiving temperature and humidity data, check bits are not considered;
    dat[i] = read_data();
  pinMode(DHpin, OUTPUT);
  digitalWrite(DHpin, HIGH); //After the completion of a release of data bus, waiting for the host to start the next signal
}
void setup()
{
  pinMode(DHpin, OUTPUT);
  start_test();
}
void loop()
{

  unsigned long currentMillis = millis();
  if (isTemp) {
    char numsToPrint[] = {dat[2] / 10 + '0', (dat[2] % 10) + '0', dat[3] + '0', 'C'};
    bool decimalsToShow[] = {false, true, false, false};
    screen1.printAll(numsToPrint, decimalsToShow);
  }
  else {
    char numsToPrint[] = {dat[0] / 10 + '0', (dat[0] % 10) + '0', dat[1] + '0', 'H'};
    bool decimalsToShow[] = {false, true, false, false};
    screen1.printAll(numsToPrint, decimalsToShow);
  }
  if (currentMillis - previousMillis > interval)
  {
    start_test();
    isTemp = !isTemp;
    screen1.reset();
    previousMillis = currentMillis;
  }
}
