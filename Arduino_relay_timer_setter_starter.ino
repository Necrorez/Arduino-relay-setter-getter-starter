#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

String menuItems[] = {"Startas", "Druska", "Cukrus", "Aliejus","Kitas","Miltai"};
int outsideButton = 12;

int readKey;
int savedDistance = 0;

int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;

int firstWrite = 0;

typedef struct{
unsigned long timer;
bool active;
int pin;
}item;

item Item1;
item Item2;
item Item3;
item Item4;
item Item5;

unsigned long previousMillis = 0;

byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};

// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

  Item5.timer = 0;
  Item5.active = false;
  Item1.timer = 0;
  Item1.active = false;
  Item2.timer = 0;
  Item2.active = false;
  Item3.timer = 0;
  Item3.active = false;
  Item4.timer = 0;
  Item4.active = false;

  //ReadFromEEPROM();

  pinMode(Item1.pin,OUTPUT);
  pinMode(Item2.pin,OUTPUT);
  pinMode(Item3.pin,OUTPUT);
  pinMode(Item4.pin,OUTPUT);

  pinMode(outsideButton,INPUT_PULLUP);
  // Initializes serial communication
  Serial.begin(9600);

  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);

}

void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
  
}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  Serial.print(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}


void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            menuItem1();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
          case 4:
            menuItem6();
            break;
          case 5:
            menuItem5();
            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
  int result = 0;
  if (x < 50) {
    result = 1; // right
  } else if (x < 195) {
    result = 2; // up
  } else if (x < 380) {
    result = 3; // down
  } else if (x < 555) {
    result = 4; // left
  }else if (x < 790) {
    result = 5; // select
  }
  return result;
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
void drawInstructions() {
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.write(byte(1)); // Up arrow
  lcd.print("/");
  lcd.write(byte(2)); // Down arrow
  lcd.print(" buttons");
}

void menuItem1() { // Function executes when you select the Yellow item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pradekite");


  while (activeButton == 0) {
    int button = buttonNumber();
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case 5:
        StartTimer();
        break;
    }
  }
}
//Function that begins the timer for 5 external relays
void StartTimer()
{
  //WriteToEEPROM();
  item Items[] = {Item1,Item2,Item3,Item4,Item5};
  //sets certain timers to 0 s if they are set as false
  for(int i = 0; i< 5; i++)
  {
    if(Items[i].active == false)
    Items[i].timer = 0;
  }
  //sorts the 5 items so that it would be possible to deactivate some of them in the same time
  for(int i = 0; i< 5; i++)
  {
    for(int j = i+1; j<5;j++ )
    {
      if(Items[i].timer>= Items[j].timer)
      {
        item temp = Items[i];
        Items[i] = Items[j];
        Items[j] = temp;
      }
    }
  }

   unsigned long currentMillis = millis();
   unsigned long endSeconds = (Items[4].timer%60000)/1000;
   unsigned long endMinutes= (Items[4].timer/60000);

   
   bool cancel = false;
   
   
   unsigned long period = Items[0].timer;
   while (millis() < (unsigned long)(currentMillis + period) )
   {  
    digitalWrite(Items[0].pin,HIGH);
    digitalWrite(Items[1].pin,HIGH);
    digitalWrite(Items[2].pin,HIGH);
    digitalWrite(Items[3].pin,HIGH);
    int button = buttonNumber();
   button = evaluateButton(readKey);
    if(button == 4 || cancel == true)
    {
      cancel = true;
      break;
      }
    if(millis() > previousMillis + 1000)
      {
        previousMillis = millis();
        
        BuildTimerTable(endSeconds,endMinutes);
        if(endSeconds <= 0)
        {
          endSeconds = 60;
          endMinutes --;
          }
        
        endSeconds--;
        }
   }

   period = Possitive(Items[1].timer,Items[0].timer);
   while (millis() < (unsigned long)(currentMillis + period) )
   {  
      digitalWrite(Items[0].pin,LOW);
      digitalWrite(Items[1].pin,HIGH);
      digitalWrite(Items[2].pin,HIGH);
      digitalWrite(Items[3].pin,HIGH);
      int button = buttonNumber();
   button = evaluateButton(readKey);
      if(button == 4 || cancel == true)
    {
      cancel = true;
      break;
      }
      if(millis() > previousMillis + 1000)
      {
        previousMillis = millis();
        
        BuildTimerTable(endSeconds,endMinutes);
        if(endSeconds <= 0)
        {
          endSeconds = 60;
          endMinutes --;
          }
        endSeconds--;
       }
   }  

   period = Possitive(Items[2].timer,(Items[1].timer+ Items[0].timer));
   while (millis() < (unsigned long)(currentMillis + period) )
   {  
      digitalWrite(Items[0].pin,LOW);
      digitalWrite(Items[1].pin,LOW);
      digitalWrite(Items[2].pin,HIGH);
      digitalWrite(Items[3].pin,HIGH);
      int button = buttonNumber();
   button = evaluateButton(readKey);
      if(button == 4 || cancel == true)
    {
      cancel = true;
      break;
      }
      if(millis() > previousMillis + 1000)
      {
        previousMillis = millis();
       
       BuildTimerTable(endSeconds,endMinutes);
        if(endSeconds <= 0)
        {
          endSeconds = 60;
          endMinutes --;
          }
        endSeconds--;
        }
   } 
   period = Possitive(Items[3].timer,(Items[1].timer+ Items[0].timer+Items[2].timer));
    while (millis() < (unsigned long)(currentMillis + period) )
    {
      digitalWrite(Items[0].pin,LOW);
      digitalWrite(Items[1].pin,LOW);
      digitalWrite(Items[2].pin,LOW);
      digitalWrite(Items[3].pin,HIGH);
      int button = buttonNumber();
   button = evaluateButton(readKey);
      if(button == 4 || cancel == true)
    {
      cancel = true;
      break;
      }
      if(millis() > previousMillis + 1000)
      {
        previousMillis = millis();
        
        BuildTimerTable(endSeconds,endMinutes);
        if(endSeconds <= 0)
        {
          endSeconds = 60;
          endMinutes --;
          }
        endSeconds--;
        }
   }
   period = Possitive((Items[4].timer),(Items[1].timer + Items[0].timer+Items[2].timer+Items[3].timer));
    while (millis() < (unsigned long)(currentMillis + period)  )
    {
      digitalWrite(Items[0].pin,LOW);
      digitalWrite(Items[1].pin,LOW);
      digitalWrite(Items[2].pin,LOW);
      digitalWrite(Items[3].pin,LOW);
      digitalWrite(Items[4].pin,HIGH);
      int button = buttonNumber();
   button = evaluateButton(readKey);
      if(button == 4 || cancel == true)
    {
      cancel = true;
      break;
      }
      if(millis() > previousMillis + 1000)
      {
        previousMillis = millis();
        
        BuildTimerTable(endSeconds,endMinutes);
        if(endSeconds <= 0)
        {
          endSeconds = 60;
          endMinutes --;
          }
        endSeconds--;
        }
   }
   
   
   
   digitalWrite(Items[0].pin,LOW);
   digitalWrite(Items[1].pin,LOW);
   digitalWrite(Items[2].pin,LOW);
   digitalWrite(Items[3].pin,LOW);
   digitalWrite(Items[4].pin,LOW);
   cancel = false;
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Pradekite");
}

