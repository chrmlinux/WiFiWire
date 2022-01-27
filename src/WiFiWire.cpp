#include <Arduino.h>
#include "WiFiWire.h"

uint8_t _send[SEND_BUF_MAX] = {0};
uint8_t _recv[RECV_BUF_MAX] = {0};
uint16_t _sendCnt = 0;
uint16_t _recvCnt = 0;
uint16_t _readPos = 0;

//===========================================
//
//===========================================
void setLocal(uint8_t *dt, uint16_t cnt)
{
  memcpy(_recv, dt, cnt);
  _recvCnt = cnt;
  _readPos = 0;
}

uint8_t getLocal(void)
{
  uint8_t rtn = _recv[_readPos];
  _readPos ++;
  return rtn;
}


//===========================================
//
//
//
//
//===========================================
WiFiWire::WiFiWire(int serverKind)
{
  _serverKind = serverKind;
}

//===========================================
// begin
//===========================================
int16_t WiFiWire::begin(void)
{
  int16_t rtn = 0;
  WiFi.disconnect(true, true);

  //===========================================
  // IPAddress set
  //===========================================
  IPAddress serverIp = IPAddress(192, 168, 4, 2);
  IPAddress     gwIp = IPAddress(192, 168, 4, 1);
  IPAddress   subnet = IPAddress(255, 255, 255, 0);
  uint16_t serverPort = 50002;

  if (_serverKind == WIFIWIRE_SERVER) {

    //===========================================
    // Setver
    //===========================================
    WiFi.softAPConfig(serverIp, gwIp, subnet);
    WiFi.softAP(_ssid, _pass);
    _udp.listen(serverPort);
    Wire.begin(MYSDA, MYSCL, 400);
    memset(_send, 0x0, sizeof(_send));
    _sendCnt = 0;
    Serial.print("UDP Listening on IP = "); Serial.print(WiFi.softAPIP());
    Serial.print(":"); Serial.println(serverPort);

  } else {

    //===========================================
    // Client
    //===========================================
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("WiFi Failed");
      while (1) {
        delay(1000);
      }
    }
    _udp.connect(serverIp, serverPort);
    Serial.print("UDP Connect IP = "); Serial.print(serverIp);
    Serial.print(":"); Serial.println(serverPort);
  }

  //===========================================
  // data on recv set
  //===========================================
  _udp.onPacket([](AsyncUDPPacket packet) {

    if (packet.length()) {

      //===========================================
      // on packet ?
      //===========================================
      uint8_t send[SEND_BUF_MAX] = {0};
      uint16_t sendCnt = 0;
      uint8_t *recv = NULL;
      uint8_t cmd = 0;

      recv = packet.data();
      cmd = recv[0];

      switch (cmd) {

        //===========================================
        // requestFrom
        //-------------------------------------------
        // Client -> local recv
        //===========================================
        case 'F':
          {
            int adrs = recv[1];
            int cnt =  recv[2];
            setLocal(&recv[3], cnt);
          }
          break;

        //===========================================
        // beginTransmission
        //-------------------------------------------
        // Server -> Wire
        //===========================================
        case 'b':
          Wire.beginTransmission(recv[1]);
          break;

        //===========================================
        // requestFrom
        //-------------------------------------------
        // Server -> Wire -> Client
        //===========================================
        case 'f':
          {
            int adrs = recv[1];
            int len = recv[2];
            int rcnt = 0;
            sendCnt = 0;

            //===========================================
            // Server -> Wire
            //===========================================
            Wire.requestFrom(adrs, len);
            send[sendCnt] =  'F'; sendCnt ++;
            send[sendCnt] = adrs; sendCnt ++;
            send[sendCnt] =  len; sendCnt ++;

            //===========================================
            // is Wire.available ?
            //===========================================
            while (1) {
              if (Wire.available() >= len) break;
              rcnt++;
            }

            //===========================================
            // Wire.read
            //===========================================
            for (int i = 0; i < len; i++) {
              send[sendCnt] = Wire.read();
              sendCnt ++;
            }

            //===========================================
            // Server -> Client
            //===========================================
            // 'F',adrs,cnt,high,low
            packet.write(send, sendCnt);
            sendCnt = 0;
          }
          break;

        //===========================================
        // endTransmission
        //-------------------------------------------
        // Server -> Wire
        //===========================================
        case 'e':
          Wire.endTransmission(recv[1] ? true : false);
          break;

        //===========================================
        // write
        //-------------------------------------------
        // Server -> Wire
        //===========================================
        case 'w':
          Wire.write(recv[1]);
          break;

      }
    }
  });

}

//===========================================
// update
//-------------------------------------------
// delay :)
//===========================================
int16_t WiFiWire::update(void)
{
  uint16_t rtn = 0;
  delay(1);
  return rtn;
}

//===========================================
// beginTransmission
//-------------------------------------------
// Client -> Server
//===========================================
int16_t WiFiWire::beginTransmission(int adrs)
{
  uint16_t rtn = 0;
  _sendCnt = 0;
  _send[_sendCnt] =  'b'; _sendCnt ++;
  _send[_sendCnt] = adrs; _sendCnt ++;
  sendUDP();
  return rtn;
}

//===========================================
// requestFrom
//-------------------------------------------
// Client -> Server
//===========================================
int16_t WiFiWire::requestFrom(int adrs, int cnt)
{
  uint16_t rtn = 0;
  _sendCnt = 0;
  _send[_sendCnt] =  'f'; _sendCnt ++;
  _send[_sendCnt] = adrs; _sendCnt ++;
  _send[_sendCnt] =  cnt; _sendCnt ++;
  sendUDP();
  return rtn;
}

//===========================================
// endTransmission
//-------------------------------------------
// Client -> Server
//===========================================
int16_t WiFiWire::endTransmission(bool flag)
{
  uint16_t rtn = 0;
  _sendCnt = 0;
  _send[_sendCnt] =  'e'; _sendCnt ++;
  _send[_sendCnt] = flag ? true : false; _sendCnt ++;
  sendUDP();
  return rtn;
}

//===========================================
// available
//-------------------------------------------
// Server -> Client
//===========================================
int16_t WiFiWire::available(void)
{
  uint16_t rtn = _recvCnt;
  return rtn;
}

//===========================================
// read
//-------------------------------------------
// local recv -> Client
//===========================================
int16_t WiFiWire::read(void)
{
  uint16_t rtn = getLocal();
  _recvCnt --;
  return rtn;
}

//===========================================
// write
//-------------------------------------------
// Client -> Server
//===========================================
int16_t WiFiWire::write(uint8_t dt)
{
  uint16_t rtn = 0;
  _sendCnt = 0;
  _send[_sendCnt] = 'w'; _sendCnt ++;
  sendUDP();
  return rtn;
}

//===========================================
// sendUDP
//-------------------------------------------
// Client -> Server
//===========================================
int16_t WiFiWire::sendUDP(void)
{
  uint16_t rtn = 0;
  _udp.write(_send, _sendCnt);
  _sendCnt = 0;
  delay(UDP_SEND_DELAY);
  return rtn;
}
