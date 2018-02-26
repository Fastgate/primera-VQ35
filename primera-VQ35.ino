#include "DigitalButton.h"
#include "TimedOutput.h"

// ******************************************************* Audi MMI 3G ********************************************* 

byte MMIbyte[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // key byte sequence
int Delay = 10; // delay in millisec
int MMIcom = 0; // bool value set 1 if comand detected
int ComID = 0;  // comand ID (for easy select case routine)
int otvet = 0;  // id otveta
int otvet2 = 0; // Keys vom Mega zum Teensy 3.5
int current2;
long millis_held2;    // How long the button was held (milliseconds)
long secs_held2;      // How long the button was held (seconds)
long prev_secs_held2; // How long the button was held in the previous check
byte previous2 = HIGH;
unsigned long firstTime2; // how long since the button was first pressed

//**************************************** USB MMI ******************************************************************

int MMI = 0;
int MediaPlayPause = 0;


//*****************************************************   System Codes   *******************************************************************

byte START_12V[8] = {0, 0x67, 0xFF, 0x1A, 0x02, 0xDF, 0xF2, 0xC5}; // Ответ пульта после подачи +12 67 FF 1A 02 DF F2 C5
byte Pult_ON[8] =   {0, 0x10, 0x02, 0x70, 0x12, 0x10, 0x03, 0xA7}; // Команда включение пульта      10 02 70 12 10 03 A7

byte Pult_ON_Otvet[8] = {0, 0x67, 0xFF, 0x2A, 0xDF, 0xF2, 0x4B, 0x00 }; // Ответ пульта после программного включения 67 FF 2A DF F2 4B 0
byte Highlight_ILL[8] = {0x10, 0x02, 0x64, 0x99, 0x01, 0x10, 0x03, 0x23}; // Highlight Beleuchtung  10 02 64 99 01 10 03 23
byte ILL_ON[8] =        {0x10, 0x02, 0x60, 0xFF, 0x10, 0x03, 0x84};       // beleuchtung all ON     10 02 60 FF 10 03 84
byte ILL_OFF[8] =       {0x10, 0x02, 0x60, 0x00, 0x10, 0x03, 0x85};       // beleuchtung all OFF    10 02 60 00 01 10 03 85


//*****************************************************   Tasten Codes   *******************************************************************
byte ScrSho[8] = { 0, 0x67, 0x3A, 0xD2, 0xBE, 0xF2, 0x3D, 0x00};            // gedrÃ¼ckt     

byte NAVI_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xF5, 0xDF, 0xF2, 0x4B};        // gedrÃ¼ckt     
byte NAVI_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xEA, 0xDF, 0xF2, 0x49};        // losgelassen  
bool status_Navi_Taste = false;   // Navi-Taste Status Variable

byte INFO_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xF3, 0xDF, 0xF2, 0x49};        // gedrÃ¼ckt              
byte INFO_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xE6, 0xDF, 0xF2, 0x47};        // losgelassen        
bool status_INFO_Taste = false;   // Info-Taste Status Variable


byte CAR_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xF1, 0xBE, 0xF2, 0x47};         // gedrÃ¼ckt      
byte CAR_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xE2, 0xBE, 0xF2, 0x45};         // losgelassen  
bool status_CAR_Taste = false;   // Car-Taste Status Variable

byte SETUP_DOWN[8] =    {0, 0x67, 0xFF, 0x3E, 0xEF, 0xBE, 0xF2, 0x45};      // gedrÃ¼ckt     
byte SETUP_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xDE, 0xBE, 0xF2, 0x43};       // losgelassen  
bool status_SETUP_Taste = false;   // Setup-Taste Status Variable

byte RADIO_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xCF, 0xDF, 0xF2, 0x25};       // gedrÃ¼ckt   
byte RADIO_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0x9E, 0xDF, 0xF2, 0x23};       // losgelassen 
bool status_RADIO_Taste = false;   // Radio-Taste Status Variable

byte MEDIA_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xFB, 0xBE, 0xF2, 0x51};       // gedrÃ¼ckt     
byte MEDIA_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xF6, 0xBE, 0xF2, 0x4F};       // losgelassen  
bool status_MEDIA_Taste = false;   // Media-Taste Status Variable

byte NAME_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xF9, 0xDF, 0xF2, 0x4F};        // gedrÃ¼ckt     
byte NAME_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xF2, 0xDF, 0xF2, 0x4D};        // losgelassen  
bool status_NAME_Taste = false;   // Name-Taste Status Variable

byte TEL_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xF7, 0xBE, 0xF2, 0x4D};         // gedrÃ¼ckt        
byte TEL_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xEE, 0xBE, 0xF2, 0x4B};         // losgelassen  
bool status_TEL_Taste = false;   // Tel-Taste Status Variable

byte LeftReturn_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xE7, 0xDF, 0xF2, 0x3D};  // gedrÃ¼ckt      
byte LeftReturn_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xCE, 0xDF, 0xF2, 0x3B};  // losgelassen   

byte Return_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xE1, 0xDF, 0xF2, 0x37};      // gedrÃ¼ckt        
byte Return_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xC2, 0xDF, 0xF2, 0x35};      // losgelassen   

byte RightReturn_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xDF, 0xBE, 0xBE, 0xF2}; // gedrÃ¼ckt       
byte RightReturn_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xBE, 0xBE, 0xBE, 0xF2}; // losgelassen   

byte Big_Left[8] =   {0, 0x67, 0xFF, 0xBA, 0xFA, 0xBE, 0xF2, 0x11};         //               
byte Big_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xFD, 0xBE, 0xF2, 0x53};         // gedrÃ¼ckt                   
byte Big_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xFA, 0xBE, 0xF2, 0x51};         // losgelassen           
byte Big_Right[8] =  {0, 0x67, 0xFF, 0xBE, 0xFD, 0xBE, 0xF2, 0x13};         //      

byte Small_Left[8] =   {0, 0x67, 0xFF, 0xFA, 0xFD, 0xBE, 0xF2, 0x31};       //                  
byte Small_Right[8] =  {0, 0x67, 0xFF, 0xFE, 0xFA, 0xBE, 0xF2, 0x33};       //                  
byte Small_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0x8F, 0xBE, 0xF2, 0xE5};       // gedrÃ¼ckt             
byte Small_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0x1E, 0xBE, 0xF2, 0xE3};       // losgelassen 
bool status_SmallDown_Taste = false;   // Small-Down-Taste Status Variable      

byte LeftUP_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xEB, 0xDF, 0xF2, 0x41};      // gedrÃ¼ckt     
byte LeftUP_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xD6, 0xDF, 0xF2, 0x3F};      // losgelassen    
bool status_LeftUP_Taste = false;   // Left-Up-Taste Status Variable

byte LeftDown_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xE9, 0xBE, 0xF2, 0x3F};    // gedrÃ¼ckt       
byte LeftDown_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xD2, 0xBE, 0xF2, 0x3D};    // losgelassen   
bool status_LeftDown_Taste = false;   // Left-Down-Taste Status Variable

byte RightUP_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xE5, 0xBE, 0xF2, 0x3B};     // gedrÃ¼ckt    
byte RightUP_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xCA, 0xBE, 0xF2, 0x39};     // losgelassen 
bool status_RightUP_Taste = false;   // Right-UP-Taste Status Variable

byte RightDown_DOWN[8] =   {0, 0x67, 0xFF, 0x3E, 0xE3, 0xBE, 0xF2, 0x39};   // gedrÃ¼ckt
byte RightDown_1_DOWN[8] = {0, 0x67, 0xFF, 0x3A, 0xC6, 0xBE, 0xF2, 0x37};   // losgelassen
bool status_RightDown_Taste = false;   // Right-Down-Taste Status Variable


//*****************************************************   Licht/Lampen Codes   **************************************************************

