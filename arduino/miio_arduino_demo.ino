#include <string.h>
#include "DHT.h"

/* macros for MIIO */
#define MODEL "model miot.light.demo1"
#define VERSION "mcu_version 0001"
#define GET_DOWN "get_down"


#define INTERNAL_GET_DOWN 300   /* MIIO command string poll internal */
#define INTERNAL_CHECK_DHT 1000 /* DHT11 check internal */
#define INTERNAL_BUTTON_PRESS 200
#define INTERNAL_BUTTON_LONG_PRESS 4000

/* string for MIIO command */
String miio_string = "";
volatile boolean miio_string_received = false;

/* pins of rgb */
int red_pin = 9;
int green_pin = 10;
int blue_pin = 11;
long rgb_value = 0;

/* pins of dht */
#define DHTPIN 4
#define DHTTYPE DHT11
float tmpValue = 0;
float humValue = 0;
DHT dht(DHTPIN, DHTTYPE);

/* pins of button */
int button_pin = 7;
int button_is_pressed = 0;
int has_reported = 0;
int has_reported_long = 0;

/* model & mcu_version send flag */
int model_sended = 0;
int mcu_version_sended = 0;

/* count timers */
long poll_timer, poll_timer_now, dht_timer, dht_timer_now, button_timer, button_timer_now;

void setup()
{
	Serial.begin(115200);

	/* init rgb pins */
	pinMode(red_pin, OUTPUT);
	pinMode(green_pin, OUTPUT);
	pinMode(blue_pin, OUTPUT);
	set_rgb(0);

	/* init button pin */
	pinMode(button_pin, INPUT);

	/* init timers */
	poll_timer = millis();
	dht_timer = millis();
	button_timer = millis();

	dht.begin();
}

void loop()
{
	/* send model & mcu_version*/
	if (model_sended < 2)
	{ /* hasn't received ack */
		if (model_sended < 1)
		{ /* hasn't sended */
			Serial.println(MODEL);
			model_sended = 1;	/* sended */
		}
		if (miio_string_received)
		{
			if (0 == string_handle())
				model_sended = 2;	/* received ack */
			else
				model_sended = 0;
		}
	}

	if (mcu_version_sended < 2)
	{ /* hasn't received ack */
		if (mcu_version_sended < 1)
		{ /* hasn't sended */
			Serial.println(VERSION);
			mcu_version_sended = 1;	/* sended */
		}
		if (miio_string_received)
		{
			if (0 == string_handle())
				mcu_version_sended = 2;	/* received ack */
			else
				mcu_version_sended = 0;
		}
	}

	/* get command string */
	poll_timer_now = millis();
	if (poll_timer_now - poll_timer > INTERNAL_GET_DOWN)
	{
		poll_timer = poll_timer_now;
		Serial.println(GET_DOWN);
		if (miio_string_received)
		{
			string_handle();
		}
	}

	/* check dht value */
	dht_timer_now = millis();
	if (dht_timer_now - dht_timer > INTERNAL_CHECK_DHT)
	{
		dht_timer = dht_timer_now;
		dht_check();
	}

	button_timer_now = millis();
	if (HIGH == digitalRead(button_pin))
	{ /* button is pressed */
		if (!button_is_pressed)
		{ /* judge if button is pressed for the first time */
			button_is_pressed = 1;
			button_timer = button_timer_now;
			has_reported = 0;
			has_reported_long = 0;
		}
		else
		{
			if (button_timer_now - button_timer > INTERNAL_BUTTON_PRESS && !has_reported)
			{ /* report short button event */
				Serial.println("event_occured 4 1 1 \"button_pressed\"");
				has_reported = 1;
			}
		}
	}
	else
	{ /* button is not pressed */
		button_is_pressed = 0;
		button_timer = button_timer_now;
	}
}

