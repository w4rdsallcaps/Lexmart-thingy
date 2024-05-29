#include <LiquidCrystal.h>

// Initialize the library with the numbers of the interface pins
// RS -> 2, EN -> 3, D4 -> 4, D5 -> 5, D6 -> 6, D7 -> 8
LiquidCrystal lcd(2, 3, 4, 5, 6, 8);

#define PIN_RELAY_1 7  // Pin used to control the relay
#define PIN_RELAY_2 9  // Pin used to detect the input current
// Variable to store the loop count
unsigned long loopCount = 0;

void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Print a message to the LCD
  lcd.setCursor(0, 0);  // Set cursor to the first column of the first row
  lcd.print("Loop Count:");  // Print the initial message

  // Initialize the relay pin as an output
  pinMode(PIN_RELAY_1, OUTPUT);

  // Initialize PIN_RELAY_2 as an input with internal pull-up resistor
  pinMode(PIN_RELAY_2, INPUT_PULLUP);

  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Check if PIN_RELAY_2 is LOW (button pressed)
  if (digitalRead(PIN_RELAY_2) == LOW) {
    // Display the names when PIN_RELAY_2 is LOW
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Powered by: ");

    // Prepare the message to scroll
    String names = "Kineth Noval, Jemuel Valencia, Ellaine Barro, Louie Valle, Miguel Hermoso";
    int namesLength = names.length();
    String paddedNames = names + "                "; // 16 spaces for padding

    // Scroll the names on the second line
    for (int position = 0; position < namesLength + 16; position++) {
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
  } else {
    // Resume the relay operation and stop displaying the names
    // Turn on the relay
    Serial.println("Turn on the relay");
    digitalWrite(PIN_RELAY_1, HIGH);
    delay(500);

    // Turn off the relay
    Serial.println("Turn off the relay");
    digitalWrite(PIN_RELAY_1, LOW);
    delay(500);

    // Increment the loop count
    loopCount++;

    // Clear the second line
    lcd.setCursor(0, 1);  // Set cursor to the first column of the second row
    lcd.print("                ");  // Print spaces to clear previous content

    // Set the cursor to the second line again to print the updated count
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(loopCount);
  }
}
