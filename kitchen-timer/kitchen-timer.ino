#include <LiquidCrystal.h>
#include <EEPROM.h>

enum pins {
	LCD_RS = 7,
	LCD_E = 8,
	LCD_DB4 = 9,
	LCD_DB5 = 10,
	LCD_DB6 = 11,
	LCD_DB7 = 12,
	BUTTON1 = 3,
	BUTTON2 = 2,
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

enum battery_icons {
	BAT_FULL_ICON,
	BAT_HIGH_ICON,
	BAT_LOW_ICON,
	BAT_EMPTY_ICON,
};

enum battery_icon_pos {
	BAT_ICON_COL = 7,
	BAT_ICON_ROW = 0,
};

/* Voltage * 100 to deal with int instead of float */
enum battery_voltage {
	BAT_FULL_VOLT = 380,
	BAT_HIGH_VOLT = 370,
	BAT_LOW_VOLT = 360,
	BAT_EMPTY_VOLT = 350,
};

byte bat_full[8] = {
	B01110,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
};

byte bat_high[8] = {
	B01110,
	B10001,
	B10001,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
};

byte bat_low[8] = {
	B01110,
	B10001,
	B10001,
	B10001,
	B10001,
	B11111,
	B11111,
	B11111,
};

byte bat_empty[8] = {
	B01110,
	B10001,
	B10001,
	B10001,
	B10001,
	B10001,
	B10001,
	B11111,
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
int battery_voltage(void);
void display_battery(void);

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);
int alarm_minutes, alarm_seconds;
int minutes, seconds;
boolean first_press;
char time_string[6];

void setup() {
	pinMode(BUTTON1, INPUT);
	pinMode(BUTTON2, INPUT);

	lcd.begin(8, 2);
	lcd.createChar(BAT_FULL_ICON, bat_full);
	lcd.createChar(BAT_HIGH_ICON, bat_high);
	lcd.createChar(BAT_LOW_ICON, bat_low);
	lcd.createChar(BAT_EMPTY_ICON, bat_empty);
	lcd.clear();

	EEPROM.get(MINUTES_ADDRESS, alarm_minutes);
	EEPROM.get(SECONDS_ADDRESS, alarm_seconds);

	display_alarm_time();
	display_battery();

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
			display_battery();
			delay(100);
		}
		break;
	case RUNNING:
		lcd.clear();
		display_alarm_time();
		display_battery();
		lcd.setCursor(0, 1);
		sprintf(time_string, "%02d:%02d", minutes, seconds);
		lcd.print(time_string);
		
		delay(1000);
		count_down();
		
		if (minutes <= 0 && seconds <= 0) {
			lcd.clear();
			display_alarm_time();
			display_battery();
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

/* Return battery voltage * 100 (to deal with int instead of float) */
int battery_voltage(void) {
	float mult = (5.0 / 1023.0) * 100.0;
	float volt = (float) analogRead(BATTERY) * mult;
	return (int) volt;
}

void display_battery(void) {
	float voltage = battery_voltage();

	lcd.setCursor(BAT_ICON_COL, BAT_ICON_ROW);
	
	if (voltage >= BAT_FULL_VOLT) {
		lcd.write(byte(BAT_FULL_ICON));
	} else if (voltage >= BAT_HIGH_VOLT) {
		lcd.write(byte(BAT_HIGH_ICON));
	} else if (voltage >= BAT_LOW_VOLT) {
		lcd.write(byte(BAT_LOW_ICON));
	} else {
		lcd.write(byte(BAT_EMPTY_ICON));
	}
}
