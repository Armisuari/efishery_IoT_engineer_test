#include <Arduino.h>
#include "DS3231.h"
#include "DS18b20.h"
#include "TDS.h"
#include "SD_log.h"

#define interval 1000
float temperature;
unsigned long previousMillis;

TDS myTDS(27);
String payload;

void read_sensors(void *parameter);

void setup()
{
  Serial.begin(115200);
  setup_time();
  setup_sd();
  sensors.begin();
  // pinMode(TdsSensorPin,INPUT);

  xTaskCreate(
      read_sensors,
      "read Sensor",
      5000,
      NULL,
      1,
      NULL);

  while (1)
  {
    DateTime now = rtc.now();
    if (millis() - previousMillis >= 5000)
    {
      previousMillis = millis();
      payload = String(daysOfTheWeek[now.dayOfTheWeek()]) + "@" + String(now.day()) + "@" + String(now.month()) + "@" + String(now.year()) +
                "#" + String(temperature, 1) + "#" + String(myTDS.read(temperature), 0) + "\r\n";

      Serial.println(payload);
      logSDCard(payload);
    }
  }
}

void loop() {}

void read_sensors(void *parameter)
{

  for (;;)
  {
    DateTime now = rtc.now();
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);

    printf("Date Time: %s,%d-%d-%d >> ", daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year());
    printf("Temperature: %.1f ºC, TDS: %.f ppm\n", temperature, myTDS.read(temperature));
    vTaskDelay(interval / portTICK_PERIOD_MS);
  }
}