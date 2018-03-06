#include "Button.h"
#include "TimedOutput.h"
#include "Mmi.h"

  /////////////////////
 // MMI DEFINITIONS // 
/////////////////////

// Buttons
Mmi mmi(&Serial5, SERIAL_8N2_RXINV, 16, 17, 2);
MmiButton *mmiBigWheelButton    = mmi.createButton(0x01);
MmiButton *mmiMediaButton       = mmi.createButton(0x02);
MmiButton *mmiNameButton        = mmi.createButton(0x03);
MmiButton *mmiTelButton         = mmi.createButton(0x04);
MmiButton *mmiNavButton         = mmi.createButton(0x05);
MmiButton *mmiInfoButton        = mmi.createButton(0x06);
MmiButton *mmiCarButton         = mmi.createButton(0x07);
MmiButton *mmiSetupButton       = mmi.createButton(0x08);
MmiButton *mmiTopLeftButton     = mmi.createButton(0x0A);
MmiButton *mmiBottomLeftButton  = mmi.createButton(0x0B);
MmiButton *mmiPreviousButton    = mmi.createButton(0x0C);
MmiButton *mmiTopRightButton    = mmi.createButton(0x0D);
MmiButton *mmiBottomRightButton = mmi.createButton(0x0E);
MmiButton *mmiReturnButton      = mmi.createButton(0x0F);
MmiButton *mmiNextButton        = mmi.createButton(0x10);
MmiButton *mmiRadioButton       = mmi.createButton(0x18);
MmiButton *mmiSmallWheelButton  = mmi.createButton(0x38);

// Wheels
MmiWheel *mmiSmallWheel = mmi.createWheel(0x40);
MmiWheel *mmiBigWheel   = mmi.createWheel(0x50);

// Lights
MmiLight mmiMediaLight(0x02, &mmi);
MmiLight mmiNameLight(0x03, &mmi);
MmiLight mmiTelLight(0x04, &mmi);
MmiLight mmiNavLight(0x05, &mmi);
MmiLight mmiInfoLight(0x06, &mmi);
MmiLight mmiCarLight(0x07, &mmi);
MmiLight mmiSetupLight(0x08, &mmi);
MmiLight mmiTopLeftLight(0x0A, &mmi);
MmiLight mmiBottomLeftLight(0x0B, &mmi);
MmiLight mmiTopRightLight(0x0D, &mmi);
MmiLight mmiBottomRightLight(0x0E, &mmi);
MmiLight mmiRadioLight(0x18, &mmi);

//*************************************************  beleuchtung   ******************************************
const byte ILL_Pin = 37;                      // eingang beleuchtungsschalter 12V  OK

byte dimValue;
const byte Pin_DimUp = 45;                   // Taster Dim Heller  OK
const byte Pin_DimDown = 46;                 // Taster Dim Dunkler  OK
int bel_Pin = 21; // 12V Dim Analog

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
Button zvLockButton(new InputTrigger(ZvLock));
Button zvUnlockButton(new InputTrigger(ZvUnlock));
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
int KLIMA_SW = 48;                      // Taster für A/C  (HVAC 15)
int KLIMA_LED = 47;                     // kontroll LED für Klima "AN" (HVAC 3)






 
// ******  Sketch ****** //  

