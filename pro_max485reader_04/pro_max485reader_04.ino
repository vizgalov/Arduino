#define S1 A3 
#define S2 13 
#define S3 12 
#define S4 11 
#define S5 10 

#define DE A1 

#define DEBUG_ENABLED false 

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String thisDev = "dev01";
unsigned long lastTime;
unsigned long timeToStop = 0;

void setup() {

//  Serial.begin(300);
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(256);
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

int stop() {
  
  timeToStop = 1000;
  
  return 0;
}

void startDO() {
  digitalWrite(DE, HIGH);    
}

void stopDO() {
  Serial.flush();
  delay(1);
  digitalWrite(DE, LOW);    
}

void stopInt() {
  digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
}


int start(int gate, long ttl) {
  if (DEBUG_ENABLED) {
    Serial.println();
    Serial.print("Staring gate ");
    Serial.print(gate);
    Serial.print(" for ");
    Serial.println(ttl);
  }
  int ret = -1;
  switch (gate) {
    case 1:
      digitalWrite(S1, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      ret = 1;
      break;
    case 2:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      ret = 2;
      break;
    case 3:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      ret = 3;
      break;
    case 4:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, HIGH);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, LOW);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      ret = 4;
      break;
    case 5:
      digitalWrite(S1, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S2, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S3, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S4, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(S5, HIGH);    // turn the LED off by making the voltage LOW
      timeToStop = ttl;
      ret = 5;
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  return ret;
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
    if (DEBUG_ENABLED) {
      Serial.print("Time to stop: ");
      Serial.print(timeToStop);
      Serial.println();
    }
    if (timeToStop == 0) {
      stopInt();
      if (DEBUG_ENABLED) {
        Serial.println("Stopped all");
      }
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

void processMessage(String msg) {
  int delim = msg.indexOf(' ');
  if (delim < 0) 
    return;
  String dev = msg.substring(0,delim);    
  if (DEBUG_ENABLED) {
    Serial.println(); 
    Serial.print("DEV: \""); 
    Serial.print(dev); 
    Serial.print("\"");
  } 
  if (!dev.equals(thisDev)) 
    return;

  if (delim + 1 > msg.length() - 1) 
    return;
  int delim1 = msg.indexOf(' ', delim + 1);
  if (delim1 < delim + 2) 
    return;
  String cmd = msg.substring(delim + 1, delim1);
  if (DEBUG_ENABLED) {
    Serial.print("CMD: \""); 
    Serial.print(cmd); 
    Serial.print("\"");
  } 

  if (cmd.equals("start")) { 
    if (delim1 + 1 > msg.length() - 1) 
      return;
    int delim2 = msg.indexOf(' ', delim1 + 1);
    if (delim2 < 0) 
      return;
    int gate = msg.substring(delim1 + 1, delim2).toInt();
    if (DEBUG_ENABLED) {
      Serial.print("GATE: \""); 
      Serial.print(gate); 
      Serial.print("\"");
    } 
  
    if (delim2 + 1 > msg.length() - 1) 
      return;
    int delim3 = msg.indexOf('\n', delim2 + 1);
    if (delim3 < 0) 
      return;
    long time = msg.substring(delim2 + 1, delim3).toInt();
    if (DEBUG_ENABLED) {
      Serial.print("TTL: \""); 
      Serial.print(time); 
      Serial.print("\""); 
      Serial.println();
    } 
  
    int ret = start(gate, time*1000);
    sendStartResponse(dev, cmd, gate, time, ret);
  }
  if (cmd.equals("stop")) {
    int ret = stop();
    sendStopResponse(dev, cmd, ret);
  } 

}

void sendStartResponse(String dev, String cmd, int gate, long time, int resp) {
  startDO();
  Serial.println();
  Serial.print("RESP "); 
  Serial.print(dev); 
  Serial.print(" "); 
  Serial.print(cmd); 
  Serial.print(" "); 
  Serial.print(gate); 
  Serial.print(" "); 
  Serial.print(time); 
  Serial.print(" "); 
  Serial.print(resp); 
  Serial.println(); 
  stopDO();
}

void sendStopResponse(String dev, String cmd, int resp) {
  startDO();
  Serial.println();
  Serial.print("RESP "); 
  Serial.print(dev); 
  Serial.print(" "); 
  Serial.print(cmd); 
  Serial.print(" "); 
  Serial.print(resp); 
  Serial.println(); 
  stopDO();
}