//Helper funtions to create shown time
void BuildTimerTable(unsigned long endSeconds, unsigned long endMinutes)
{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Liko laiko:");
        lcd.setCursor(6, 1);
        lcd.print(endSeconds);
        lcd.setCursor(8, 1);
        lcd.print("s");
        lcd.setCursor(2, 1);
        lcd.print("min");
        lcd.setCursor(0, 1);
        lcd.print(endMinutes);
}
//Reads values from the EEPROM memory so that everytime you should not need to set it
void ReadFromEEPROM()
{
  EEPROM.get(0,Item1);
  EEPROM.get(50,Item2);
  EEPROM.get(100,Item3);
  EEPROM.get(150,Item4);
  EEPROM.get(200,Item5);
}

//Writes to EEPROM
void WriteToEEPROM()
{
  
  if(CheckIfUnchanged() == false)
  {
    /*Item1.pin = 0;
    Item2.pin = 1;           //   <<===== If there ever need to be a time to change certains pins it needs to be done inside the code
    Item3.pin = 2;
    Item4.pin = 3;
    Item5.pin = 11*/
    EEPROM.put(0,Item1);
    EEPROM.put(50,Item2);
    EEPROM.put(100,Item3);
    EEPROM.put(150,Item4);
    EEPROM.put(200,Item5);
    }
    
}
//Checks if all values are changed or not, so that would save the EEPROM cycles
bool CheckIfUnchanged()
{
  if(ItemUnchanged(Item1,0) == true &&ItemUnchanged(Item2,50) == true &&ItemUnchanged(Item3,100) == true &&ItemUnchanged(Item4,150) == true &&ItemUnchanged(Item5,200) == true )
    return true;
  else return false;   
}
//checks if anything from a single items has been changed
bool ItemUnchanged(item x,int adress)
{
  item toCheck;
  EEPROM.get(adress,toCheck);
  if(x.timer == toCheck.timer && x.pin == toCheck.pin && x.active == toCheck.active)
     return true;
  else return false;
  }
//method that takes aways time so that it wouldn't return a negative number
unsigned long Possitive(unsigned long right, unsigned long left)
{
  unsigned long toReturn = 0;
  if(right == 0)
    toReturn = 0;
  else if(right < left){ unsigned long temp = left - right; toReturn = left - temp;}
  else toReturn = right - left;
  return toReturn;
}

