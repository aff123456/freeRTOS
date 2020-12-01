int randNumber;

void setup() {
  Serial.begin(115200);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
}

void loop() {
  // print a random number from 0 to 299
  randNumber = random(100);
  Serial.println(randNumber);

/*
  // print a random number from 10 to 19
  randNumber = random(10, 20);
  Serial.println(randNumber);
*/
  if(checkPrime(randNumber)) {
    Serial.println("PRIMO!");
  }
  delay(100);
}

bool checkPrime(int number) {
  int numberDiv = 0;
  for(int i = 1; i <= number; i++) {
    if(number % i == 0) {
      numberDiv += 1;
      }
    }
  if(numberDiv == 2) {
    return true;
  } else {
    return false;
  }
}
