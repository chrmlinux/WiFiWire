#include "WiFiWire.h"
#include "AsyncUDP.h"

WiFiWire WWire(WIFIWIRE_SERVER);

void setup()
{
  Serial.begin( 115200 );
  while (!Serial);
  WWire.begin();
}

void loop()
{
  WWire.update();
}
