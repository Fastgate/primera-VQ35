#include "Button.h"
#include "Output.h"
#include "Mmi.h"


  /////////////
 // HELPERS // 
/////////////

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))


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


  //////////////////////////////
 // ILLUMINATION DEFINITIONS // 
//////////////////////////////

Button illuminationToggleButton(new InputTrigger(37, 20, LOW, INPUT_PULLUP), 0);
Button illuminationDimUpButton(new InputTrigger(45, 20, LOW, INPUT_PULLUP), 0);
Button illuminationDimDownButton(new InputTrigger(46, 20, LOW, INPUT_PULLUP), 0);
Output illuminationOutput(21, HIGH);

uint8_t desiredIlluminationLevel = 0xFF / 2;
uint8_t illuminationLevel = 0x00;
bool illuminationState = false;


  //////////////////////////
 // IGNITION DEFINITIONS // 
//////////////////////////

#define IGNITION_OFF  0
#define IGNITION_ACC  1
#define IGNITION_ON   2

Button ignitionButton(new InputTrigger(39, 20, LOW, INPUT_PULLUP), 0);
Button crankSensor(new InputTrigger(41, 20, LOW));
InputTrigger clutchSensor(16, 20, LOW, INPUT_PULLUP);
InputTrigger breakSensor(17, 20, LOW, INPUT_PULLUP);
InputTrigger neutralSensor(36, 20, LOW, INPUT_PULLUP);
InputTrigger clutch(16, 20, LOW, INPUT_PULLUP);
InputTrigger keySensor(6, 20, LOW, INPUT_PULLUP);

struct {
  uint8_t state = IGNITION_OFF;
  bool engine   = false;
} ignition;

struct {
  Output *ready   = new Output(20, HIGH);
  Output *button  = new Output(24, HIGH);
  Output *acc     = new Output(5, HIGH);
  Output *on      = new Output(2, HIGH);
  Output *nats    = new Output(57, HIGH);
} ignitionLights;

struct {
  Output *acc         = new Output(42, HIGH);
  Output *on          = new Output(44, HIGH);
  Output *key         = new Output(43, HIGH);
  Output *nats        = new Output(53, HIGH);
  TimedOutput *crank  = new TimedOutput(new Output(41, HIGH));
} ignitionOutputs;


// ************************** Steering Wheel Control *****************************

int wheelPin = A11; // steering wheel resistance reading pin  OK
int wheelPin2 = A10; // steering wheel resistance reading pin  OK

int prevButton_SWC=0;


//************************** Primera STW Inputs *******************************

int RevGear = 38;           // Rückwärtsgang  OK
int RevGear_Stat = 0;

  ////////////////////////////
 // KEY REMOTE DEFINITIONS //
////////////////////////////

int ZvLock = 22;                            // Verriegeln // gedrÃ¼ckt 5V // losgelassen 0V getestet  OK
int LOCK_RLY = 27;                          // OK
int UNLOCK_RLY = 28;                        // OK
int ZvUnlock = 23;                          // Entriegeln // gedrÃ¼ckt 5V // losgelassen 0V  OK
Button zvLockButton(new InputTrigger(ZvLock));
Button zvUnlockButton(new InputTrigger(ZvUnlock));
TimedOutput lockRelay(new Output(LOCK_RLY, HIGH));
TimedOutput unlockRelay(new Output(UNLOCK_RLY, HIGH));
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
    
  // **************************** Steering Wheel Control ***********

   pinMode(wheelPin, INPUT);
   pinMode(wheelPin2, INPUT);


  // *********************** Primera STW Inputs ************************
  
  pinMode(RevGear, INPUT_PULLUP);
  digitalWrite(RevGear, HIGH); 
     
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
  updateIllumination();

  updateIgnition();

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


  ////////////////////////////
 // ILLUMINATION FUNCTIONS // 
////////////////////////////

