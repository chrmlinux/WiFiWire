#include "WiFiWire.h"
#include "AsyncUDP.h"

WiFiWire WWire(WIFIWIRE_CLIENT);

#define SENS_ADRS (0x49)
#define TEMP_REGS (   0)
#define RECV_BYTE (   2)

float tempRead(void)
{
  float rtn = 0.0;
  
  WWire.beginTransmission(SENS_ADRS);
  WWire.write(TEMP_REGS);
  WWire.endTransmission(false);
  WWire.requestFrom(SENS_ADRS, RECV_BYTE);
  while(1) {
    if (WWire.available() >= RECV_BYTE) break;
  }

  uint16_t dt = 0x0000;
  dt  = WWire.read() << 8;
  dt |= WWire.read();

  Serial.printf("dt = 0x%04x\n", dt);
  rtn = (float)(dt >> 3) / 16.0;
  return rtn;
}

void setup(void)
{
  Serial.begin( 115200 );
  while (!Serial);
  WWire.begin();

  Serial.printf("%.2f'C\n", tempRead());
}

void loop(void)
{
  WWire.update();
}
