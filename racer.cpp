/* 
 *  Aaron Bantay
 *  
 *  Arduino Racer Game
 * 
 */

#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int randNum;
int randNumY;
int posX;
int posY;
int count;
int score;
bool topBool = true;
bool botBool = false;
bool reset = false;
unsigned short top = 0x0000;
unsigned short bot = 0x0000;
char obs = '0';
char car = 'D';
char collis = 'X';

void generate()
{
  //Serial.println("3");
  randNum = random(0,4); // 1 means generate object, 2 means don't
  randNumY = random(0,4); // 1 means top row, 2 means bottom
  //Serial.println(randNum);
  //Serial.println(randNumY);
  
  if (randNum == 1 && randNumY == 1)
  {
    top = top | 0x0001;
  }
  else if ((randNum == 2 && randNumY == 1) || (randNum == 0 && randNumY == 1))
  {
    top = top | 0x0000;
  }
  if (randNum == 1 && randNumY == 2)
  {
    bot = bot | 0x0001;
  }
  else if ((randNum == 2 && randNumY == 2) || (randNum == 0 && randNumY == 2))
  {
    bot = bot | 0x0000;
  }
}

void print_obj()
{
  //Serial.println("4");
  for (int i = 0; i < 16; i++)
  {
    if ((top >> i) & 0x0001)
    {
      lcd.setCursor(15-i, 0);
      lcd.write(obs);
    }
    else
    {
      lcd.setCursor(15-i, 0);
      lcd.write(' ');
    }

    if ((bot >> i) & 0x0001)
    {
      lcd.setCursor(15-i, 1);
      lcd.write(obs);
    }
    else
    {
      lcd.setCursor(15-i, 1);
      lcd.write(' ');
    }
    if (posY == 0)
    {
      lcd.setCursor(0, posY);
      lcd.write(car);
    }
    else if (posY == 1)
    {
      lcd.setCursor(0, posY);
      lcd.write(car);
    }
  }
}

typedef struct task
{
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
}task;

const unsigned short tasksNum = 2;
task tasks[tasksNum];

enum P2_States {P2_Start, P2_Display, P2_Lose, P2_Restart} P2_States = P2_Start;
int Tick_Game(int state)
{
 switch(state)
 {
  case P2_Start:
    state = P2_Display;
    //Serial.println("0");
    break;

  case P2_Display:
    generate();
    print_obj();

    if (reset == true)
    {
      state = P2_Restart;
    }
 
    if ((posY == 0) && (top >> 15))
    {
      state = P2_Lose;
      lcd.setCursor(0,0);
      lcd.write(collis);
    }
    
    if ((posY == 1) && (bot >> 15))
    {
      state = P2_Lose;
      lcd.setCursor(0,1);
      lcd.write(collis);
    }

    if (score % 10 == 0)
    {
      if (tasks[1].period > 100)
      {
        tasks[1].period -= 35;
      }
    }
       
    top = top << 1;
    bot = bot << 1;
    score++;
    //Serial.println("1");
    break;

  case P2_Lose:
    tasks[1].period = 500;
    if (count == 6)
    {
      lcd.clear();
      //lcd.setCursor(0,0);
      lcd.print("Score = ");
      lcd.print(score);
    }
    if (reset == true)
    {
      state = P2_Start;
    }
    count++;
    break;

  case P2_Restart:
    lcd.clear();
    reset = false;
    tasks[1].period = 500;
    top = 0x0000;
    bot = 0x0000;
    count = 0;
    score = 0;
    state = P2_Start;

  default:
    state = P2_Start;
    break;
 }
 return state;
}

enum P2_States2 {P2_Start2, P2_Move, P2_DoNoth, P2_Reset} P2_States2 = P2_Start2;
int Tick_Buttons(int state)
{
  switch(state)
  {
    case P2_Start2:
      Serial.println("1");
      lcd.setCursor(0,0);
      lcd.write(car);
      posY = 0;
      state = P2_Move;
      break;

    case P2_Move:
      Serial.println("2");
      if (digitalRead(A2) == HIGH)
      {
        state = P2_Reset;
      }
      if (digitalRead(A0) == HIGH && digitalRead(A1) == LOW && digitalRead(A2) == LOW)
      {
        posY = 0;
      }
      else if (digitalRead(A1) == HIGH && digitalRead(A0) == LOW && digitalRead(A2) == LOW)
      {
        posY = 1;
      }
      else if (digitalRead(A0) == LOW && digitalRead(A0) == LOW && digitalRead(A2) == LOW)
      {
        state = P2_DoNoth;
      }
      break;

    case P2_DoNoth:
      Serial.println("3");
      if (digitalRead(A0) == HIGH || digitalRead(A1) == HIGH)
      {
        state = P2_Move;
      }
      else if (digitalRead(A2) == HIGH)
      {
        state = P2_Reset;
      }
      else if (digitalRead(A0) == LOW && digitalRead(A0) == LOW)
      {
        if (posY == 0)
        {
          posY = 0;
        }
        else if (posY == 1)
        {
          posY = 1;
        }
      }
      break;

    case P2_Reset:
      reset = true;
      state = P2_Start2;
      break;
    

    default:
      state = P2_Start2;
      break;
  }
  return state;
}


void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //lcd.setCursor(0,0);
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  randomSeed(analogRead(0));
  //lcd.cursor();
  
  unsigned char i = 0;
  tasks[i].state = P2_Start;
  // CHANGE LATER
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Tick_Buttons;
  i++;

  tasks[i].state = P2_Start2;
  // CHANGE LATER
  tasks[i].period = 500;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Tick_Game;
  
  Serial.begin(9600);
}


void loop()
{
  unsigned char i;
  for (i = 0; i < tasksNum; ++i)
  {
    if ((millis() - tasks[i].elapsedTime) >= tasks[i].period)
    {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
  delay(100);
}