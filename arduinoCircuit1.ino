#define BUTTON1_PIN               7  // Button 1
#define BUTTON2_PIN               2  // Button 2
#define BUTTON3_PIN               3  // Button 3
#define BUTTON4_PIN               4  // Button 4
#define BUTTON5_PIN               5  // Button 5
#define BUTTON6_PIN               6  // Button 6

#define DEFAULT_LONGPRESS_LEN    25  // Min nr of loops for a long press
#define DELAY                    30  // Delay per loop in ms
#define MAX_BAR_VALUE            210 // Maximum value for utopia and distopia 
#define MAX_BAR_COLS              15 // All cols for progress bar 

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 9, 13, 10, 8);

char* myStrings[]={"QES:", "0  ", " EMA:", "0  ",
                       "SAT:", "0  ", " ART:", "0  ",
                       "TRA:", "0  ", " JUS:", "0  ",
                       "TEC:", "0  "
                      };
    

//////////////////////////////////////////////////////////////////////////////

enum { EV_NONE=0, EV_SHORTPRESS, EV_LONGPRESS };
//society characteristics
int ethicsCnt = 0;
int logicCnt = 0;
int mathCnt = 0;
int perceptCnt = 0;
int mythCnt = 0;
int rhetoricCnt = 0;

//leds pins
int ledU = A1;
int ledD = A0;

//influence factor
float infFactor = 1.2, utopia = 0, distopia = 0;

byte p1[8] = {
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10
};
 
byte p2[8] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18};

byte p3[8] = {
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C};

byte p4[8] = {
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E};

byte p5[8] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F};




//////////////////////////////////////////////////////////////////////////////
// Class definition

class ButtonHandler {
  public:
    // Constructor
    ButtonHandler(int pin, int longpress_len=DEFAULT_LONGPRESS_LEN);

    // Initialization done after construction, to permit static instances
    void init();

    // Handler, to be called in the loop()
    int handle();

  protected:
    boolean was_pressed;     // previous state
    int pressed_counter;     // press running duration
    const int pin;           // pin to which button is connected
    const int longpress_len; // longpress duration
};

ButtonHandler::ButtonHandler(int p, int lp)
: pin(p), longpress_len(lp)
{
}

void ButtonHandler::init()
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH); // pull-up
  was_pressed = false;
  pressed_counter = 0;
}

int ButtonHandler::handle()
{
  int event;
  int now_pressed = !digitalRead(pin);

  if (!now_pressed && was_pressed) {
    // handle release event
    if (pressed_counter < longpress_len)
      event = EV_SHORTPRESS;
    else
      event = EV_LONGPRESS;
  }
  else
    event = EV_NONE;

  // update press running duration
  if (now_pressed)
    ++pressed_counter;
  else
    pressed_counter = 0;

  // remember state, and we're done
  was_pressed = now_pressed;
  return event;
}

//////////////////////////////////////////////////////////////////////////////

