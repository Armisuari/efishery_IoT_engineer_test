#include <Arduino.h>
#include "DS3231.h"
#include "DS18b20.h"
#include "TDS.h"
#include "SD_log.h"

int interval = 5000;
float temperature;
unsigned long prevMill, previousMillis;
String timestamp, inChar, inData;
bool config = true;

TDS myTDS(27);
String payload;

void read_sensors(void *parameter);
void input_handle();
void start_handle();

void setup()
{
  Serial.begin(115200);
  setup_time();
  setup_sd();
  sensors.begin();

  xTaskCreate(
      read_sensors,
      "read Sensor",
      5000,
      NULL,
      1,
      NULL);

  printf("Ready\n");
  start_handle();

  while (1)
  {
    DateTime now = rtc.now();

    input_handle();

    if (millis() - previousMillis >= interval)
    {
      previousMillis = millis();
      payload = String(daysOfTheWeek[now.dayOfTheWeek()]) + "@" + String(now.day()) + "@" + String(now.month()) + "@" + String(now.year()) +
                "@" + timestamp + "#" + String(temperature, 1) + "#" + String(myTDS.read(temperature), 0) + "\r\n";
      logSDCard(payload);
      printf("interval: %d\n", interval);
      printf("Ketik 'help' untuk beralih ke menu !\n\n");
    }
  }
}

void loop() {}

void read_sensors(void *parameter)
{

  for (;;)
  {
    DateTime now = rtc.now();
    timestamp = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);

    if (millis() - prevMill >= interval)
    {
      prevMill = millis();
      printf("Date Time: %s,%d-%d-%d | %s >> ", daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year(), timestamp);
      printf("Temperature: %.1f ºC, TDS: %.f ppm\n", temperature, myTDS.read(temperature));
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void start_handle()
{
  printf("\n\n=======================================\n");
  printf("ESP32 with TDS & DS18b20 Sensor Temp\n");
  printf("=======================================\n");
  printf("Pilih Menu !:\n\n1. Stream Data\n2. Set Interval\n3. Buka Data SDcard\n");
  printf("==================================\n");

  while (1)
  {
    while (Serial.available())
    {
      inChar = Serial.readString();
      Serial.println(inChar);
    }
    if (inChar.length() > 0)
    {
      if (inChar == "1\n")
      {
        printf("Stream Data dipilih\n");
        printf("==================================\n");
        break;
      }
      else if (inChar == "2\n")
      {
        printf("Set Interval dipilih\n");
        printf("Masukan Nilai:\n");
        printf("==================================\n");

        while (1)
        {
          String inVal;
          if (Serial.available())
          {
            inVal = Serial.readString();
            Serial.println(inVal);
            interval = inVal.toInt();

            config = false;
            inChar = "";
            break;
          }
        }
        break;
      }
      else if (inChar == "3\n")
      {
        printf("Buka Data SDcard Dipilih\n");
        printf("==================================\n\n");
        printf("Membuka file...\n");
        delay(3000);
        readFile(SD, "/data.txt");
        delay(2000);

        printf("\nStream Data start in 10 second...\n");
        delay(10000);
        break;
      }
    }
  }
}

void input_handle()
{

  while (Serial.available())
  {
    inChar = Serial.readString();
    Serial.println(inChar);
  }

  if (inChar == "help\n")
  {
    printf("==================================\n");
    printf("Pilih Menu !:\n\n1. Ubah Interval\n2. Buka Data SDcard\n");
    printf("==================================\n");
    while (1)
    {
      String inMenu;
      while (Serial.available())
      {
        inMenu = Serial.readString();
        Serial.println(inMenu);
      }
      if (inMenu.length() > 0)
      {
        if (inMenu == "1\n")
        {
          printf("Ubah Interval Dipilih\nMasukan Nilai:\n");
          printf("==================================\n");
          while (1)
          {
            String inVal;
            if (Serial.available())
            {
              inVal = Serial.readString();
              Serial.println(inVal);
              interval = inVal.toInt();

              config = false;
              inChar = "";
              inMenu = "";
              break;
            }
          }
        }
        else if (inMenu == "2\n")
        {
          printf("Buka Data SDcard Dipilih\n");
          printf("==================================\n\n");
          printf("Membuka file...\n");
          delay(3000);
          readFile(SD, "/data.txt");
          delay(2000);

          config = false;
          inChar = "";
          inMenu = "";
          break;
        }
        else
        {
          printf("Not Valid\n");
          break;
        }
      }
      if (config == false)
        break;
    }
  }
  config = true;
}