void updateIllumination() {
  illuminationDimUpButton.update();
  illuminationDimDownButton.update();
  illuminationToggleButton.update();
  
  if (illuminationDimUpButton.wasPressedTimes(1)) {
    desiredIlluminationLevel = min(255, (desiredIlluminationLevel + 0xFF / 16));
  }
  if (illuminationDimDownButton.wasPressedTimes(1)) {
    desiredIlluminationLevel = max(46, (desiredIlluminationLevel - 0xFF / 16));
  }  
  if (illuminationToggleButton.wasPressedTimes(1)) {
    illuminationState = !illuminationState;
  }

  changeIllumination(illuminationState, desiredIlluminationLevel);
}

void changeIllumination(bool newState, uint8_t newLevel) {
  newLevel = newState ? newLevel : 0x00; 
  if (illuminationState != newState || illuminationLevel != newLevel) {
    illuminationState = newState;
    illuminationLevel = newLevel;
    
    Serial.printf("Illumination %d.\r\n", illuminationLevel);
    illuminationOutput.set(!illuminationState);
    mmi.setIllumination(illuminationLevel);
    mmi.setHighlightLevel(max(0x46, illuminationLevel));
  }
}


  ////////////////////////
 // IGNITION FUNCTIONS // 
////////////////////////

void updateIgnition() {
  ignitionButton.update();
  ignitionOutputs.crank->update();

  if (ignitionButton.isPressed() && keySensor.getState()) {
    // switch engine on
    if ((clutch.getState() || neutralSensor.getState()) && ignition.state != IGNITION_ON) {
      startEngine();
    }
    // toggle through ignition states
    else {
      setIgnition(ignition.state >= IGNITION_ON ? IGNITION_OFF : ignition.state+1);
    }
  }

  ignitionLights.ready->set(!(clutch.getState() && ignition.state != IGNITION_ON && keySensor.getState()));

  // reenable ACC after crank has stopped
  if (crankSensor.wasPressedTimes(1)) {
    ignitionLights.acc->set(ignition.state >= IGNITION_ACC);
  }
}

void setIgnition(uint8_t newState) {
  if (ignition.state != newState) {
    ignition.state = newState;

    ignitionOutputs.acc->set(!(ignition.state >= IGNITION_ACC && !ignitionOutputs.crank->getState()));
    ignitionLights.acc->set(!(ignition.state == IGNITION_ACC));
    ignitionLights.nats->set(!(ignition.state >= IGNITION_ACC));

    ignitionOutputs.nats->set(!(ignition.state >= IGNITION_ON));
    ignitionOutputs.key->set(!(ignition.state >= IGNITION_ON));
    ignitionOutputs.on->set(!(ignition.state >= IGNITION_ON));
    ignitionLights.on->set(!(ignition.state >= IGNITION_ON));
    Serial.printf("Ignition: %d.\r\n", ignition.state);
  }
}

void startEngine() {
  setIgnition(IGNITION_ON);
  if (clutchSensor.getState() || neutralSensor.getState()) {
    ignitionOutputs.acc->set(HIGH);
    ignitionOutputs.crank->set(LOW, 600);
    Serial.println("Start engine!");    
  }
}


  //////////////////////////////
 // STEERING WHEEL FUNCTIONS //
//////////////////////////////
 
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


  //////////////////////////
 // KEY REMOTE FUNCTIONS //
//////////////////////////

void FOB(){
  zvLockButton.update();
  zvUnlockButton.update();
  lockRelay.update();
  unlockRelay.update();

  if (zvUnlockButton.wasPressedTimes(1)) {
    Serial.println("AufschlieÃŸen.");
    unlockRelay.set(LOW, 100);
    digitalWrite(Android_OTG, HIGH);
    OTG_status = 1;
  }
  if (zvUnlockButton.wasPressedTimes(3)) {
    Serial.println("AufschlieÃŸen und Fenster auf.");
    unlockRelay.set(LOW, 4000);
  }
  if (zvLockButton.wasPressedTimesOrMore(4)) {
    // TODO: CODE FUER 15 MINUTEN MOTORSTART  
  }
  if (zvLockButton.wasPressedTimes(3)) {
    Serial.println("AbschlieÃŸen und Fenster zu.");
    lockRelay.set(LOW, 6000);
  }

  if (zvLockButton.wasPressedTimesOrMore(4)) {
     digitalWrite(Android_OTG, LOW);
     OTG_status = 0;
  }
}