// Instantiate button objects
ButtonHandler button1(BUTTON1_PIN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler button2(BUTTON2_PIN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler button3(BUTTON3_PIN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler button4(BUTTON4_PIN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler button5(BUTTON5_PIN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler button6(BUTTON6_PIN, DEFAULT_LONGPRESS_LEN*2);

void setup()
{
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.createChar(1, p1);
  lcd.createChar(2, p2);
  lcd.createChar(3, p3);
  lcd.createChar(4, p4);
  lcd.createChar(5, p5);

  lcd.begin(16, 4);
  
  // Print a message to the LCD.
  
  //delay(500);
  printLcd();
  lcd.setCursor(4,3);
  lcd.print('U');
  lcd.setCursor(8,3);
  lcd.print('D');
  
  // init buttons pins; I suppose it's best to do here
  button1.init();
  button2.init();
  button3.init();
  button4.init();
  button5.init();
  button6.init();
  
  //init leds
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  
  pinMode(ledU, OUTPUT);
  pinMode(ledD, OUTPUT);
  digitalWrite(ledD, LOW);
  digitalWrite(ledU, LOW);
}

void print_event(const char* button_name, int event)
{
  //check if button has been pressed
  if (event){
  
    if(button_name == "1") {  // Ethics button
       ethicsCnt++;
      
       compQuestioning();
       compEmancipation();
       compJustice();
    }
    else if(button_name == "2") {  // Logic button
      logicCnt++;
      
      compJustice();
      compTechnology();
    }
    else if(button_name == "3") {  // Mathematics button
      mathCnt++;
      
      compSatisfaction();
      compTechnology();
    }
    else if(button_name == "4") {  // Perception button
      perceptCnt++;
      
      compJustice();
      compQuestioning();
      compEmancipation();
    }
    else if(button_name == "5") {  // Mythology
      mythCnt++;
      
      compTraditioning();
      compArts();
    }
    else if(button_name == "6") {  // Rhetoric button
      rhetoricCnt++;
      
      compTraditioning();
      compArts();
      compSatisfaction();
    }
    
    //Serial.print(button_name);
    
    
  }
  //Serial.print(".SL"[event]);
}

///////////////////////////////////////////////////////////////////////////////////////////////
char* fTs(char * outstr, float value, int places, int minwidth=0, bool rightjustify=false) {
    // this is used to write a float value to string, outstr.  oustr is also the return value.
    int digit;
    float tens = 0.1;
    int tenscount = 0;
    int i;
    float tempfloat = value;
    int c = 0;
    int charcount = 1;
    int extra = 0;
    // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
    // if this rounding step isn't here, the value  54.321 prints as 54.3209

    // calculate rounding term d:   0.5/pow(10,places)  
    float d = 0.5;
    if (value < 0)
        d *= -1.0;
    // divide by ten for each decimal place
    for (i = 0; i < places; i++)
        d/= 10.0;    
    // this small addition, combined with truncation will round our values properly 
    tempfloat +=  d;

    // first get value tens to be the large power of ten less than value    
    if (value < 0)
        tempfloat *= -1.0;
    while ((tens * 10.0) <= tempfloat) {
        tens *= 10.0;
        tenscount += 1;
    }

    if (tenscount > 0)
        charcount += tenscount;
    else
        charcount += 1;

    if (value < 0)
        charcount += 1;
    charcount += 1 + places;

    minwidth += 1; // both count the null final character
    if (minwidth > charcount){        
        extra = minwidth - charcount;
        charcount = minwidth;
    }

    if (extra > 0 and rightjustify) {
        for (int i = 0; i< extra; i++) {
            outstr[c++] = ' ';
        }
    }

    // write out the negative if needed
    if (value < 0)
        outstr[c++] = '-';

    if (tenscount == 0) 
        outstr[c++] = '0';

    for (i=0; i< tenscount; i++) {
        digit = (int) (tempfloat/tens);
        itoa(digit, &outstr[c++], 10);
        tempfloat = tempfloat - ((float)digit * tens);
        tens /= 10.0;
    }

    // if no places after decimal, stop now and return

    // otherwise, write the point and continue on
    if (places > 0)
    outstr[c++] = '.';


    // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
    for (i = 0; i < places; i++) {
        tempfloat *= 10.0; 
        digit = (int) tempfloat;
        itoa(digit, &outstr[c++], 10);
        // once written, subtract off that digit
        tempfloat = tempfloat - (float) digit; 
    }
    if (extra > 0 and not rightjustify) {
        for (int i = 0; i< extra; i++) {
            outstr[c++] = ' ';
        }
    }


    outstr[c++] = '\0';
    return outstr;
}

///////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////    INFLUENCE SOCIETY CHARACTERISTICS  FUNCTIONS    /////////////////////

void printResult( int col, int row, float val )
{
  char buff[15];
  lcd.setCursor( col, row );
  
  if( val > 99.9 ){
    
    lcd.print(fTs(buff, val, 0));
    lcd.print(" ");
  }
  else{
    
    lcd.print(fTs(buff, val, 1));
  }
  
}

float art,quest,trad,eman,jus,sat,tech;

void compQuestioning()
{
  quest = (ethicsCnt + perceptCnt)*infFactor;
  
  printResult( 4, 0, quest );
  compResources();
}

void compTraditioning() 
{
  
  trad = (mythCnt + rhetoricCnt)*infFactor;
  printResult( 0, 2, trad );
  compResources();
}

void compEmancipation() 
{
  eman = (ethicsCnt + perceptCnt)*infFactor;
  printResult( 12, 0, eman );
  compResources();
}

void compJustice() 
{
  jus = (logicCnt + ethicsCnt + perceptCnt)*infFactor;
  printResult( 8, 2, jus );
  compResources();
}

void compSatisfaction() 
{
  sat = (rhetoricCnt + mathCnt)*infFactor;
  printResult( 4, 1, sat );
  compResources();
}

void compTechnology() 
{
  tech = (logicCnt + mathCnt)*infFactor;
  printResult( 0, 3, tech );
  compResources();
}

void compArts()
{
  art = (mythCnt + rhetoricCnt)*infFactor;
  printResult( 12, 1, art );
}

void printLcd()
{
  lcd.clear();
  
  for(int i=0; i < 14; i++){
    
    if( i == 4 ){
      lcd.setCursor(0, 1);
    }
    
    if( i ==8 ){
      lcd.setCursor(-4, 2);
    }
    
    if(  i == 12  ){
      lcd.setCursor(-4, 3);
    }
    
    lcd.print(myStrings[i]);
  }
}

void compResources()
{
  compUtopia();
  compDistopia();  
}

void letItBlink( int ledValue, int del=500 )
{
  for(int i = 0; i< 10; i++)
  {  
    digitalWrite(ledValue, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(del);                     // wait for a second
    digitalWrite(ledValue, LOW);    // turn the LED off by making the voltage LOW
    delay(del);
  }
}

int distopiaLedCnt = 1;
int utopiaLedCnt = 1;

void compUtopia()
{
  utopia = quest + eman + jus + sat + tech;
  
  if(utopia > (utopiaLedCnt * MAX_BAR_VALUE) )
  {
      //Reset UTOPIA's loader
      printProgressBar( 5, utopia);
      letItBlink(ledU, 250);

      utopia = utopia - ( utopiaLedCnt * MAX_BAR_VALUE );
      utopiaLedCnt++;
      
      cleanProgressBar(5);
  }
  else if(utopia > MAX_BAR_VALUE)
  {
    utopia = utopia - ( (utopiaLedCnt - 1) * MAX_BAR_VALUE );
    printProgressBar( 5, utopia);
  }
  else
  {
    printProgressBar( 5, utopia);
  }
}

void compDistopia()
{
  distopia = trad + art + sat + tech;
   
  if(distopia > (distopiaLedCnt * MAX_BAR_VALUE) )
  {
      //Reset distopia's loader
      printProgressBar( 9, distopia);
      letItBlink(ledD, 250);

      distopia = distopia - ( distopiaLedCnt * MAX_BAR_VALUE );
      distopiaLedCnt++;
      
      cleanProgressBar(9);
  }
  else if(distopia > MAX_BAR_VALUE)
  {
    distopia = distopia - ( (distopiaLedCnt - 1) * MAX_BAR_VALUE );
    printProgressBar( 9, distopia);
  }
  else
  {
    printProgressBar( 9, distopia);
  } 
}

void cleanProgressBar(int col)
{
  lcd.setCursor( col , 3 );
  lcd.print(" ");
  lcd.setCursor( col + 1 , 3 );
  lcd.print(" ");
  lcd.setCursor( col + 2 , 3 );
  lcd.print(" ");
}

void printProgressBar( int col, float floatVal )
{
  float x = (MAX_BAR_VALUE/MAX_BAR_COLS);
  int res = floatVal/x;
    
  for(int i=0; i<res; i++)
  {
    if(i < 5)
    {
      lcd.setCursor( col , 3 );
    }
    else if( i < 10)
    {
      lcd.setCursor( col + 1, 3 );      
    }
    else
    {
      lcd.setCursor( col + 2, 3 );
    }
    
    int b = i + 1;
    
    if(( b % 5 ) == 0)
    {
      lcd.write(5);
    }
    else
    {
      lcd.write(b % 5);
    }

  }
}

void loop()
{
  // handle button
  int event1 = button1.handle();
  int event2 = button2.handle();
  int event3 = button3.handle();
  int event4 = button4.handle();
  int event5 = button5.handle();
  int event6 = button6.handle();

  // do other things
  print_event("1", event1);
  print_event("2", event2);
  print_event("3", event3);
  print_event("4", event4);
  print_event("5", event5);
  print_event("6", event6);
}

///////////////////////////////////////////////////////////////////////////////////////////////