#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char PhoneNoOwner[]="+919322319362";
//char PhoneNoOwner[]="+919657027366";

//char PhoneNoOwner[] = "+917744867737";
int DialStatus = 0;

//---------------For GPRS Part

char Input[200];

int tmp = 0;
String serialResponse = "";
String arr[22];

int cnt = 0;
int Tick = 0;

int8_t answer;
int onModulePin = 2;
char aux_str[129];

float w = 0;

int smsdone = 0;
int calldone = 0;

//---------------


int mode = 0;


void setup() {
  //Clear the Display

  pinMode(13, INPUT_PULLUP);

  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);  //Ignition key

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("--Wheel Jammer--");
  lcd.setCursor(0, 1);
  lcd.print("----Tracker----");
  // SendDetectedSMS();



  //     sprintf(aux_str, "AT+CFUN=1");
  //    sendATcommand(aux_str, "OK", 2000);
}

void loop() {

  //SendHelpSMS();
  if (digitalRead(2) == LOW) {

    if (digitalRead(13) == LOW) {

      lcd.setCursor(0, 0);
      lcd.print("Jammer Activated");
      lcd.setCursor(0, 1);
      lcd.print("---Sending SMS---");

      SendDetectedSMS();

      //lcd.setCursor(0,0);  lcd.print("Jammer Activated");
      //lcd.setCursor(0,1);  lcd.print("----SMS Sent----");
    } else {

      lcd.setCursor(0, 0);
      lcd.print("Jammer Activated");
      lcd.setCursor(0, 1);
      lcd.print("-Uploading Data-");

      UploadData();

    }

    lcd.setCursor(0, 0);
    lcd.print("--Wheel Jammer--");
    lcd.setCursor(0, 1);
    lcd.print("----Tracker----");

  }
}



void SendDetectedSMS() {
  Serial.begin(9600);  //Initialize a serial communication with baud rate 9600
  delay(1000);
  Serial.println("AT+CMGF=1");  //Initialize the GSM modem
  delay(2000);
  Serial.print("AT+CMGS=\"");  //Send dial a phone AT command
  Serial.print(PhoneNoOwner);  //Send SMS receiver's phone number
  Serial.write(0x22);          //Hex code equivalent to "
  Serial.write(0x0D);          //Hex code equivalent to carraige return i.e. \r
  Serial.write(0x0A);          //Hex code equivalent to new line char i.e. \n
  delay(2000);

  Serial.print("Wheel Jammer Activated at : https://www.google.com/maps/search/?api=1&query=21.09698271377966,79.15201760823251");  //Test SMS Message Body to Send

  delay(500);
  Serial.println(char(26));
}




void UploadData() {

  Serial.begin(9600);  //Initialize a serial communication with baud rate 9600
  delay(1000);

  //Enable bearer 1
  sprintf(aux_str, "AT");
  sendATcommand(aux_str, "OK", 2000);


  //Check the registration status
  while ((sendATcommand("AT+CGREG?", "+CGREG: 0,1", 500) || sendATcommand("AT+CGREG?", "+CGREG: 0,5", 500)) == 0);

  //Enable bearer 1
  sprintf(aux_str, "AT+SAPBR=1,1");
  sendATcommand(aux_str, "OK", 2000);

  //Check whether bearer 1 is open.
  sprintf(aux_str, "AT+SAPBR=2,1");
  sendATcommand(aux_str, "OK", 2000);
  delay(6000);

  //Initialize HTTP service
  sprintf(aux_str, "AT+HTTPINIT");
  sendATcommand(aux_str, "OK", 2000);

  //Set the context ID
  sprintf(aux_str, "AT+HTTPPARA=\"CID\",1");
  sendATcommand(aux_str, "OK", 2000);


  //http://data.iomatic.in/StoreSensorReading.aspx?VAL=2222*35*60*40*40

  //app.iomatic.in/UploadData.aspx?VAL=IA12345*nilesh.sahare@gmail.com*f31742c0-6e7c-44f2-b65c-542c2e203e2d*50*1*0*0

  Serial.print("AT+HTTPPARA=\"URL\",\"http://projects.bliinc.in/WheelJammer/UploadData.aspx?VAL=IA12345*0*0*");

  Serial.print("1");
  Serial.print("*");
  Serial.print("0");
  Serial.print("*");
  Serial.print("0");
  Serial.print("*");
  Serial.print("0");
  Serial.println("\"");

  delay(2000);

  //Set up the HTTP action
  sprintf(aux_str, "AT+HTTPACTION=0");
  sendATcommand(aux_str, "OK", 2000);

  //Do a HTTP read
  sprintf(aux_str, "AT+HTTPREAD");
  sendATcommand(aux_str, "OK", 2000);

  //Wait for the HTTP response
  delay(6000);

  //Terminate the HTTP service
  sprintf(aux_str, "AT+HTTPTERM");
  sendATcommand(aux_str, "OK", 2000);
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);  // Initialice the string
  delay(100);

  while (Serial.available() > 0) Serial.read();  // Clean the input buffer
  Serial.println(ATcommand);                     // Send the AT command

  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the answer
    if (Serial.available() != 0) {
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer) != NULL) {
        //Serial.println(response);
        answer = 1;
      }
    }
    // Waits for the answer with time out
  } while ((answer == 0) && ((millis() - previous) < timeout));
  return answer;
}
