unsigned long lastStepTime = 0;
int stepLength = 1000;
long lastDelayTime = 0;
long delayTime = 50;
int ledPinArray[4] = { 7, 8, 9, 10 };
int buttonPinArray[4] = { 33, 34, 35, 37 };
int lastButtonStateArray[4] = { LOW, LOW, LOW, LOW };
int buttonStateArray[4] = { LOW, LOW, LOW, LOW };
int currentStateArray[4] = { 0, 0, 0, 0 };
int totalSteps = 4;
int currentStep = 0;
int channelButtonPin = 36;
int channelDisplayed = 0;
int currentChannelState = LOW;
int lastButtonChannelState = LOW;
int ButtonChannelState = LOW;
int channelButtonOn = false;
int channelLedPinArray[3] = { 39, 40, 41 };
bool buttonOnArray[3][4] = {
  { LOW, LOW, LOW, LOW },
  { LOW, LOW, LOW, LOW },
  { LOW, LOW, LOW, LOW }
};
int midiNotes[3] = { 36, 38, 42 };

void setup() {
  Serial.begin(9600);
  pinMode(channelButtonPin, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPinArray[i], OUTPUT);
    pinMode(buttonPinArray[i], INPUT);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(channelLedPinArray[i], OUTPUT);
  }
}

void loop() {
  sequence();
  checkStepButton();
  checkChannelButton();
  checkLed();
  checkChannelLeds();
}

void checkStepButton() {
  for (int i = 0; i < 4; i++) {
    currentStateArray[i] = digitalRead(buttonPinArray[i]);
    if (currentStateArray[i] != lastButtonStateArray[i]) {
      lastDelayTime = millis();
    }
    if ((millis() - lastDelayTime) > delayTime) {
      if (currentStateArray[i] != buttonStateArray[i]) {
        buttonStateArray[i] = currentStateArray[i];

        if (buttonStateArray[i] == HIGH) {
          buttonOnArray[channelDisplayed][i] = !buttonOnArray[channelDisplayed][i];
        }
      }
    }
    lastButtonStateArray[i] = currentStateArray[i];
  }
}

void checkChannelButton() {
  lastButtonChannelState = ButtonChannelState;
  ButtonChannelState = digitalRead(channelButtonPin);

  if (lastButtonChannelState == LOW and ButtonChannelState == HIGH) {
    channelButtonOn = !channelButtonOn;
    Serial.println(channelDisplayed);
    channelDisplayed += 1;
    delay(5);
  } else if (lastButtonChannelState == LOW and ButtonChannelState == LOW) {
    delay(5);
  }
  if (channelDisplayed > 2) {
    channelDisplayed = 0;
  }
}

void nextStep() {
  currentStep = currentStep + 1;
  if (currentStep > 3) {
    currentStep = 0;
  }
}

void sequence() {
  if (millis() > lastStepTime + stepLength) {
    lastStepTime = millis();
    nextStep();
    for (int i = 0; i < 3; i++) {
    usbMIDI.sendNoteOff(midiNotes[i], 127, 1);
      if (buttonOnArray[i][currentStep] == HIGH) {
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}

void checkLed() {
  for (int i = 0; i < 4; i++) {
    if (currentStep == i) {
      analogWrite(ledPinArray[i], 255);
    } else if (buttonOnArray[channelDisplayed][i] == true) {
      analogWrite(ledPinArray[i], 75);
    } else {
      analogWrite(ledPinArray[i], 5);
    }
  }
}

void checkChannelLeds() {
  for (int i = 0; i < 3; i++) {
    if (channelDisplayed == i) {
      digitalWrite(channelLedPinArray[i], HIGH);
    } else {
      digitalWrite(channelLedPinArray[i], LOW);
    }
  }
}