byte NAVI_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x05, 0x10, 0x03, 0x93};  
byte NAVI_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x05, 0x10, 0x03, 0x92}; 

byte INFO_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x06, 0x10, 0x03, 0x94};  
byte INFO_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x06, 0x10, 0x03 , 0x93};

byte CAR_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x07, 0x10, 0x03, 0x95};    
byte CAR_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x07, 0x10, 0x03, 0x94};   

byte SETUP_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x08, 0x10, 0x03, 0x96}; 
byte SETUP_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x08, 0x10, 0x03, 0x95};   

byte RADIO_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x18, 0x10, 0x03, 0xA6}; 
byte RADIO_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x18, 0x10, 0x03, 0xA5};  

byte MEDIA_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x02, 0x10, 0x03, 0x90}; 
byte MEDIA_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x02, 0x10, 0x03, 0x8F}; 

byte NAME_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x03, 0x10, 0x03, 0x91};
byte NAME_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x03, 0x10, 0x03, 0x90};

byte TEL_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x04, 0x10, 0x03, 0x92};   
byte TEL_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x04, 0x10, 0x03, 0x91};

byte LeftUP_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x0A, 0x10, 0x03, 0x98}; 
byte LeftUP_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x0A, 0x10, 0x03, 0x97}; 

byte LeftDown_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x0B, 0x10, 0x03, 0x99}; 
byte LeftDown_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x0B, 0x10, 0x03, 0x98};

byte RightUP_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x0D, 0x10, 0x03, 0x9B}; 
byte RightUP_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x0D, 0x10, 0x03, 0x9A};

byte RightDown_ON[8] =  {0x10, 0x02, 0x68, 0x01, 0x0E, 0x10, 0x03, 0x9C}; 
byte RightDown_OFF[8] = {0x10, 0x02, 0x68, 0x00, 0x0E, 0x10, 0x03, 0x9B}; 

//*************************************************  beleuchtung   ******************************************
const byte ILL_Pin = 37;                      // eingang beleuchtungsschalter 12V  OK

byte dimValue;
const byte Pin_DimUp = 45;                   // Taster Dim Heller  OK
const byte Pin_DimDown = 46;                 // Taster Dim Dunkler  OK
int bel_Pin = 21; // 12V Dim Analog

// *************************** MMI Beleuchtung Ã¤ndern ******
// ****** Info: http://www.carpc-portal.de/forum/thread-369-page-3.html
byte Beleuchtung_enable[7] =     {0x10, 0x02, 0x70, 0x12, 0x10, 0x03, 0xA7};   // aktivieren der Taster-Beleuchtung
byte Beleuchtungshelligkeit[7] = {0x10, 0x02, 0x60, 0x80, 0x10, 0x03, 0x05};   // 50% Helligkeit Startwert
byte Beleuchtung_disable[7] =    {0x10, 0x02, 0x70, 0x11, 0x10, 0x03, 0xA6};   // deaktivieren der Taster-Beleuchtung

// ************************ ENGINE START BUTTON ********************************

int unlock_led = 20; // ready to start  GREEN LED  OK
int b1 = 39; //Start button.  OK
int b2 = 16; //Clutch interlock switch.  OK
int acc = 42; //Accessory output.  OK
int ign = 44; //Ignition output.  OK
int st = 41; //Starter output.  OK
int ledPin = 24; //LED for button illumination RED LED  OK
int accLed = 5; //ACC indicator. OK
int onLed = 2; //ON indicator.  OK
int brk = 17; // Brake Switch  OK
int NATS =53;
int NATS_LED = 57;

long unsigned time3 = 0; // Timer.
long unsigned time4 = 0; //2nd timer.
int ledFlash = 0;
int ledFlash2 = 0;
int unlock_ledOn = 0;
int ledOn = 0;
int b1Stat = 0; //Was the primary switch pressed?
int b2Stat = 0; // Secondary switch pressed?
int did = 0;
int did2 = 0;
int did3 = 0;
int on = 0;
int on2 = 0;
int brkStat =0;


int current;         // Current state of the button
                     // (LOW is pressed b/c i'm using the pullup resistors)
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = HIGH;
unsigned long firstTime; // how long since the button was first pressed

// ************************** Steering Wheel Control *****************************

int wheelPin = A11; // steering wheel resistance reading pin  OK
int wheelPin2 = A10; // steering wheel resistance reading pin  OK

int prevButton_SWC=0;


//************************** Primera STW Inputs *******************************

int IGN_KEY_RLY = 43;        // Zündschloss Relais  OK
int IGN_KEY = 6;            // Zündschloss Kontakt OK
int IGN_KEY_status = 0;
int RevGear = 38;           // Rückwärtsgang  OK
int RevGear_Stat = 0;
int Handbrake_SW = 36;            // Handbremse  OK
int Handbrake_SW_Stat =0;



// ****************************** Zentralverrieglung ****************************
int ZvLock = 22;                            // Verriegeln // gedrÃ¼ckt 5V // losgelassen 0V getestet  OK
int LOCK_RLY = 27;                          // OK
int UNLOCK_RLY = 28;                        // OK
int ZvUnlock = 23;                          // Entriegeln // gedrÃ¼ckt 5V // losgelassen 0V  OK
DigitalButton zvLockButton(ZvLock, INPUT, HIGH);
DigitalButton zvUnlockButton(ZvUnlock, INPUT, HIGH);
TimedOutput lockRelay(LOCK_RLY, HIGH);
TimedOutput unlockRelay(UNLOCK_RLY, HIGH);
int fob_did = 0;



// **************************** ANDROID OTG **************************************

int Android_OTG = 5;
int OTG_status = 0;
int USB_HUB = 54;
int USB_HUB_status = 0;

// *************************** HVAC ***********************************************


int Temp = 14;                          // OPAMP für Temperaturreglung (HVAC 12)
int Geblaese = 30;                      // OPAMP für die Gebläsestärke  (HVAC 13)
int Luftklappe = 35;                    // OPAMP für die Lüftungsklappenstellung (HVAC 11)
int RR_DEF_SW = 25;                     // Taster für Heckscheibenheizung "AN" (HVAC 17)
int RR_DEF_LED = 32;                    // kontroll LED Heckscheibenheizung (HVAC 8)
int UMLUFT_SW = 26;                     // Taster für Umluftklappe  (HVAC 16)
int UMLUFT_FRE_LED = 31;                // kontroll LED Umluft (HVAC 5)
int UMLUFT_REC_LED = 29;                // kontroll LED Umluft (HVAC 6)
//int KLIMA_SW = 33;                      // Taster für A/C  (HVAC 15)
int KLIMA_LED = 47;                     // kontroll LED für Klima "AN" (HVAC 3)






 
// ******  Sketch ****** //  

