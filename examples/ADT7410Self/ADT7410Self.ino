//
// ADT7410.ino
// THX. by https://hatakekara.com/adt7410/
//

#include <Wire.h>
#define MYSDA (25)
#define MYSCL (21)
#define SENS_ADRS (0x49)
#define TEMP_REGS (   0)
#define RECV_BYTE (   2)

void setup()
{
  Serial.begin( 115200 );
  while (!Serial);
  Wire.begin(MYSDA, MYSCL, 400);

  Wire.beginTransmission(SENS_ADRS);
  Wire.write(TEMP_REGS);
  Wire.endTransmission(false);
  Wire.requestFrom(SENS_ADRS, RECV_BYTE);
  while(1) {
    if (Wire.available() >= RECV_BYTE) break;
  }

  uint16_t dt = 0x0000;
  float tmp = 0.0;
  dt  = Wire.read() << 8;
  dt |= Wire.read();

  Serial.printf("dt = 0x%04x\n", dt);
  Serial.printf("Temp = %.2f'C\n",(float)(dt >> 3)/16.0);
}

void loop()
{
}
