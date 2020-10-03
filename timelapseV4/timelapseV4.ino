//  Time Lapse Panorama Controller for GoPro Cameras
//  by
//  Brett Oliver
//  http://www.brettoliver.org.uk
//  http://www.instructables.com/member/oliverb/
//
//  Based on hardware and code by Tyler Winegarner
//  http://www.instructables.com/member/twinegarner/
//  https://www.youtube.com/watch?v=cS93hhrdY6U
// 
//  and also
//  code by Mark Wheeler
//  http://markwheeler.com/wordpress/?p=381
//  http://www.instructables.com/member/mwheeler3/
//


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

#include <TimeLib.h>
#define STEPS  2048   //Number of steps per revolution


// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
// lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE)

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
Stepper small_stepper(STEPS, 4, 6, 5, 7);
//Stepper small_stepper(STEPS, 18, 20, 19, 21);


int lcd_key     = 0;
int adc_key_in  = 0;
int endStep = 1;
int startPos = 14336;
int endPos = 14336;
int startAngle=0; // to calc/display total angle
int endAngle=0; // to calc/display total angle
int panAngle=0; // to calc/display total angle
int curPos = 0;
int minutes = 1;
int endtime = 0;
int timeNow = 0;
int rotations = 1; // Fixed to 1 rotation
long runtime = 0;
int rightLock = 0;
//int leftLock = 13600;
int leftLock = 20700;
int span = 0;
int spanTotal = 0;
int stepDelay = 0;
int spanRemain = 0;
int motorPWR = 5;
int time = 0; // clock holder
int runlength = 0;
int StepJump = 100;
int PanorRotate = 11; // Off standard Panorama Timelapse  On Panorama Timelapse with Rotation
int degRemain = 0; // shows degree of camera


#define btnRIGHT  9
#define btnLEFT   8
#define btnSELECT 10


int read_LCD_buttons()
{
 adc_key_in = digitalRead(9);
 if (adc_key_in == LOW) return btnRIGHT;      // read the value from the switch
 adc_key_in = digitalRead(8);
if (adc_key_in == LOW) return btnLEFT;      // read the value from the switch
 adc_key_in = digitalRead(10);
if (adc_key_in == LOW) return btnSELECT;      // read the value from the switch

} 
 



void setup(){

lcd.begin(20, 4);  // initialize the lcd for 20 chars 4 lines, turn on backlight

pinMode(motorPWR, OUTPUT);
pinMode(4, OUTPUT);
pinMode(btnRIGHT, INPUT_PULLUP);
pinMode(11, INPUT_PULLUP);
pinMode(btnLEFT, INPUT_PULLUP);
pinMode(btnSELECT, INPUT_PULLUP);
}

 
 

