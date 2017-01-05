#define S1 A3 
#define S2 13 
#define S3 12 
#define S4 11 
#define S5 10 

#define DE A1 

#define DEBUG_ENABLED true 

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String thisDev = "dev01";
unsigned long lastTime;
unsigned long timeToStop = 0;

String nullStr = "";

void setup() {

//  Serial.begin(300);
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT);
  pinMode(S5, OUTPUT);
  digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW

  pinMode(DE, OUTPUT);
  digitalWrite(DE, LOW);    // turn the LED off by making the voltage LOW


  lastTime = millis();
  timeToStop = 0;
}

void stop() {
  digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
}

void start(int gate, long ttl) {
  if (DEBUG_ENABLED) {
    Serial.println();
    Serial.print("Staring gate ");
    Serial.print(gate);
    Serial.print(" for ");
    Serial.println(ttl);
  }
  switch (gate) {
    case 1:
      digitalWrite(S1, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      break;
    case 2:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      break;
    case 3:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      break;
    case 4:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      break;
    case 5:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, HIGH);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
    break;
  }
}

void useTime(unsigned long t) {
  if (timeToStop > t) {
    timeToStop = timeToStop - t;
  } else {
    timeToStop = 0;
  }
}

void loop() {
  // print the string when a newline arrives:
  unsigned long time = millis();
  if (timeToStop > 0){
    useTime(time - lastTime);
//    Serial.print("Time to stop: ");
//    Serial.print(timeToStop);
//    Serial.println();
    if (timeToStop == 0) {
      stop();
      Serial.println("Stopped all");
    }
  }
  lastTime = time;
  
  if (stringComplete) {
//    Serial.println(inputString); 
    processMessage(inputString); 
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  delay(300);
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void logMessageToken(String token, String val) {
    Serial.print(token); 
    Serial.print(": \""); 
    Serial.print(val); 
    Serial.print("\" ");
}

int lastDelim;
int delim;

void initTokenizer() {
  lastDelim = -1;
  delim = -1;
}

String nextToken(String msg, String tokenName) {
  if (lastDelim + 1 > msg.length() - 1) 
    return nullStr;
  int delim = msg.indexOf(' ', lastDelim + 1);
  if (delim < 0) 
    return nullStr;
  String ret = msg.substring(lastDelim + 1,delim);
  lastDelim = delim;
  if (DEBUG_ENABLED) {
    logMessageToken(tokenName, ret);
  } 
}

void processMessage(String msg) {
  initTokenizer();
  String dev = nextToken(msg, "DEV");
  if (dev.equals(nullStr)) 
    return;
//  if (!dev.equals(thisDev) || dev.equals("")) 
//    return;

  String cmd = nextToken(msg, "CMD");
  if (cmd.equals(nullStr)) 
    return;

  String gateStr = nextToken(msg, "GATE");
  if (gateStr.equals(nullStr)) 
    return;
  int gate = gateStr.toInt();

  String timeStr = nextToken(msg, "TTL");
  if (timeStr.equals(nullStr)) 
    return;
  long time = timeStr.toInt();
    
  start(gate, time);
}

