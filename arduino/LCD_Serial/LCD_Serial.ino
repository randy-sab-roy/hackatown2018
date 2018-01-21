#include <rgb_lcd.h>

#include <Wire.h>

rgb_lcd lcd;

int level = 0;
int oldLevel = 0;

byte square[8] = {
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b00000,
   0b00000
};

void setup() {

  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(0, square);
  // Print a message to the LCD.
  //lcd.print("hello, world!");

  lcd.setCursor(0 ,0);

  //Buzzer
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
}



void loop() {

  if (Serial.available() > 0) 
  {      
    int in = Serial.parseInt();  //Read the data the user has input
    Serial.print("Received: ");
    Serial.println(in, DEC);
    
    if (in < 0 || in > 16){
      in = 0;
    }
    
    level = in;
    
    Serial.print("Level is now: ");
    Serial.println(level, DEC);
  }
  
  if (level != oldLevel){
    lcd.clear();
  }
  
  lcd.clear();
  if (level == 0)
  {
      clearLCD();
      digitalWrite(8, LOW);
  }
  else if (level < 6)
  {
    setGreen();
    digitalWrite(8, LOW);

  }
  else if ( level < 12)
  {
    setYellow();
    digitalWrite(8, LOW);

  }
  else
  {
    setRed();
    digitalWrite(8, HIGH);
  }

  for(int j = 0; j < 2; j++)
  {
    lcd.setCursor(0 ,j);
    for (int i = 0; i<level; i++)
    {
      lcd.write((unsigned char)0);
    }
  }

  oldLevel = level;
  //level = (level + 1) % 16;//for testing
  
  if (level >= 12){
  
    delay(1000);
  
  }else{
  
    delay(15); 
  
  }
}


void setGreen()
{
  lcd.setRGB(0, 255, 0);
}
void setYellow()
{
  lcd.setRGB(255, 255, 0);
}
void setRed()
{
  lcd.setRGB(255, 0, 0);
}
void clearLCD()
{
   lcd.clear();
   lcd.setRGB(0, 0, 0);
}