void setup() {
  Serial.begin(9600);                       // Serial Monitor
  Serial.println("Goodnight moon!");
          
    
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

void loop() {  
  updateMmi();
  
  ILL_Beleuchtung();
  dimValue = Taster_Dimming();          
  mmi.setIllumination(dimValue);

  Engine_Start_Button();

  SWC();                           // Steering Wheel Control

  FOB();                           // Funkfernbedienung
}


  ///////////////////
 // MMI FUNCTIONS // 
///////////////////

void updateMmi() {
  mmi.update(mmiEvent);
  
  if (mmiNavButton->isPressed()) {
    Serial.println("Nav down!");
  }
  else if (mmiNavButton->wasPressedFor(1000)) {
    Serial.println("Nav 1 sec gehalten!");
  }
  else if (mmiNavButton->wasPressedTimes(1)) {
    Serial.println("Nav 1x gedrueckt!");
  }
  else if (mmiNavButton->wasPressedTimes(2)) {
    Serial.println("Nav 2x gedrueckt!");
    mmiNavLight.toggle();
  }

  if (mmiInfoButton->isPressed()) {
    Serial.println("Info down!");
  }
  else if (mmiInfoButton->wasPressedFor(1000)) {
    Serial.println("Info 1 sec gehalten!");
  }
  else if (mmiInfoButton->wasPressedTimes(1)) {
    Serial.println("Info 1x gedrueckt!");
  }
  else if (mmiInfoButton->wasPressedTimes(2)) {
    Serial.println("Info 2x gedrueckt!");
    mmiInfoLight.toggle();
  }


  if (mmiCarButton->isPressed()) {
    Serial.println("Car down!");
  }
  else if (mmiCarButton->wasPressedFor(1000)) {
    Serial.println("Car 1 sec gehalten!");
  }
  else if (mmiCarButton->wasPressedTimes(1)) {
    Serial.println("Car 1x gedrueckt!");
  }
  else if (mmiCarButton->wasPressedTimes(2)) {
    Serial.println("Car 2x gedrueckt!");
    mmiCarLight.toggle();
  }


  if (mmiSetupButton->isPressed()) {
    Serial.println("Setup down!");
  }
  else if (mmiSetupButton->wasPressedFor(1000)) {
    Serial.println("Setup 1 sec gehalten!");
  }
  else if (mmiSetupButton->wasPressedTimes(1)) {
    Serial.println("Setup 1x gedrueckt!");
  }
  else if (mmiSetupButton->wasPressedTimes(2)) {
    Serial.println("Setup 2x gedrueckt!");
    mmiSetupLight.toggle();
  }


  if (mmiRadioButton->isPressed()) {
    Serial.println("Radio down!");
  }
  else if (mmiRadioButton->wasPressedFor(1000)) {
    Serial.println("Radio 1 sec gehalten!");
  }
  else if (mmiRadioButton->wasPressedTimes(1)) {
    Serial.println("Radio 1x gedrueckt!");
  }
  else if (mmiRadioButton->wasPressedTimes(2)) {
    Serial.println("Radio 2x gedrueckt!");
    mmiRadioLight.toggle();
  }

if (mmiMediaButton->isPressed()) {
    Serial.println("Media down!");
  }
  else if (mmiMediaButton->wasPressedFor(1000)) {
    Serial.println("Media 1 sec gehalten!");
  }
  else if (mmiMediaButton->wasPressedTimes(1)) {
    Serial.println("Media 1x gedrueckt!");
  }
  else if (mmiMediaButton->wasPressedTimes(2)) {
    Serial.println("Media 2x gedrueckt!");
    mmiMediaLight.toggle();
  }


if (mmiNameButton->isPressed()) {
    Serial.println("Name down!");
  }
  else if (mmiNameButton->wasPressedFor(1000)) {
    Serial.println("Name 1 sec gehalten!");
  }
  else if (mmiNameButton->wasPressedTimes(1)) {
    Serial.println("Name 1x gedrueckt!");
  }
  else if (mmiNameButton->wasPressedTimes(2)) {
    Serial.println("Name 2x gedrueckt!");
    mmiNameLight.toggle();
  }


  if (mmiTelButton->isPressed()) {
    Serial.println("Tel down!");
  }
  else if (mmiTelButton->wasPressedFor(1000)) {
    Serial.println("Tel 1 sec gehalten!");
  }
  else if (mmiTelButton->wasPressedTimes(1)) {
    Serial.println("Tel 1x gedrueckt!");
  }
  else if (mmiTelButton->wasPressedTimes(2)) {
    Serial.println("Tel 2x gedrueckt!");
    mmiTelLight.toggle();
  }
  
  if (mmiTopLeftButton->wasPressedTimes(1)) {
    mmiTopLeftLight.toggle();
  }
  if (mmiTopRightButton->wasPressedTimes(1)) {
    mmiTopRightLight.toggle();
  }
  if (mmiBottomLeftButton->wasPressedTimes(1)) {
    mmiBottomLeftLight.toggle();
  }
  if (mmiBottomRightButton->wasPressedTimes(1)) {
    mmiBottomRightLight.toggle();
  }
  if (mmiPreviousButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_PREV_TRACK);
    Keyboard.release(KEY_MEDIA_PREV_TRACK);
  }
  if (mmiReturnButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_ESC);
    Keyboard.release(KEY_ESC);
  }
  if (mmiNextButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_NEXT_TRACK);
    Keyboard.release(KEY_MEDIA_NEXT_TRACK);
  }
  if (mmiNextButton->wasPressedTimes(2)) {
    Keyboard.press(KEY_MEDIA_PLAY_SKIP);
    Keyboard.release(KEY_MEDIA_PLAY_SKIP);
  }
  
  if (mmiBigWheel->wasTurned()) {
    if (mmiBigWheel->getAmount() < 0) {
      Keyboard.press(MODIFIERKEY_SHIFT);
      Keyboard.press(KEY_TAB);    
      Keyboard.release(KEY_TAB);
      Keyboard.release(MODIFIERKEY_SHIFT);    
    }
    else {
      Keyboard.press(KEY_TAB);    
      Keyboard.release(KEY_TAB);
    }
  }
  if (mmiBigWheelButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    Keyboard.release(KEY_MEDIA_PLAY_PAUSE);     
  }
  
  if (mmiSmallWheel->wasTurned()) {
    if (mmiSmallWheel->getAmount() < 0) {
      Keyboard.press(KEY_MEDIA_VOLUME_DEC);
      Keyboard.release(KEY_MEDIA_VOLUME_DEC);
    }
    else {
      Keyboard.press(KEY_MEDIA_VOLUME_INC);
      Keyboard.release(KEY_MEDIA_VOLUME_INC);
    }
  }
  if (mmiSmallWheelButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_MUTE);
    Keyboard.release(KEY_MEDIA_MUTE);
  }
}

void mmiEvent(uint8_t code) {
  if (code == 0xff || code == 0x38) {
    mmi.enableKeys();
  }
  else if (code == 0x35) {
    mmi.setHighlightLevel(0x99);
  }
}

void ILL_Beleuchtung() {
  static bool old_state = false;        // letzten Tasterzustand merken

  bool new_state = digitalRead(ILL_Pin); // Tasterzustand einlesen

  if (new_state != old_state) {       // hat sich Taster/Schalter-Stellung geÃ¤ndert?
    if ( new_state == true ) {        // Licht ein- oder ausschalten ...
      mmi.setIllumination(0xFF); Serial.print('\t'); Serial5.println(F("ON"));
      digitalWrite(bel_Pin, LOW);
      Serial.println("BELEUCHTUNG_AN") ;
    }
    else {
      mmi.setIllumination(0x00); Serial.print('\t'); Serial.println(F("OFF"));
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


