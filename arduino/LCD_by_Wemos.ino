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

  Serial.begin(250000);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(0, square);
  // Print a message to the LCD.
  //lcd.print("hello, world!");

  lcd.setCursor(0 ,0);

  //Buzzer
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  //Wemos rx
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
}



void loop() {

  readLevel();

  if (level != oldLevel){
    lcd.clear();
  }
  
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
  
  lcd.setCursor(0 ,0);
  for ( int i = 0;i<level; i++)
  {
    lcd.write((unsigned char)0);
  }
  lcd.setCursor(0 ,1);
  for ( int i = 0;i<level; i++)
  {
    lcd.write((unsigned char)0);
  }

  oldLevel = level;
  //level = (level + 1) % 16;//for testing
  delay(15);//test
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

void readLevel(){
  int level0 = digitalRead(3);
  int level1 = digitalRead(4);
  int level2 = digitalRead(5);
  int level3 = digitalRead(6);
  int level4 = digitalRead(7);
  
  level = 16*level4 + 8*level3 + 4*level2 + 2*level1 + level0;
  Serial.print("New Level:");
  Serial.println(level, DEC);
}

