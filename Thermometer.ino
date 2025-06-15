#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// LCD init (address may be 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Thermistor setup
#define THERMISTOR_PIN A0
#define SERIES_RESISTOR 20000  // You can change to 10K for better range

// Thermistor characteristics
#define THERMISTOR_NOMINAL 10000    // Resistance at 25C
#define TEMPERATURE_NOMINAL 25      // Nominal temperature
#define B_COEFFICIENT 3950          // Beta value
#define ADC_MAX 1023.0              // 10-bit ADC max

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1000);
}

void loop() {
  int adcValue = analogRead(THERMISTOR_PIN);

  // Avoid edge case where math breaks
  if (adcValue <= 0 || adcValue >= 1023) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor error");
    Serial.println("ADC error: Out of range");
    delay(1000);
    return;
  }

  // Calculate resistance of thermistor
  float voltageRatio = ADC_MAX / adcValue - 1.0;
  float resistance = SERIES_RESISTOR / voltageRatio;

  // Steinhart-Hart calculation
  float steinhart;
  steinhart = resistance / THERMISTOR_NOMINAL;
  steinhart = log(steinhart);
  steinhart /= B_COEFFICIENT;
  steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  // Show on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(steinhart, 1);
  lcd.print((char)223); // degree symbol
  lcd.print("C");

  // Debug info (optional)
  Serial.print("ADC: "); Serial.print(adcValue);
  Serial.print(" | R: "); Serial.print(resistance);
  Serial.print(" ohms | Temp: "); Serial.print(steinhart);
  Serial.println(" C");

  delay(1000);
}
