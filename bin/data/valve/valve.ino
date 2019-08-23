
void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  Serial.begin(9600);

  digitalWrite(3, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  char sread = Serial.read();
  if (sread == 'o') {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }

  else if (sread == 'c') {
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
  }

  else if (sread == 'q') { //echo !!
    Serial.write('w');
  }
}
