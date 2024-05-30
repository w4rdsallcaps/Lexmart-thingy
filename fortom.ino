#include <LiquidCrystal.h>
#include <EEPROM.h> // Include the EEPROM library

// Initialize the library with the numbers of the interface pins
// RS -> 2, EN -> 3, D4 -> 4, D5 -> 5, D6 -> 6, D7 -> 8
LiquidCrystal lcd(10, 3, 4, 5, 6, 8);

#define PIN_RELAY_1 7  // Pin used to control the relay
#define PIN_RELAY_2 9  // Pin used to detect the input current
#define PIN_RESET_BUTTON 2 // Pin used for the reset button

// Variable to store the loop count
unsigned long loopCount = 0;
volatile bool reset = false;

// Variable to store LCD contrast value
int contrast = 60;

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);

  // Initialize the relay pin as an output
  pinMode(PIN_RELAY_1, OUTPUT);

  // Initialize PIN_RELAY_2 as an input with internal pull-up resistor
  pinMode(PIN_RELAY_2, INPUT_PULLUP);

  // Initialize the reset button pin as an input with internal pull-up resistor
  pinMode(PIN_RESET_BUTTON, INPUT_PULLUP);

  // Read the loop count from EEPROM
  loopCount = EEPROM.read(0) | (EEPROM.read(1) << 8) | (EEPROM.read(2) << 16) | (EEPROM.read(3) << 24);

  // Set up reset button interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_RESET_BUTTON), resetCount, FALLING);

  // Set up the LCD's number of columns and rows
  analogWrite(11, contrast);
  lcd.begin(16, 2);

  // Print the initial message to the LCD
  lcd.setCursor(0, 0);  // Set cursor to the first column of the first row
  lcd.print("Loop Count:");  // Print the initial message

  // Display the initial loop count on the LCD
  lcd.setCursor(0, 1);
  lcd.print(loopCount);
}

void loop() {
  // Check if the reset button is pressed
  if (reset) {
    // Reset the loop count
    loopCount = 0;

    // Save the reset loop count to EEPROM
    EEPROM.write(0, loopCount & 0xFF);
    EEPROM.write(1, (loopCount >> 8) & 0xFF);
    EEPROM.write(2, (loopCount >> 16) & 0xFF);
    EEPROM.write(3, (loopCount >> 24) & 0xFF);

    // Update the display immediately after reset
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loop Count:");
    lcd.setCursor(0, 1);
    lcd.print(loopCount);
    reset = false;
  } else if (digitalRead(PIN_RELAY_2) == LOW) {
    // Prepare the scrolling messages
    String topMessage = "Powered by the Circuit Crusader";
    String names = "Kineth Noval, Jemuel Valencia, Ellaine Barro, Louie Valle, Miguel Hermoso";
    String paddedTopMessage = topMessage + "                "; // 16 spaces for padding
    String paddedNames = names + "                "; // 16 spaces for padding

    int topMessageLength = paddedTopMessage.length();
    int namesLength = paddedNames.length();
    int maxLength = max(topMessageLength, namesLength);

    // Scroll the names on the second line
    for (int position = 0; position < maxLength; position++) {
      lcd.setCursor(0, 0);
      lcd.print(paddedTopMessage.substring(position, position + 16));
      lcd.setCursor(0, 1);
      lcd.print(paddedNames.substring(position, position + 16));
      delay(150);

      if (digitalRead(PIN_RELAY_2) == HIGH) {
        break; // Exit the loop if the switch is released
      }
    }

    // Clear the display immediately after scrolling
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loop Count:");
    lcd.setCursor(0, 1);
    lcd.print(loopCount);
  } else {
    // Resume the relay operation and stop displaying the scrolling names
    // Turn on the relay
    Serial.println("pull");
    digitalWrite(PIN_RELAY_1, HIGH);
    delay(1500);

    // Turn off the relay
    Serial.println("push");
    digitalWrite(PIN_RELAY_1, LOW);
    delay(1500);

    // Increment the loop count
    loopCount++;

    // Save the loop count to EEPROM
    EEPROM.write(0, loopCount & 0xFF);
    EEPROM.write(1, (loopCount >> 8) & 0xFF);
    EEPROM.write(2, (loopCount >> 16) & 0xFF);
    EEPROM.write(3, (loopCount >> 24) & 0xFF);

    // Clear the second line
    lcd.setCursor(0, 1);  // Set cursor to the first column of the second row
    lcd.print("                ");  // Print spaces to clear previous content

    // Set the cursor to the second line again to print the updated count
    lcd.setCursor(0, 1);
    lcd.print(loopCount);
  }
}

void resetCount() {
  reset = true;
}