int set_color(int red, int green, int blue)
{
	analogWrite(red_pin, red);
	analogWrite(green_pin, green);
	analogWrite(blue_pin, blue);

	return 0;
}
void set_rgb(long rgb)
{
	int r = (rgb >> 16) & 0xFF;
	int g = (rgb >> 8) & 0xFF;
	int b = rgb & 0xFF;

	set_color(r, g, b);
	rgb_value = rgb;
}

void dht_check()
{
	humValue = dht.readHumidity();
	tmpValue = dht.readTemperature();
}

void serialEvent()
{
	if(!miio_string_received) {	/* if the string before has been handled */
		while (Serial.available())	/* receive command string from wifi module */
		{
			if (Serial.peek() != '\r')
			{
				miio_string += (char)Serial.read();
			}
			else
			{
				miio_string += (char)Serial.read();
				miio_string_received = true;
			}
		}
	}
}

/* process miio command string */
int string_handle()
{
	int ret = 0;
	unsigned char *str_tmp = (unsigned char *)calloc(1, strlen(miio_string.length()));
	/* 此处需要修改Arduino源文件String.h中buffer数组权限为public */
	memcpy(str_tmp, miio_string.buffer, strlen(miio_string.buffer));

	/* remove '\r' at the end of command string*/
	str_tmp[miio_string.length() - 1] = '\0';
	unsigned char *ptr = strtok(str_tmp, " ");
	/* pass "down" */
	ptr = strtok(NULL, " ");

	if (0 == strncmp(ptr, "get_properties", strlen("get_properties")))
	{ /* if get_properties */
		ptr = strtok(NULL, " ");
		Serial.print("result ");

		while (ptr != NULL)
		{
			int siid = atoi(ptr);
			ptr = strtok(NULL, " ");
			int piid = atoi(ptr);

			switch (siid)
			{
			case 2:
			{
				if (piid == 1)
				{
					if (0 != rgb_value)
					{
						Serial.print("2 1 0 true ");
					}
					else
					{
						Serial.print("2 1 0 false ");
					}
				}
				if (piid == 4)
				{
					Serial.print("2 4 0 ");
					Serial.print(rgb_value, 1);
					Serial.print(" ");
				}
			}
			break;

			case 3:
			{
				if (piid == 1)
				{
					Serial.print("3 1 0 ");
					Serial.print(humValue, 1);
					Serial.print(" ");
				}
				if (piid == 2)
				{
					Serial.print("3 2 0 ");
					Serial.print(tmpValue, 1);
					Serial.print(" ");
				}
			}
			break;
			}

			ptr = strtok(NULL, " ");
		}

		Serial.println("\r");
	}
	else if (0 == strncmp(ptr, "set_properties", strlen("set_properties")))
	{ /* if set_properties */
		ptr = strtok(NULL, " ");
		Serial.print("result ");

		while (ptr != NULL)
		{
			int siid = atoi(ptr);
			ptr = strtok(NULL, " ");
			int piid = atoi(ptr);

			switch (siid)
			{
			case 2:
			{
				if (piid == 1)
				{
					ptr = strtok(NULL, " ");
					if (0 == strncmp(ptr, "true", strlen("true")))
					{
						if (rgb_value == 0)
						{
							set_rgb(0xFFFFFF);
						}
					}
					else if (0 == strncmp(ptr, "false", strlen("false")))
					{
						if (rgb_value != 0)
						{
							set_rgb(0);
						}
					}

					Serial.print("2 1 0 ");
				}
				else if (piid == 4)
				{
					ptr = strtok(NULL, " ");
					set_rgb(atol(ptr));
					Serial.print("2 4 0 ");
				}
			}
			}
			break;

			ptr = strtok(NULL, " ");
		}

		Serial.println("\r");
	}
	else if (0 == strncmp(ptr, "error", strlen("error")))
	{
	}
	else if (0 == strncmp(ptr, "ok", strlen("ok")))
	{
	}
	else
	{
	}

	miio_string = "";
	miio_string_received = false;

	free(str_tmp);

	return ret;
}
