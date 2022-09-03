#include "DS18b20.h"
#include "TDS.h"

TDS myTDS(27);

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  // pinMode(TdsSensorPin,INPUT);

  while (1)
  {
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    printf("Temperature: %.1f ºC, TDS: %.f ppm\n", temperature, myTDS.read(temperature));
    delay(500);
  }
}

void loop() {}