void loop(){
  
  if (digitalRead(11) != 0) // If selection switch is on then Standard Panorama Timelapse is run
  {
 
  digitalWrite(motorPWR, HIGH);
  lcd.setCursor(0,0);
    lcd.print(" TimeLapse Panorama ");
    lcd.setCursor(0,1);
    lcd.print("        v9.8        ");
    delay(2000);
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
  lcd.setCursor(0,0);
  lcd.print("Enter Start Angle   ");
  lcd.setCursor(0,2);
  lcd.print("Use Left/Right Keys ");
  lcd.setCursor(0,3);
  lcd.print("   Sel to confirm   ");
  small_stepper.setSpeed(9);
  

  while(endStep){
    
    lcd_key = read_LCD_buttons();  // read the buttons
     switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
       {
         if (startPos > rightLock){
           small_stepper.step(-StepJump);
           startPos = startPos - StepJump;
         }
         lcd.setCursor(0,1);
         lcd.print("pos:            ");
         lcd.setCursor(4,1);
         lcd.print(startPos);
         
         lcd.setCursor(10,1);
         lcd.print("Angle:");
         lcd.setCursor(16,1);
         lcd.print("    ");
         lcd.setCursor(16,1);
         lcd.print((14336-startPos)/40); // show position in degrees
         startAngle= (14336-startPos)/40;
         
         
         delay(250);
         break;
       }
     case btnLEFT:
       {
         if (startPos < leftLock){
           small_stepper.step(+StepJump);
           startPos = startPos + StepJump;
         }
         lcd.setCursor(0,1);
         lcd.print("pos:            ");
         lcd.setCursor(4,1);
         lcd.print(startPos);
         
         lcd.setCursor(10,1);
         lcd.print("Angle:");
         lcd.setCursor(16,1);
         lcd.print("    ");
         lcd.setCursor(16,1);
         lcd.print((14336-startPos)/40); // show position in degrees
         startAngle= (14336-startPos)/40;
         delay(250);
         break;
       }
       case btnSELECT:
       {
         endStep = 0;
         delay(250);
         break;
       }
       {
         break;
       }
     }
  }
  endStep = 1;
  lcd.setCursor(0,0);
  lcd.print("Enter End Angle     ");
  lcd.setCursor(0,3);
  lcd.print("   Sel to Confirm   ");

  endPos = startPos;
  
  while(endStep){
     lcd_key = read_LCD_buttons();  // read the buttons
     switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
       {
         if (endPos > rightLock){
           small_stepper.step(-StepJump);
           endPos = endPos - StepJump;
         }
         lcd.setCursor(0,1);
         lcd.print("pos:            ");
         lcd.setCursor(4,1);
         lcd.print(startPos);
         
         lcd.setCursor(10,1);
         lcd.print("Angle:");
         lcd.setCursor(16,1);
         lcd.print("    ");
         lcd.setCursor(16,1);
         
         lcd.print((14336-endPos)/40); // show position in degrees
         endAngle= (14336-endPos)/40;
         delay(250);
         break;
       }
     case btnLEFT:
       {
         if (endPos < leftLock){
           small_stepper.step(+StepJump);
           endPos = endPos + StepJump;
         }
         lcd.setCursor(0,1);
         lcd.print("pos:            ");
         lcd.setCursor(4,1);
         lcd.print(startPos);
         
         lcd.setCursor(10,1);
         lcd.print("Angle:");
         lcd.setCursor(16,1);
         lcd.print("    ");
         lcd.setCursor(16,1);
         
         lcd.print((14336-endPos)/40); // show position in degrees
         endAngle= (14336-endPos)/40;
         delay(250);
         break;
       }
       case btnSELECT:
       {
         endStep = 0;
         delay(250);
         break;
       }
       {
         break;
       }
     }
  }
  endStep = 1;
  lcd.setCursor(0,0);
  lcd.print("Enter Run Time:     ");
  lcd.setCursor(0,1);
  lcd.print("                    ");
  while(endStep){
    lcd_key = read_LCD_buttons();  // read the buttons
     lcd.setCursor(0,1);
     lcd.print("    minutes");
     lcd.setCursor(0,1);
     lcd.print(minutes);
     delay(50);
     switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
       {
         if (minutes < 30) {
         minutes = minutes + 1;
         }
         else if (minutes < 120) {
         minutes = minutes + 15;
         }
         else if (minutes < 480) {
         minutes = minutes + 30;
         }
         delay(250);
         break;
       }
     case btnLEFT:
       {
         if (minutes > 0 && minutes < 35) {
         minutes = minutes - 1;
         }
         else if (minutes > 0 && minutes < 150) {
         minutes = minutes - 15;
         }
         else if (minutes > 0 && minutes < 485) {
         minutes = minutes - 30;
         }
         delay(250);
         break;
       }
       case btnSELECT:
       {
         endStep = 0;
         delay(250);
         break;
       }
//         case btnNONE:
       {
         break;
       }
     }
  }
  lcd.setCursor(0,2);
     lcd.print("                    ");
  lcd.setCursor(0,3);
     lcd.print("                    ");
  runtime = (minutes * 60000);
  lcd.setCursor(0,0);
  tone(3, 2000, 100);
  lcd.print("  Panorama Running  ");
  lcd.setCursor(0,1);
  lcd.print("    Minutes Left");
  
  if (startAngle > 0 && endAngle > 0)
  {
    panAngle = startAngle - endAngle;
    abs(panAngle);
  }
  
  else if (startAngle > 0 && endAngle < 0 || startAngle > 0 && endAngle == 0)
  {
  panAngle = startAngle + abs(endAngle);
  }
  
  else if (startAngle < 0 && endAngle > 0 || startAngle == 0 && endAngle > 0)
  {
  panAngle = abs(startAngle) + abs(endAngle);
  }
  
  else if ((startAngle < 0 && endAngle < 0 || startAngle < 0 && endAngle == 0) && startAngle > endAngle)
  {
  panAngle = startAngle + abs(endAngle);
    abs(panAngle);
  }
  
  else if ((startAngle < 0 && endAngle < 0 || startAngle < 0 && endAngle == 0) && startAngle < endAngle)
  {
  panAngle = abs(startAngle) + endAngle;
    
  }
  
  else 
  {
  panAngle = 0;
  
  }
  
  
  if (startPos > endPos){ // checks to see if span is +ve or -ve
    span = startPos - endPos;
    spanTotal = startPos - endPos;
    spanRemain = span;
    stepDelay = (runtime / span);
    small_stepper.step(span);
    while (spanRemain) {
      small_stepper.step(-1);
      runtime = (runtime - stepDelay);
      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.setCursor(0,1);
      lcd.print((runtime / 60000) + 1);
      spanRemain--;
      delay(stepDelay);
        //**********************
  lcd.setCursor(0,2);
  lcd.print("Step Time m/s ");
 
    lcd.setCursor(14,2);
    lcd.print(stepDelay);
    
    
    
    
    lcd.setCursor(0,3);    
  lcd.print("Panorama Angle ");
  lcd.print(panAngle);
  //lcd.print(span/7,0);
  //lcd.setCursor(13,3);
  //lcd.print("Deg");
  
  //**********************
    }
      
  } else { //if span is -ve do this
    span = endPos - startPos;
    spanTotal = endPos - startPos;
    spanRemain = span;
    stepDelay = (runtime / span);
    small_stepper.step(span * -1);
    while (spanRemain) {
      small_stepper.step(1);
      runtime = (runtime - stepDelay);
      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.setCursor(0,1);
      lcd.print((runtime / 60000) + 1);
      spanRemain--;
      delay(stepDelay);
        //**********************
  lcd.setCursor(0,2);
  lcd.print("Step Time m/s ");

    lcd.setCursor(14,2);
    lcd.print(stepDelay);
    
  
    
    lcd.setCursor(0,3);
  lcd.print("Panorama Angle ");
  lcd.print(panAngle);
  //lcd.print(span/7,0);
  //lcd.setCursor(13,3);
  //lcd.print("Deg");
  
  //**********************
    }
    
   
  }
  endStep = 0;
  digitalWrite(4, LOW); // sets all motor coils off
  while(1){
   
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("                    ");
    
    lcd.setCursor(0,1);
    lcd.print(" Panorama  Complete ");
    lcd.setCursor(0,3);
    lcd.print("                v9.8");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("    Press Reset     ");
    lcd.setCursor(0,2);
    lcd.print("  To Start Again    ");
    
    delay(3000);
   
  }
  }
 else // If selection switch is on then Timelapse Panorama with rotation is run
 {
  //############################################################################
  //############################ ROTATION Panorama #############################
  //###########################Now fixed at 1 Rotation##########################
  
  digitalWrite(motorPWR, HIGH);
    while(endStep){
      // I do this for no reason.
    lcd.setCursor(0,0);
    lcd.print(" Rotation Panorama");
    lcd.setCursor(0,1);
    lcd.print("        v9.8        ");
    //delay(2000);
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    time_t timeNow = now(); // What time is it right now?
     endStep = 0;
  
  }
  // If you require more than 1 rotation un comment between /* and */ below
  /*
  endStep = 1; // Ask for Rotations
  lcd.setCursor(0,0);
  lcd.print("Rotations:      ");
  lcd.setCursor(0,1);
  lcd.print("(L/R) then SEL");

  

  while(endStep){
     lcd_key = read_LCD_buttons();  // read the buttons
     switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
       {
         
          rotations = rotations + 1;           
   
         lcd.setCursor(0,1);
         lcd.print("Rotations:     ");
         lcd.setCursor(11,1);
         lcd.print(rotations);
         delay(150);
         break;
       }
     case btnLEFT:
       {
    
          rotations = rotations - 1;           
         

         lcd.setCursor(0,1);
         lcd.print("Rotations:     ");
         lcd.setCursor(11,1);
         lcd.print(rotations);
         delay(150);
         break;
       }
       case btnSELECT:
       {
         endStep = 0;
         delay(150);
         break;
       }
         case btnNONE:
       {
         break;
       }
     }
  }
  */
  endStep = 1;
  // Ask for Duration
  lcd.setCursor(0,0);
  lcd.print("Enter Run Time:     ");
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("Use Min Down/Up Keys");
  
  lcd.setCursor(0,3);
  lcd.print("   Sel to Confirm   ");

  while(endStep){
     lcd_key = read_LCD_buttons();  // read the buttons
     switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
       {
          minutes = minutes + 1;
         
         lcd.setCursor(0,1);
         lcd.print("Minutes:      ");
         lcd.setCursor(11,1);
         lcd.print(minutes);
         delay(150);
         break;
       }
     case btnLEFT:
       {
           minutes = minutes - 1;
       
         lcd.setCursor(0,1);
         lcd.print("Minutes:      ");
         lcd.setCursor(11,1);
         lcd.print(minutes);
         delay(150);
         break;
       }
       case btnSELECT:
       {
    lcd.setCursor(0,2);
    lcd.print("                    "); // blanks display
    lcd.setCursor(0,3);
    lcd.print("                    "); // blanks display
         time_t timeNow = now(); // What time is it right now?
         endtime = second(timeNow) + (minutes*60); // Add duration seconds to get endtime
          endStep = 0;
         delay(150);
         break;
       }
//         case btnNONE:
       {
         break;
       }
     }
  }


  

  runtime = (minutes * 60000);
  lcd.setCursor(0,0);
  lcd.print("  Panorama Running  ");
  lcd.setCursor(0,1);
  lcd.print("        Seconds Left");

  if (rotations > 0) {
   
    span = (endPos - curPos);
    spanRemain = span;
    stepDelay = (runtime / span);
    // small_stepper.step(span * -1);
    while (spanRemain) {
      // small_stepper.step(1); // Counterclockwise
      small_stepper.step(-1); // Clockwise

      runtime = (runtime - stepDelay);
      // test for seconds left length
      runlength = ((runtime / 1000) + 1);
      
      
     // add number blanking for seconds display
     if (runlength >= 1000)
     {
      lcd.setCursor(0,1);
     } 
     else if (runlength >= 100 and runlength <= 999)
     {
       lcd.setCursor(0,1);
       lcd.print(" ");
       lcd.setCursor(1,1);
     } 
     else if (runlength >= 10 and runlength <= 99)
     {
     lcd.setCursor(0,1);
      lcd.print("   ");
       lcd.setCursor(2,1);
     }
     
     else {
     lcd.setCursor(0,1);
      lcd.print("    ");
       lcd.setCursor(3,1);
     }
      lcd.print((runtime / 1000) + 1  );
    // end add number blanking for seconds display  
      
     
    degRemain = (endPos/(spanRemain * 360));
    
    
      
     
      spanRemain--;
      // curPos = curPos + 1;
      delay(stepDelay);
    }
  rotations = rotations - 1;
 }
 
  endStep = 0;
  digitalWrite(4, LOW); // sets all motor coils off
  while(1){
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("                    ");
    
    lcd.setCursor(0,1);
    lcd.print(" Panorama  Complete ");
    lcd.setCursor(0,3);
    lcd.print("                v9.8");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("    Press Reset     ");
    lcd.setCursor(0,2);
    lcd.print("  To Start Again    ");
    
    delay(3000);
  }
  
  //==================================================================== 
 }
}

//######################################################################################################