void setup()
{

  Serial5.begin(9600,SERIAL_8N1);                      // verbindung zum Audi MMI 3G 
  Serial.begin(9600);                       // Serial Monitor
  Serial.println("Goodnight moon!");
  //Serial2.begin(9600);                      // verbindung zum Teensy 3.5          
    
//***************************** beleuchtung **************************************
  pinMode(Pin_DimUp, INPUT);         // mit INPUT_PULLUP invertierte Logik
  pinMode(Pin_DimDown, INPUT);       // mit INPUT_PULLUP invertierte Logik
  pinMode(ILL_Pin, INPUT_PULLUP);           // eingang beleuchtungsschalter

  digitalWrite(bel_Pin, HIGH);
  digitalWrite(ILL_Pin, HIGH);
//***************************** Engine Start Button ******************************

  
  pinMode(b1, INPUT);                // Engine Start Button
  pinMode(b2, INPUT);                // Kupplungsschalter 12V
  pinMode(acc, OUTPUT);
  pinMode(IGN_KEY_RLY, OUTPUT);
  pinMode(ign, OUTPUT);
  pinMode(st, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(unlock_led, OUTPUT);
  pinMode(accLed, OUTPUT);
  pinMode(onLed, OUTPUT);
  pinMode(brk, INPUT);
  pinMode(bel_Pin, OUTPUT);
  pinMode(NATS, OUTPUT);
  pinMode(NATS_LED, OUTPUT);
  time3 = millis();
  time4 = millis();
  
  digitalWrite(acc, HIGH);
  digitalWrite(IGN_KEY_RLY, HIGH);
  digitalWrite(ign, HIGH);
  digitalWrite(st, HIGH);
  digitalWrite(accLed, HIGH);
  digitalWrite(onLed, HIGH);
  digitalWrite(ledPin, HIGH);
  digitalWrite(brk, HIGH);
  digitalWrite(unlock_led, HIGH);
  digitalWrite(b2, LOW);
  digitalWrite(NATS, HIGH);
  digitalWrite(NATS_LED,HIGH);

// **************************** Steering Wheel Control ***********

   pinMode(wheelPin, INPUT);
   pinMode(wheelPin2, INPUT);


// *********************** Primera STW Inputs ************************

  pinMode(IGN_KEY, INPUT);
  digitalWrite(IGN_KEY, HIGH);
  pinMode(RevGear, INPUT_PULLUP);
  digitalWrite(RevGear, HIGH); 
  pinMode(Handbrake_SW,INPUT); 
  digitalWrite(Handbrake_SW, HIGH);
   
// *********************** Android OTG *******************************

pinMode(Android_OTG, OUTPUT);
pinMode(USB_HUB, OUTPUT);

// ************************* HVAC **********************************

pinMode(Temp, OUTPUT);
pinMode(Geblaese, OUTPUT);
pinMode(Luftklappe, OUTPUT);
pinMode(RR_DEF_SW, INPUT);
pinMode(RR_DEF_LED, OUTPUT);
pinMode(UMLUFT_SW, INPUT);
pinMode(UMLUFT_FRE_LED, OUTPUT);
pinMode(UMLUFT_REC_LED, OUTPUT);
//pinMode(KLIMA_SW, INPUT);
pinMode(KLIMA_LED, OUTPUT);


}
void loop()
{  
  ILL_Beleuchtung();

  Engine_Start_Button();

  

  SWC();                           // Steering Wheel Control

  FOB();                           // Funkfernbedienung

 dimValue = Taster_Dimming();          // Dimming Tasten abfragen

  //                            +- Name des zu modifizierenden 7 Byte langen Arrays
  //                            |                 +- Wert der neuen Helligkeit
  //                            |                 |     +- Indexposition des Helligkeitswertes
  //                            |                 |     |        
  aendere_Helligkeit (Beleuchtungshelligkeit, dimValue, 3);   // MMI Helligkeit Ã¤ndern  

  
  
  if (Serial.available()) {
    Serial5.write(Serial.read());
    }
  else

 
  if (Serial5.available() && MMIcom == 0) {
    ReadMMI();
    
  }
  else 
  
  {
    switch (otvet) 
    {
      case 1:
        delay (5);
        WriteMMI(Pult_ON, 8);
        break;

      case 2:
        delay(5);
        WriteMMI(Highlight_ILL, 8);
        break;

      case 3:
        delay (5);
        if (status_Navi_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(NAVI_ON, 8);
          status_Navi_Taste = true;       // Status umschalten          
          Serial.println("NAVI-ON") ;
                         
        }
        else {
          WriteMMI(NAVI_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_Navi_Taste = false;      // Status umschalten
          Serial.println("NAVI-OFF") ;
          
          
        }
        break;
        
      case 4:
        delay (5);
        if (status_INFO_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(INFO_ON, 8);
          status_INFO_Taste = true;       // Status umschalten
          Serial.println("INFO-ON") ;
        }
        else {
          WriteMMI(INFO_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_INFO_Taste = false;      // Status umschalten
          Serial.println("INFO-OFF") ;
        }
        break;
        
      case 5:
        delay (5);
        if (status_CAR_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(CAR_ON, 8);
          status_CAR_Taste = true;       // Status umschalten
          Serial.println("CAR-ON") ;
        }
        else {
          WriteMMI(CAR_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_CAR_Taste = false;      // Status umschalten
          Serial.println("CAR-OFF") ;
        }
        break;
        
      case 6:
        delay (5);
        if (status_SETUP_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(SETUP_ON, 8);
          status_SETUP_Taste = true;       // Status umschalten
          Serial.println("SETUP-ON") ;
        }
        else {
          WriteMMI(SETUP_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_SETUP_Taste = false;      // Status umschalten
          Serial.println("SETUP-OFF") ;
        }
        break;
        
      case 7:
        delay (5);
        if (status_RADIO_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(RADIO_ON, 8);
          WriteMMI(ILL_ON, 8);
          status_RADIO_Taste = true;       // Status umschalten
          Serial.println("RADIO-ON");
        }
        else {
          WriteMMI(RADIO_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_RADIO_Taste = false;      // Status umschalten
          Serial.println("RADIO-OFF") ;
        }
        break;
        
      case 8:
        delay (5);
        if (status_MEDIA_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(MEDIA_ON, 8);
          status_MEDIA_Taste = true;       // Status umschalten
          Serial.println("MEDIA-ON") ;
        }
        else {
          WriteMMI(MEDIA_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_MEDIA_Taste = false;      // Status umschalten
          Serial.println("MEDIA-OFF") ;
        }
        break;
        
      case 9:
        delay (5);
        if (status_NAME_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(NAME_ON, 8);
          status_NAME_Taste = true;       // Status umschalten
          Serial.println("NAME-ON") ;
        }
        else {
          WriteMMI(NAME_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_NAME_Taste = false;      // Status umschalten
          Serial.println("NAME-OFF") ;
        }
        break;
        
      case 10:
        delay (5);
        if (status_TEL_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(TEL_ON, 8);
          status_TEL_Taste = true;       // Status umschalten
          Serial.println("TEL-ON") ;
          //Serial2.print( otvet2 = 14, HEX);
        }
        else {
          WriteMMI(TEL_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_TEL_Taste = false;      // Status umschalten
          Serial.println("TEL-OFF") ;
        }
        break;
        
      case 11:
        delay (5);
        if (status_LeftUP_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(LeftUP_ON, 8);
          status_LeftUP_Taste = true;       // Status umschalten
          Serial.println("Links-OBEN-ON") ;
          //Serial2.print( otvet2 = 12, HEX);
        }
        else {
          WriteMMI(LeftUP_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_LeftUP_Taste = false;      // Status umschalten
          Serial.println("Links-Oben-OFF") ;
          
        }
        break;
        
      case 12:
        delay (5);
        if (status_LeftDown_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(LeftDown_ON, 8);
          status_LeftDown_Taste = true;       // Status umschalten
          Serial.println("Links-Unten-ON") ;
        }
        else {
          WriteMMI(LeftDown_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_LeftDown_Taste = false;      // Status umschalten
          Serial.println("Links-Unten-OFF") ;
        }
        break;
        
      case 13:
        delay (5);
        if (status_RightUP_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(RightUP_ON, 8);
          status_RightUP_Taste = true;       // Status umschalten
          Serial.println("Rechts-Oben-ON") ;
          //Serial2.print( otvet2 = 13, HEX);
        }
        else {
          WriteMMI(RightUP_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_RightUP_Taste = false;      // Status umschalten
          Serial.println("Rechts-Oben-OFF") ;
        }
        break;
        
      case 14:
        delay (5);
        if (status_RightDown_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(RightDown_ON, 8);
          status_RightDown_Taste = true;       // Status umschalten
          Serial.println("Rechts-Unten-ON") ;
        }
        else {
          WriteMMI(RightDown_OFF, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_RightDown_Taste = false;      // Status umschalten
          Serial.println("Rechts-Unten-OFF") ;
        }
        break;
        
      case 15:
        delay (5);
        Serial.println("Left-Return") ;
        //Keyboard.press(KEY_MEDIA_PREV_TRACK); // linke taste ganz unten
        //Keyboard.release(KEY_MEDIA_PREV_TRACK);
        break;
      case 16:
        delay (5);
        Serial.println("Return") ;
        //Keyboard.press(KEY_ESC);  // taste ganz unten in der mitte
        //Keyboard.release(KEY_ESC);
        break;
      case 17:
        delay (5);
        Serial.println("Right-Return") ;
        //Keyboard.press(KEY_MEDIA_NEXT_TRACK);  // rechte taste ganz unten
        //Keyboard.release(KEY_MEDIA_NEXT_TRACK);
        delay(5);
        //Keyboard.press(KEY_MEDIA_PLAY_SKIP);
        //Keyboard.release(KEY_MEDIA_PLAY_SKIP);
        break;
      case 18:
        delay (5);
        Serial.println("Big-Left") ;
        //Keyboard.press(MODIFIERKEY_SHIFT);    // großer drehregler nach links
        //Keyboard.press(KEY_TAB);    
        //Keyboard.release(KEY_TAB);
        //Keyboard.release(MODIFIERKEY_SHIFT);
        break;  
      case 19:
        delay (5);
        Serial.println("BIG_DOWN") ;
         {
        //Keyboard.press(KEY_ENTER); // großer drehregler
        //Keyboard.release(KEY_ENTER);
      
      
      if(MediaPlayPause == 0 && (MMI == 0x36)) { 
          //Keyboard.press(KEY_MEDIA_PLAY);
          //Keyboard.release(KEY_MEDIA_PLAY);      
          Serial.println("MEDIA_PLAY") ;
          MediaPlayPause ++;
          
        }else
        if(MediaPlayPause == 1 && (MMI == 0x36)) {
          //Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
          //Keyboard.release(KEY_MEDIA_PLAY_PAUSE);     
          Serial.println("MEDIA_PAUSE") ;
          MediaPlayPause --;
        }
        }
        break;
      case 20:
        delay (5);
        Serial.println("Big-Right") ;
        //Keyboard.press(KEY_TAB);    // großer drehregler nach rechts
        //Keyboard.release(KEY_TAB);
        break;
      case 21:
        delay (5);
        Serial.println("Volume-Down") ;
        //Keyboard.press(KEY_MEDIA_VOLUME_DEC);  // kleiner regler nach rechts
        //Keyboard.release(KEY_MEDIA_VOLUME_DEC);
        break;
      case 22:
        delay (5);
        Serial.println("Volume-Up") ;
        //Keyboard.press(KEY_MEDIA_VOLUME_INC);  // kleiner regler nach links
        //Keyboard.release(KEY_MEDIA_VOLUME_INC);
        break;
      case 23:
        delay (5);
        if (status_SmallDown_Taste == false) { // wenn vorher OFF, dann ON-Code ausfÃ¼hren
          WriteMMI(Small_1_DOWN, 8);
          status_SmallDown_Taste = true;       // Status umschalten
          Serial.println("MUTE-ON") ;
          //Keyboard.press(KEY_MEDIA_MUTE); // kleiner drehregler
          //Keyboard.release(KEY_MEDIA_MUTE);
          
        }
        else if (status_SmallDown_Taste == true){
          WriteMMI(Small_1_DOWN, 8);          // wenn vorher ON, dann OFF-Code ausfÃ¼hren
          status_SmallDown_Taste = false;      // Status umschalten
          Serial.println("MUTE-OFF") ;
          //Keyboard.press(KEY_MEDIA_MUTE); // kleiner drehregler
          //Keyboard.release(KEY_MEDIA_MUTE);
        }
        
        break;

      case 24:
         
           
        delay (5);
        break;
            

      case 0:
        // Dummy fÃ¼r Idle Zustand
        break;
        
      default:                                // fÃ¼r ungÃ¼ltige otvet  
        Serial.print(otvet); Serial.println(F(" otvet ungÃ¼ltig"));
        break; 

    }
    //Keyboard.releaseAll();
    otvet = 0;
  }



  if (MMIcom == 1)
  {
    switch (ComID) // Ð¾Ð±Ñ€Ð°Ð±Ð°Ñ‚Ñ‹Ð²Ð°ÐµÐ¼ ÐºÐ¾Ð¼Ð°Ð½Ð´Ñ‹, ÐžÑ�Ð½Ð¾Ð²Ð½Ñ‹Ðµ Case Ñ�Ñ‚Ð¾ 1 Ð¸ 2 , Ð¾Ñ�Ñ‚Ð°Ð»ÑŒÐ½Ñ‹Ðµ Ñ�Ð²Ñ�Ð·Ð°Ð½Ñ‹ Ñ� ÐºÐ½Ð¾Ð¿ÐºÐ°Ð¼Ð¸. Ð¡ÐµÐ¹Ñ‡Ð°Ñ� Ñ� Ð¸Ñ… Ð¸Ñ�Ð¿Ð¾Ð»ÑŒÐ·ÑƒÑŽ Ñ‡Ñ‚Ð¾ Ð±Ñ‹ Ð·Ð°Ð¶Ð¸Ð³Ð°Ñ‚ÑŒ Ð»Ð°Ð¼Ð¿Ð¾Ñ‡ÐºÐ¸ Ð²Ð¾Ð·Ð»Ðµ ÐºÐ½Ð¾Ð¿Ð¾Ðº.
      //   ************************************************************** Ð²Ð¿Ñ€Ð¸Ð½Ñ†Ð¸Ð¿Ðµ Ð¸Ñ… Ð¼Ð¾Ð¶Ð½Ð¾ Ð½Ðµ Ð¸Ñ�Ð¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ñ‚ÑŒ, Ð»Ð¸Ð±Ð¾ Ð¸Ñ�Ð¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ñ‚ÑŒ ÐµÑ�Ð»Ð¸ Ð½ÑƒÐ¶Ð½Ð° Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚ÐºÐ° Ð´Ð»Ð¸Ð½Ð½Ñ‹Ñ… Ð½Ð°Ð¶Ð°Ñ‚Ð¸Ð¹.
    {
      case 100:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 1:
      SerialPrintKey();
        Serial5.write(06);
        otvet = 1;
        break;

      case 2:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 2;
        break;

      case 3:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 4:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 3;
        break;

      case 5:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 6:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 4;
        break;

      case 7:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 8:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 5;
        break;

      case 9:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 10:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 6;
        break;
        
      case 11:
        SerialPrintKey();
        Serial5.write(06);

        break;

      case 12:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 7;
        break;

      case 13:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 14:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 8;
        break;

      case 15:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 16:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 9;
        break;

      case 17:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 18:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 10;
        break;

      case 19:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 20:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 15;
        break;

      case 21:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 22:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 16;
        break;

      case 23:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 24:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 17;
        break;

      case 25:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 18;
        break;

      case 26:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 27:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 19;
        break;

      case 28:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 20;
        break;

      case 29:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 21;
        break;

      case 30:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 22;
        break;

      case 31:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 32:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 23;
        break;

      case 33:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 34:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 11;
        break;

      case 35:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 36:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 12;
        break;
        
      case 37:
        SerialPrintKey();
        Serial5.write(06);
        break;
        
      case 38:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 13;
        break;

      case 39:
        SerialPrintKey();
        Serial5.write(06);
        break;

      case 40:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 14;
        break;

      case 41:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 24;
        break;

      case 42:
        SerialPrintKey();
        Serial5.write(06);
        otvet = 50;
        break;

      default:                                // fÃ¼r ungÃ¼ltige ComID  
        Serial.print(ComID); Serial.println(F(" ComID ungÃ¼ltig"));
        break; 

    }

    MMIcom = 0;
    Serial5.flush();
  }
}

void ReadMMI()
{
  byte MMIreceiveByte = Serial5.read();    //read 1-st byte Ð•Ñ�Ð»Ð¸ 67 Ñ‚Ð¾ Ñ�Ñ‚Ð¾ Ð¿ÑƒÐ»ÑŒÑ‚
  if (MMIreceiveByte == 0x67)
  {
    MMIbyte[1] = 0x67;
    for (int i = 2; i <= 7; i++)
    {
      MMIbyte[i] = Serial5.read();    //read  2,3,4,5,6,7 bytes of data   -  Ñ‡Ð¸Ñ‚Ð°ÐµÐ¼ Ñ�Ð»ÐµÐ´ÑƒÑŽÑ‰Ð¸Ðµ Ð±Ð°Ð¹Ñ‚Ñ‹
      delay(Delay);
    }

    ComID = 100; // Ð¿Ð¾ ÑƒÐ¼Ð¾Ð»Ñ‡Ð°Ð½Ð¸ÑŽ Ð½Ðµ Ð¾Ð¿Ð¾Ð·Ð½Ð°Ð½Ñ‹Ð¹ ÐºÐ¾Ð´

    if (memcmp(MMIbyte, START_12V, 8) == 0 ) {
      ComID = 1;
    }
    if (memcmp(MMIbyte, Pult_ON_Otvet, 8) == 0 ) {
      ComID = 2;
    }
    if (memcmp(MMIbyte, NAVI_DOWN, 8) == 0 ) {
      ComID = 3;
    }
    if (memcmp(MMIbyte, NAVI_1_DOWN, 8) == 0 ) {
      ComID = 4;
    }
    if (memcmp(MMIbyte, INFO_DOWN, 8) == 0 ) {
      ComID = 5;
    }
    if (memcmp(MMIbyte, INFO_1_DOWN, 8) == 0 ) {
      ComID = 6;
    }
    if (memcmp(MMIbyte, CAR_DOWN, 8) == 0 ) {
      ComID = 7;
    }
    if (memcmp(MMIbyte, CAR_1_DOWN, 8) == 0 ) {
      ComID = 8;
    }
    if (memcmp(MMIbyte, SETUP_DOWN, 8) == 0 ) {
      ComID = 9;
    }
    if (memcmp(MMIbyte, SETUP_1_DOWN, 8) == 0 ) {
      ComID = 10;
    }
    if (memcmp(MMIbyte, RADIO_DOWN, 8) == 0 ) {
      ComID = 11;
    }
    if (memcmp(MMIbyte, RADIO_1_DOWN, 8) == 0 ) {
      ComID = 12;
    }
    if (memcmp(MMIbyte, MEDIA_DOWN, 8) == 0 ) {
      ComID = 13;
    }
    if (memcmp(MMIbyte, MEDIA_1_DOWN, 8) == 0 ) {
      ComID = 14;
    }
    if (memcmp(MMIbyte, NAME_DOWN, 8) == 0 ) {
      ComID = 15;
    }
    if (memcmp(MMIbyte, NAME_1_DOWN, 8) == 0 ) {
      ComID = 16;
    }
    if (memcmp(MMIbyte, TEL_DOWN, 8) == 0 ) {
      ComID = 17;
    }
    if (memcmp(MMIbyte, TEL_1_DOWN, 8) == 0 ) {
      ComID = 18;
    }
    if (memcmp(MMIbyte, LeftReturn_DOWN, 8) == 0 ) {
      ComID = 19;
    }
    if (memcmp(MMIbyte, LeftReturn_1_DOWN, 8) == 0 ) {
      ComID = 20;
    }
    if (memcmp(MMIbyte, Return_DOWN, 8) == 0 ) {
      ComID = 21;
    }
    if (memcmp(MMIbyte, Return_1_DOWN, 8) == 0 ) {
      ComID = 22;
    }
    if (memcmp(MMIbyte, RightReturn_DOWN, 8) == 0 ) {
      ComID = 23;
    }
    if (memcmp(MMIbyte, RightReturn_1_DOWN, 8) == 0 ) {
      ComID = 24;
    }
    if (memcmp(MMIbyte, Big_Left, 8) == 0 ) {
      ComID = 25;
      }
    if (memcmp(MMIbyte, Big_DOWN, 8) == 0 ) {
      ComID = 26;
    }
    if (memcmp(MMIbyte, Big_1_DOWN, 8) == 0 ) {
      ComID = 27;
    }
    if (memcmp(MMIbyte, Big_Right, 8) == 0 ) {
      ComID = 28;
    }
    if (memcmp(MMIbyte, Small_Left, 8) == 0 ) {
      ComID = 29;
      }
    if (memcmp(MMIbyte, Small_Right, 8) == 0 ) {
      ComID = 30;
      }
    if (memcmp(MMIbyte, Small_DOWN, 8) == 0 ) {
      ComID = 31;
    }
    if(memcmp(MMIbyte, Small_1_DOWN, 8) == 0 ){
      ComID=32;
    }
  current2 = (memcmp(MMIbyte, Small_DOWN, 8));

  // if the button state changes to pressed, remember the start time 
  if (memcmp(MMIbyte, Small_DOWN, 8) == LOW  && previous2 == HIGH && (millis() - firstTime2) > 200) {
    firstTime2 = millis();
  }

  millis_held2 = (millis() - firstTime2);
  secs_held2 = millis_held2 / 1000;

  // This if statement is a basic debouncing tool, the button must be pushed for at least
  // 100 milliseconds in a row for it to be considered as a push.
  if (millis_held2 > 50) {

    if (memcmp(MMIbyte, Small_DOWN, 8) == LOW  && secs_held2 > prev_secs_held2) {
      
    }

    // check if the button was released since we last checked
    if (memcmp(MMIbyte, Small_DOWN, 8) == HIGH && previous2 == LOW) {
      // HERE YOU WOULD ADD VARIOUS ACTIONS AND TIMES FOR YOUR OWN CODE
      // ===============================================================================

      // Button pressed for less than 1 second, one long LED blink
      if (secs_held2 <= 0) {
        
      }

      // If the button was held for 3-6 seconds blink LED 10 times
      if (secs_held2 >= 1 && secs_held < 3) {
        
      }

      // Button held for 1-3 seconds, print out some info
      if (secs_held2 >= 3) {
        Serial.print("It Works!!! Seconds held: ");
        Serial.print(secs_held2);
        Serial.print("   Milliseconds held: ");
        Serial.println(millis_held2);
        
      }
      // ===============================================================================
    }
  }

  previous2 = (memcmp(MMIbyte, Small_DOWN, 8) == 0 );
  prev_secs_held2 = secs_held2;


      
      
    if (memcmp(MMIbyte, LeftUP_DOWN, 8) == 0 ) {
      ComID = 33;
    }
    if (memcmp(MMIbyte, LeftUP_1_DOWN, 8) == 0 ) {
      ComID = 34;
    }
    if (memcmp(MMIbyte, LeftDown_DOWN, 8) == 0 ) {
      ComID = 35;
    }
    if (memcmp(MMIbyte, LeftDown_1_DOWN, 8) == 0 ) {
      ComID = 36;
    }
    if (memcmp(MMIbyte, RightUP_DOWN, 8) == 0 ) {
      ComID = 37;
    }
    if (memcmp(MMIbyte, RightUP_1_DOWN, 8) == 0 ) {
      ComID = 38;
    }
    if (memcmp(MMIbyte, RightDown_DOWN, 8) == 0 ) {
      ComID = 39;
    }
    if (memcmp(MMIbyte, RightDown_1_DOWN, 8) == 0 ) {
      ComID = 40;
    }

    MMIcom = 1;
  }
  else
  {
    MMIcom = 0;
  }
  Serial5.flush();


}

void SerialPrintKey()  //Ð’Ñ‹Ð²Ð¾Ð´ ÐºÐ¾Ð¼Ð°Ð½Ð´Ñ‹ Ð² Ñ�ÐµÑ€Ð¸Ð°Ð» Ð¿Ð¾Ñ€Ñ‚ , Ñ‚ÑƒÑ‚ Ð¼Ð¾Ð¶Ð½Ð¾ Ð¿Ñ€Ð¸Ð²ÐµÑ�Ñ‚Ð¸ ÐµÐµ Ðº Ð¿Ñ€Ð¸Ð²Ñ‹Ñ‡Ð½Ð¾Ð¼Ñƒ Ð´Ð»Ñ� iCar Ð²Ð¸Ð´Ñƒ
{
  for (int x = 1; x <= 7; x++) { 
    Serial.print(MMIbyte[x], HEX);
    Serial.print(" ");
   
  }
  Serial.println();
  
}

void WriteMMI(byte Mas[], int Byte)   //  ÐŸÑ€Ð¾Ñ†ÐµÐ´ÑƒÑ€Ñ‹ Ð¾Ñ‚Ð¿Ñ€Ð°Ð²ÐºÐ¸ ÐºÐ¾Ð¼Ð°Ð½Ð´ Ð½Ð° Ð¿ÑƒÐ»ÑŒÑ‚
{
  for (int i = 0; i < Byte; i++) {
    Serial5.write(Mas[i]);
    delay(2);
    
  }
}

void ILL_Beleuchtung ()
{
  static bool old_state = false;        // letzten Tasterzustand merken

  bool new_state = digitalRead(ILL_Pin); // Tasterzustand einlesen

  if (new_state != old_state) {       // hat sich Taster/Schalter-Stellung geÃ¤ndert?
    if ( new_state == true ) {        // Licht ein- oder ausschalten ...
      WriteMMI(ILL_ON, 8); Serial.print('\t'); Serial5.println(F("ON"));
      digitalWrite(bel_Pin, LOW);
      Serial.println("BELEUCHTUNG_AN") ;
    }
    else {
      WriteMMI(ILL_OFF, 8); Serial.print('\t'); Serial.println(F("OFF"));
      //digitalWrite(ILL_Ausgang, HIGH);
      digitalWrite(bel_Pin, HIGH);
      Serial.println("BELEUCHTUNG_AUS") ;
    }
    old_state = new_state;  // aktuellen Zustand merken
    delay(20);              // Totzeit Tasterentprellung
  }
}


byte Taster_Dimming ()
{
  static unsigned long last_Time_DimUp = 0;
  static unsigned long last_Time_DimDown = 0;
  const unsigned int Entprellzeit = 30;           // Taster Entprellzeit [ms]
  static bool last_state_DimUp = true;           // INPUT_PULLUP invertierte Logik
  static bool last_state_DimDown = true;         // INPUT_PULLUP invertierte Logik
  static int DimValue = 0x80;                     // Helligkeitswert, 50% Startwert
  
  bool read_DimUp = digitalRead(Pin_DimUp);       // UP Tasterstatus einlesen
  bool read_DimDown = digitalRead(Pin_DimDown);   // DOWN Tasterstatus einlesen

  if (read_DimUp != last_state_DimUp) {           // wenn UP Tasterstatus verschieden zu vorher
    last_Time_DimUp = millis();                   // Zeit vom letzten Tastendruck merken
  }

  if (read_DimDown != last_state_DimDown) {       // wenn DOWN Tasterstatus verschieden zu vorher
    last_Time_DimDown = millis();                 // Zeit vom letzten Tastendruck merken
  }

  if ((millis() - last_Time_DimUp) > Entprellzeit) {
    if (read_DimUp == HIGH) {                      // wurde UP Taster wirklich gedrÃ¼ckt?
      DimValue = DimValue + 15;                   // Dimming Variable erhÃ¶hen
      last_state_DimUp = !last_state_DimUp;       // Status umschalten, ZÃ¤hlsperre
    }
    else {
      last_state_DimUp = !last_state_DimUp;       // Status umschalten, ZÃ¤hlsperre
    }
  }

  if ((millis() - last_Time_DimDown) > Entprellzeit) {
    if (read_DimDown == HIGH  ) {                    // wurde DOWN Taster wirklich gedrÃ¼ckt?
      DimValue = DimValue - 15;                   // Dimming Variable erniedrigen
      last_state_DimDown = !last_state_DimDown;   // Status umschalten, ZÃ¤hlsperre
    }
    else {
      last_state_DimDown = !last_state_DimDown;   // Status umschalten, ZÃ¤hlsperre
    }
  }

  if (DimValue < 45) {                             // ZÃ¤hlumfang/Wert nach unten auf 0 begrenzen
    DimValue = 45;
  }

  if (DimValue > 255) {                           // ZÃ¤hlumfang/Wert nach oben auf 255 begrenzen
    DimValue = 255;
  }
  
  return (byte)DimValue;
}


void aendere_Helligkeit (byte feld[], byte value, byte index) // Ã¤ndert direkt im Array
{
  byte Checksumme = 0;
  static byte old_value = 0;
  
  if (value == 0x10) {                            // 0x10 Problem umgehen
    value = 0x11;                                 // nur minimal heller
  }

  feld[index]= value;                             // neuen Wert an Indexpos. schreiben
  
  for (byte i=0; i<6; i++) {                      // die Index 0...5 summieren
    Checksumme = Checksumme + feld[i];            // ohne RÃ¼cksicht auf Ãœberlauf
  }
   
  feld[6]= Checksumme;                            // korrigierte PrÃ¼fsumme an letzte Stelle schreiben  

  if (value != old_value) {                       // wenn neuer Wert verschieden, dann ausgeben
    old_value = value;                            // aktuellen Wert merken
    //WriteMMI(Beleuchtung_enable, 7);      delay(5);
    WriteMMI(Beleuchtungshelligkeit, 7);  delay(5);
    //WriteMMI(Beleuchtung_disable, 7); 
    Serial5.write(06);

    // *** Debugausgabe, kann weg wenn nicht benÃ¶tigt
    Ausgabe_HEX (Beleuchtungshelligkeit, sizeof(Beleuchtungshelligkeit));  // Ausgabe/Anzeige
    Serial.println(dimValue);  
  }
}


// *** fÃ¼r Debugausgabe, kann weg wenn nicht benÃ¶tigt
void Ausgabe_HEX (byte feld[], byte laenge)       // Debugausgabe
{   
    for (byte i=0; i<laenge; i++) { 
      Serial.print(feld[i],HEX);
      Serial.print("."); 
    }
    Serial.print('\t');
   
}


void Engine_Start_Button()

{
  current = digitalRead(b1);

  if( digitalRead(IGN_KEY) ==1){
    digitalWrite(IGN_KEY_RLY, LOW);
    IGN_KEY_status = 1;
    
  // -------------------------Read the bottons:-----------
  if(digitalRead(b1) == LOW && b1Stat == 0){
    b1Stat++;
    on++;
    Serial.println("Startknopf betÃ¤tigt");
    delay(90);
  }
  if(digitalRead(b1) == HIGH && b1Stat == 1){
    b1Stat--;
    on--;
    delay(90);
  }
  
  
  if(digitalRead(b2) == HIGH && b2Stat == 0){
    b2Stat++;
    on2++;
    Serial.println("Kupplung_betÃ¤tigt") ;
    delay(90);
  }
 
  if(digitalRead(b2) == LOW && b2Stat == 1){
    b2Stat--;
    on2--;
    delay(90);
  }

  if(digitalRead(brk) == HIGH && brkStat == 0){
    brkStat++;
    Serial.println("BREMSE_BETAETIGT") ;
    delay(90);
  }

   if(digitalRead(brk) == LOW && brkStat == 1){
    brkStat--;
    delay(90);
  }

  if(digitalRead(Handbrake_SW) == LOW && Handbrake_SW_Stat == 0){
    Handbrake_SW_Stat++;
    Serial.println("Handbremse_ON");
    delay(90);
  }
  if(digitalRead(Handbrake_SW) == HIGH && Handbrake_SW_Stat == 1){
    Handbrake_SW_Stat--;
    Serial.println("Handbremse_OFF");
    delay(90);
  }

  if(digitalRead(RevGear) == HIGH && RevGear_Stat == 0){
    RevGear_Stat ++;
    Serial.println("Rückwärtsgang_ON");
    delay(90);
  }
  if(digitalRead(RevGear) == LOW && RevGear_Stat == 1){
    RevGear_Stat --;
    Serial.println("Rückwärtsgang_OFF");
    delay(90);
  }
  

    
  //--------------------Operation:---------------------------
  if(on == 1 && on2 == 1 && did == 0){ // Start engine, IGN on.
      digitalWrite(acc, LOW);
      digitalWrite(accLed, LOW);
      delay(500);
      digitalWrite(acc, HIGH);
      digitalWrite(accLed, HIGH);
      digitalWrite(NATS,LOW);
      digitalWrite(ign, LOW); //Write LOW to activate relay.
      digitalWrite(onLed, LOW);
      delay(600);
      digitalWrite(st, LOW);
      delay(600);
      //Keyboard.press(KEY_SYSTEM_WAKE_UP);  // ENGINE_BUTTON
      //Keyboard.release(KEY_SYSTEM_WAKE_UP);
            
      did = 1;
      did2 = 0;
      did3 = 1;
      ledOn = 1;
      unlock_ledOn = 1;
      Serial.println("KeyStart");
      IGN_KEY_status = 1;
    
  }
  
  if(on == 0 && did == 1){ //Starter off.
    digitalWrite(st, HIGH);
    //Serial2.print(otvet2 = 1, HEX);       //Wake UP Tablet
    did = 2;
  }
  
  if(did == 2 && on == 0){ //ACC on after engine start.
    digitalWrite(acc, LOW);
    digitalWrite(accLed, HIGH);
    digitalWrite(unlock_led, HIGH);
    
    
    
    did = 3;
  }
  
  if(did == 3 && on == 1 && brkStat == 1){  //Turn everything off.
   
    digitalWrite(ign, HIGH);
    digitalWrite(acc, HIGH);
    digitalWrite(onLed, HIGH);
    digitalWrite(accLed, HIGH);
    digitalWrite(NATS,HIGH);
    //Keyboard.press(KEY_SYSTEM_POWER_DOWN);   // ENGINE_BUTTON
    //Keyboard.release(KEY_SYSTEM_POWER_DOWN);
    
    did = 4;
    ledOn = 0;
    time4 = 0;
    unlock_ledOn = 0;
    IGN_KEY_status = 0;
  }else

  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;

  // This if statement is a basic debouncing tool, the button must be pushed for at least
  // 100 milliseconds in a row for it to be considered as a push.
  if (millis_held > 50) {

    if (current == LOW && secs_held > prev_secs_held) {
       // Each second the button is held blink the indicator led
    }

    // check if the button was released since we last checked
    if (current == HIGH && previous == LOW) {
      // HERE YOU WOULD ADD VARIOUS ACTIONS AND TIMES FOR YOUR OWN CODE
      // ===============================================================================

      // Button pressed for less than 1 second, one long LED blink
      if (secs_held <= 0) {
        
      }

      // If the button was held for 3-6 seconds blink LED 10 times
      if (secs_held >= 1 && secs_held < 3) {
        
      }

      // Button held for 1-3 seconds, print out some info
      if (secs_held >= 2 ) {
        Serial.print("It Works!!! Seconds held: ");
        Serial.print(secs_held);
        Serial.print("   Milliseconds held: ");
        Serial.println(millis_held);

        digitalWrite(ign, HIGH);
        digitalWrite(acc, HIGH);
        digitalWrite(onLed, HIGH);
        digitalWrite(accLed, HIGH);
        digitalWrite(NATS,HIGH);
        //Keyboard.press(KEY_SYSTEM_POWER_DOWN);   // ENGINE_BUTTON
        //Keyboard.release(KEY_SYSTEM_POWER_DOWN);
        
        did = 4;
        ledOn = 0;
        time4 = 0;
        unlock_ledOn = 0;
        IGN_KEY_status = 0;
      }
      // ===============================================================================
    }
  }

  previous = current;
  prev_secs_held = secs_held;

  
  
  
  if(did == 4 && on == 0){
    did = 0;
    did2 = 0;
    did3 = 0;
    IGN_KEY_status = 0;
    
  }
  //-----------------NON STARTER OPERATION--------------------
  
  if(on == 1 && did2 == 0 && did3 == 0){ //Push for ACC
    digitalWrite(acc, LOW);
    digitalWrite(accLed, LOW);
    //Keyboard.press(KEY_SYSTEM_WAKE_UP);  // ENGINE_BUTTON
    //Keyboard.release(KEY_SYSTEM_WAKE_UP);
   
    
    did2 = 1;
    ledOn = 1;
    unlock_ledOn = 1;
  }
  
  if(on == 0 && did2 == 1 && did3 == 0){
    did2 = 2;
    
  }
  
  if(did2 == 2 && on == 1 && on2 == 0){ //IGN without start
    digitalWrite(ign, LOW);
    digitalWrite(onLed, LOW);
    digitalWrite(accLed, LOW);
    digitalWrite(NATS,LOW);
    //Serial2.print(otvet2 = 1, HEX);       //Wake UP Tablet
    did2 = 3;
  }
  
  if(on == 0 && did2 == 3 && did3 == 0){
  did2 = 4;
  
  }
  
  if(did2 == 4 && on == 1 && on2 == 0){ //ALL SYSTEMS OFF.
    digitalWrite(ign, HIGH);
    digitalWrite(acc, HIGH);
    digitalWrite(accLed, HIGH);
    digitalWrite(onLed, HIGH); 
    digitalWrite(NATS,HIGH);
    //Serial2.print(otvet2 = 2, HEX);       //PowerDown Tablet
     
    did2 = 5;
    ledOn = 0;
    time4 = 0;
    unlock_ledOn = 0;
  }
  
  if(did2 == 5 && on == 0){
    did2 = 0;
    
  }
  //-----------------LED OUTPUT/BUTTON FLASH/illum.-------------------
  if(ledOn == 1 && time4 == 0){
  digitalWrite(ledPin, LOW);
    time4 = 1;
   }
   else if(millis() - time3 > 2500 && ledOn == 0 && b2Stat == 0){
    digitalWrite(ledPin, LOW);
    time3 = millis();
    ledFlash = 1;
    }
  if(millis() - time3 > 100 && ledFlash == 1 && ledOn == 0 && b2Stat == 0){
    digitalWrite(ledPin, HIGH);
    time3 = millis();
    ledFlash = 0;
   }
  if(b2Stat == 1 && b1Stat == 0 && ledOn == 0){
    digitalWrite(unlock_led, HIGH);
    delay(90);
    digitalWrite(unlock_led, LOW);
    delay(100);
   
  }else

  if(b2Stat == 0 && b1Stat == 0 && ledOn == 0){
  digitalWrite(unlock_led,HIGH);
  }
 if(b2Stat == 1 && b1Stat == 0 && unlock_ledOn == 1){
    digitalWrite(unlock_led, LOW);
    digitalWrite(ledPin, HIGH);
    
 }else
 if(b2Stat == 0 && b1Stat == 0 && unlock_ledOn == 1 ){
  digitalWrite(unlock_led,HIGH);
  digitalWrite(ledPin, LOW);
 }


 if(did == 3){
  digitalWrite(unlock_led,HIGH);
  digitalWrite(ledPin, LOW);
 }
}else
{

    digitalWrite(IGN_KEY_RLY, HIGH);
    digitalWrite(ign, HIGH);
    digitalWrite(acc, HIGH);
    digitalWrite(accLed, HIGH);
    digitalWrite(onLed, HIGH); 
    digitalWrite(ledPin, HIGH);
    digitalWrite(NATS,HIGH);
    //Keyboard.press(KEY_SYSTEM_POWER_DOWN);   // ENGINE_BUTTON
    //Keyboard.release(KEY_SYSTEM_POWER_DOWN);
   
    
  }
  
}

  
void SWC() {

static unsigned long last_Time_PHONE = 0;
  static unsigned long last_Time_Vol_UP = 0;
  static unsigned long last_Time_Vol_DOWN = 0;
  static unsigned long last_Time_SPEAK = 0;
  static unsigned long last_Time_SEEK_UP = 0;
  static unsigned long last_Time_SEEK_DOWN = 0;
  const unsigned int Entprellzeit = 40;           
  static bool last_state_PHONE = false;           
  static bool last_state_Vol_UP = false;         
  static bool last_state_Vol_DOWN = false;           
  static bool last_state_SPEAK = false;
  static bool last_state_SEEK_UP = false;           
  static bool last_state_SEEK_DOWN = false;
  
  int r= analogRead(wheelPin);
  int r2= analogRead(wheelPin2);
   bool read_Vol_UP= (r>=28 && r<=34) ;      
   bool read_Vol_DOWN= (r2>=28 && r2<=34) ;      
   bool read_PHONE= (r>=10 && r<=16) ;      
   bool read_SPEAK= (r2>=10 && r2<=16) ;      
   bool read_SEEK_UP= (r2>=15 && r2<=20) ;      
   bool read_SEEK_DOWN= (r2>=15 && r2<=20) ;      
  

  if ((r>=28 && r<=34) != last_state_Vol_UP) {           
    last_Time_Vol_UP = millis();                   
  }

  if ((r2>=28 && r2<=34) != last_state_Vol_DOWN) {           
    last_Time_Vol_DOWN = millis();                   
  }

  if ((r>=10 && r<=16) != last_state_PHONE) {          
    last_Time_PHONE = millis();                   
  }

  if ((r2>=10 && r2<=16) != last_state_SPEAK) {           
    last_Time_SPEAK = millis();                   
  }

  if ((r2>=15 && r2<=20) != last_state_SEEK_UP) {           
    last_Time_SEEK_UP = millis();                  
  }

  if ((r>=15 && r<=20) != last_state_SEEK_DOWN) {           
    last_Time_SEEK_DOWN = millis();                   
  }

 

  if ((millis() - last_Time_Vol_UP ) > Entprellzeit) {
    if (read_Vol_UP == LOW) {                      
      //Serial2.print( otvet2 = 8, HEX);                   
      Serial.println("VOLUME_UP");
      last_state_Vol_UP = !last_state_Vol_UP;       
    }
    else {
      last_state_Vol_UP = !last_state_Vol_UP;       
    }
  }

  if ((millis() - last_Time_Vol_DOWN) > Entprellzeit) {
    if (read_Vol_DOWN == LOW) {                      
      //Serial2.print( otvet2 = 7, HEX);                   
      Serial.println("VOLUME_DOWN");
      last_state_Vol_DOWN  = !last_state_Vol_DOWN;       
    }
    else {
      last_state_Vol_DOWN = !last_state_Vol_DOWN;       
    }
  }

  if ((millis() - last_Time_PHONE) > Entprellzeit) {
    if (read_PHONE == LOW) {                      
      //Serial2.print( otvet2 = 7, HEX);                   
      Serial.println("PHONE");
      last_state_PHONE = !last_state_PHONE;       
    }
    else {
      last_state_PHONE = !last_state_PHONE;       
    }
  }

  if ((millis() - last_Time_SPEAK) > Entprellzeit) {
    if (read_SPEAK == LOW) {                      
      //Serial2.print( otvet2 = 7, HEX);                   
      Serial.println("SPEAK");
      last_state_SPEAK = !last_state_SPEAK;       
    }
    else {
      last_state_SPEAK = !last_state_SPEAK;       
    }
  }

  if ((millis() - last_Time_SEEK_UP) > Entprellzeit) {
    if (read_SEEK_UP == LOW) {                      
      //Serial2.print( otvet2 = 5, HEX);                   
      Serial.println("SEEK-UP");
      last_state_SEEK_UP = !last_state_SEEK_UP;       
    }
    else {
      last_state_SEEK_UP = !last_state_SEEK_UP;       
    }
  }

  if ((millis() - last_Time_SEEK_DOWN) > Entprellzeit) {
    if (read_SEEK_DOWN == LOW) {                      
      //Serial2.print( otvet2 = 4, HEX);                   
      Serial.println("SEEK-DOWN");
      last_state_SEEK_DOWN = !last_state_SEEK_DOWN;       
    }
    else {
      last_state_SEEK_DOWN = !last_state_SEEK_DOWN;       
    }
  }
 return;

}

void FOB(){
  zvLockButton.update();
  zvUnlockButton.update();
  lockRelay.update();
  unlockRelay.update();

  

   
  
 

  if (zvUnlockButton.wasPressedTimes(1)) {
    Serial.println("AufschlieÃŸen.");
    unlockRelay.write(LOW, 100);
    digitalWrite(Android_OTG, HIGH);
    OTG_status = 1;
  }
  if (zvUnlockButton.wasPressedTimes(3)) {
    Serial.println("AufschlieÃŸen und Fenster auf.");
    unlockRelay.write(LOW, 4000);
  }
  if (zvLockButton.wasPressedTimesOrMore(4)) {
    // TODO: CODE FUER 15 MINUTEN MOTORSTART
    if(fob_did == 0){ // Start engine, IGN on.
      digitalWrite(IGN_KEY_RLY, LOW);
      digitalWrite(acc, LOW);
      digitalWrite(accLed, LOW);
      delay(500);
      digitalWrite(NATS,LOW);
      digitalWrite(acc, HIGH);
      digitalWrite(accLed, HIGH);
      digitalWrite(ign, LOW); //Write LOW to activate relay.
      digitalWrite(onLed, LOW);
      delay(600);
      digitalWrite(st, LOW);
      delay(600);
      
            
      fob_did = 1;
      
      unlock_ledOn = 1;
      Serial.println("FOB Start");
      
      IGN_KEY_status = 1;
    
  }
  
  if(on == 0 && fob_did == 1){ //Starter off.
    digitalWrite(st, HIGH);
    //Serial2.print(otvet2 = 1, HEX);       //Wake UP Tablet
    fob_did = 2;
  }
  
  if(fob_did == 2 && on == 0){ //ACC on after engine start.
    digitalWrite(acc, LOW);
    digitalWrite(accLed, HIGH);
    //Serial2.print(otvet2 = 1, HEX);       //Wake UP Tablet
    digitalWrite(unlock_led, HIGH);
    
    
    
    fob_did = 3;
  }

  }
  if(zvUnlockButton.wasPressedTimesOrMore(1)){
    if(fob_did == 3) {
  
       
   
    digitalWrite(ign, HIGH);
    digitalWrite(acc, HIGH);
    digitalWrite(IGN_KEY_RLY, HIGH);
    digitalWrite(onLed, HIGH);
    digitalWrite(accLed, HIGH);
    digitalWrite(NATS,HIGH);
    //Serial2.print(otvet2 = 2, HEX);       //PowerDown Tablet
    
    fob_did = 0;
    unlock_ledOn = 0;
    IGN_KEY_status = 0;
  
    }
  
  }
  if (zvLockButton.wasPressedTimes(3)) {
    Serial.println("AbschlieÃŸen und Fenster zu.");
    lockRelay.write(LOW, 6000);
  }

  if (zvLockButton.wasPressedTimesOrMore(4)) {
     digitalWrite(Android_OTG, LOW);
     OTG_status = 0;
  }
}