void menuItem2() { // Function executes when you select the Green item from main menu
  int activeButton = 0;
  CreateScreen("Druska",Item1); 

  while (activeButton == 0) {
    
    int button = buttonNumber();
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case 2:
        Item1.timer = AddToTimer(Item1.timer,1000,true,60000);
        CreateScreen("Druska",Item1); 
        break;   
      case 3:
        Item1.timer = AddToTimer(Item1.timer,1000,false,60000);
        CreateScreen("Druska",Item1);  
        break;
      case 5:
        if(Item1.active == false) Item1.active = true;
        else if(Item1.active == true) Item1.active = false;
        CreateScreen("Druska",Item1);  
        break;
    }
  }
}

void menuItem3() { // Function executes when you select the Red item from main menu
  int activeButton = 0;
  CreateScreen("Cukrus",Item2); 

  while (activeButton == 0) {
    
    int button = buttonNumber();
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case 2:
        Item2.timer = AddToTimer(Item2.timer,1000,true,60000);
        CreateScreen("Cukrus",Item2); 
        break;   
      case 3:
        Item2.timer = AddToTimer(Item2.timer,1000,false,60000);
        CreateScreen("Cukrus",Item2);  
        break;
      case 5:
        if(Item2.active == false) Item2.active = true;
        else if(Item2.active == true) Item2.active = false;
        CreateScreen("Cukrus",Item2);  
        break;
    }
  }
}

void menuItem4() { // Function executes when you select the Purple item from main menu
  int activeButton = 0;
  CreateScreen("Aliejus",Item3); 

  while (activeButton == 0) {
    
    int button = buttonNumber();
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case 2:
        Item3.timer = AddToTimer(Item3.timer,1000,true,60000);
        CreateScreen("Aliejus",Item3); 
        break;   
      case 3:
        Item3.timer = AddToTimer(Item3.timer,1000,false,60000);
        CreateScreen("Aliejus",Item3);  
        break;
      case 5:
        if(Item3.active == false) Item3.active = true;
        else if(Item3.active == true) Item3.active = false;
        CreateScreen("Aliejus",Item3);  
        break;
    }
  }
}

void menuItem5() { // Function executes when you select the Purple item from main menu
  int activeButton = 0;
  CreateScreenMin("Miltai",Item5); 

  while (activeButton == 0) {
    
    int button = buttonNumber();
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case 2:
        Item5.timer = AddToTimer(Item5.timer,60000,true,900000);
        CreateScreenMin("Miltai",Item5); 
        break;   
      case 3:
        Item5.timer = AddToTimer(Item5.timer,60000,false,900000);
        CreateScreenMin("Miltai",Item5);  
        break;
      case 5:
        if(Item5.active == false) Item5.active = true;
        else if(Item5.active == true) Item5.active = false;
        CreateScreenMin("Miltai",Item5);  
        break;
    }
  }
}

void menuItem6() { // Function executes when you select the Purple item from main menu
  int activeButton = 0;
  CreateScreen("Kitas",Item4); 

  while (activeButton == 0) {
    
    int button = buttonNumber();
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case 2:
        Item4.timer = AddToTimer(Item4.timer,1000,true,60000);
        CreateScreen("Kitas",Item4); 
        break;   
      case 3:
        Item4.timer = AddToTimer(Item4.timer,1000,false,60000);
        CreateScreen("Kitas",Item4);  
        break;
      case 5:
        if(Item4.active == false) Item4.active = true;
        else if(Item4.active == true) Item4.active = false;
        CreateScreen("Kitas",Item4);  
        break;
    }
  }
}

void CreateScreen(char message[], item x)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(0,1);
    lcd.print(x.timer/1000);
    lcd.setCursor(3,1);
    lcd.print("s");
    lcd.setCursor(12,0);
    if(x.active == true){
      lcd.print("ON");
    }else if(x.active == false){
      lcd.print("OFF");
    }
}

void CreateScreenMin(char message[], item x)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(0,1);
    lcd.print(x.timer/60000);
    lcd.setCursor(3,1);
    lcd.print("min");
    lcd.setCursor(12,0);
    if(x.active == true){
      lcd.print("ON");
    }else if(x.active == false){
      lcd.print("OFF");
    }
}
//Function that adds x amount to a certain item
unsigned long AddToTimer(unsigned long item ,unsigned long amount, bool sign, unsigned long maximum)
{
  unsigned long toReturn = item;
  if(sign == true)
  {
    if(toReturn < maximum){ toReturn = toReturn + amount;}
    else toReturn = maximum;
  }
  else if(sign == false){
    if(toReturn >= 1) {toReturn = toReturn - amount;}
    else toReturn = 0;
  }
  return toReturn;
}

int buttonNumber(){
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    return button;
}
