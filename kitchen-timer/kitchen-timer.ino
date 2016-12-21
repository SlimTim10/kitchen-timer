#include <LiquidCrystal.h>
#include <EEPROM.h>

enum pins {
	LCD_RS = 7,
	LCD_E = 8,
	LCD_DB4 = 9,
	LCD_DB5 = 10,
	LCD_DB6 = 11,
	LCD_DB7 = 12,
	BUTTON1 = 2,
	BUTTON2 = 3,
	BUZZER = 5,
	BATTERY = 0,
};

enum config {
	TONE_FREQ = 2110,
	BEEPS = 4,
	BEEP_DELAY = 100,
	BEEP_GROUPS = 3,
	BEEP_GROUP_DELAY = 1000,
};

enum constants {
	DEBOUNCE_MS = 50,
	MINUTES_ADDRESS = 0,
	SECONDS_ADDRESS = 0 + sizeof(int),
};

enum State {
	IDLE = 0,
	RUNNING,
} state;

void count_down(void);
void display_alarm_time(void);
void alarm(void);
void increase_alarm_time(void);
boolean button_press(uint8_t pin);
float battery_voltage(void);

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);
int alarm_minutes, alarm_seconds;
int minutes, seconds;
boolean first_press;
char time_string[6];

void setup() {
	pinMode(BUTTON1, INPUT);
	pinMode(BUTTON2, INPUT);
	
	lcd.begin(8, 2);
	lcd.clear();

	EEPROM.get(MINUTES_ADDRESS, alarm_minutes);
	EEPROM.get(SECONDS_ADDRESS, alarm_seconds);

	display_alarm_time();

	float voltage = battery_voltage();

	first_press = true;

	state = IDLE;
}

void loop() {
	switch (state) {
	case IDLE:
		if (button_press(BUTTON1)) {
			EEPROM.put(MINUTES_ADDRESS, alarm_minutes);
			EEPROM.put(SECONDS_ADDRESS, alarm_seconds);

			minutes = alarm_minutes;
			seconds = alarm_seconds;
			
			state = RUNNING;
		} else if (button_press(BUTTON2)) {
			if (first_press) {
				alarm_minutes = 0;
				alarm_seconds = 0;
				first_press = false;
			}
			increase_alarm_time();
			lcd.clear();
			display_alarm_time();
			delay(100);
		}
		break;
	case RUNNING:
		lcd.clear();
		display_alarm_time();
		lcd.setCursor(0, 1);
		sprintf(time_string, "%02d:%02d", minutes, seconds);
		lcd.print(time_string);
		
		delay(1000);
		count_down();
		
		if (minutes <= 0 && seconds <= 0) {
			lcd.clear();
			display_alarm_time();
			lcd.setCursor(0, 1);
			lcd.print("!!!!");
			alarm();
			
			state = IDLE;
		}
		break;
	default:
		lcd.clear();
		lcd.print("ERROR");
		break;
	}
}

void count_down(void) {
	seconds--;
	if (seconds < 0) {
		minutes--;
		seconds = 59;
	}
}

void display_alarm_time(void) {
	lcd.setCursor(0, 0);
	sprintf(time_string, "%02d:%02d", alarm_minutes, alarm_seconds);
	lcd.print(time_string);
}

void alarm(void) {
	for (uint8_t j = 0; j < BEEP_GROUPS; j++) {
		for (uint8_t i = 0; i < BEEPS; i++) {
			tone(BUZZER, TONE_FREQ);
			delay(BEEP_DELAY);
			noTone(BUZZER);
			delay(BEEP_DELAY);
		}
		delay(BEEP_GROUP_DELAY);
	}
}

void increase_alarm_time(void) {
	alarm_minutes++;
}

boolean button_press(uint8_t pin) {
	if (digitalRead(pin) == true) {
		delay(DEBOUNCE_MS);
		if (digitalRead(pin) == true) {
			return true;
		}
	}
	return false;
}

float battery_voltage(void) {
	return analogRead(BATTERY) * (5.0 / 1023.0);
}
