#ifndef _WIFIWIRE_H_
#define _WIFIWIRE_H_

#include "arduino.h"
#include "WiFi.h"
#include "AsyncUDP.h"

#define RECV_BUF_MAX (16)
#define SEND_BUF_MAX (16)

#include <Wire.h>
#define MYSDA (25)
#define MYSCL (21)

enum {NOP = 0, SOH, STX, ETX};
enum {WIFIWIRE_SERVER = 0, WIFIWIRE_CLIENT};

#define UDP_SEND_DELAY (1)

class WiFiWire {

  public:
  
    WiFiWire(int);
    int16_t begin(void);
    int16_t beginTransmission(int);
    int16_t requestFrom(int, int);
    int16_t endTransmission(bool);
    int16_t available(void);
    int16_t read(void);
    int16_t write(uint8_t);
    int16_t update(void);
    
  private:

    AsyncUDP _udp;
    const char *_ssid = "WifiWire";
    const char *_pass = "#WifiWire#";
    int _serverKind = WIFIWIRE_SERVER;
		int16_t sendUDP(void);

};
#endif
