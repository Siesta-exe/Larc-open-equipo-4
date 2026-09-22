void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  int ADC_SHARP = analogRead(34);

  Serial.print("ADC: ");
  Serial.println(ADC_SHARP);

  delay(200);
}