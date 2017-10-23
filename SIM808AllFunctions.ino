/**************************************************************
 *
 * Based on
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 * modified by Marino Linaje (22-Oct-2017)
 * primary to add some GPS functionality
 *
 **************************************************************/

// Select your modem:
//#define TINY_GSM_MODEM_SIM800
 #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX


//#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

// Set phone numbers, if you want to test SMS and Calls
//#define SMS_TARGET  "+34xxxxxxxxx"
//#define CALL_TARGET "+34xxxxxxxxx"

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "YourAPN";
const char user[] = "";
const char pass[] = "";

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate
  TinyGsmAutoBaud(SerialAT);
  delay(3000);
}

void loop() {

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.restart()) {
    delay(10000);
    return;
  }

  String modemInfo = modem.getModemInfo();
  DBG("Modem:", modemInfo);

  // Unlock your SIM card with a PIN
  modem.simUnlock("1314");

  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return;
  }

  bool res;

  String ccid = modem.getSimCCID();
  DBG("CCID:", ccid);

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);

  String cop = modem.getOperator();
  DBG("Operator:", cop);
  
  IPAddress local = modem.localIP();
  DBG("Local IP:", local);

  int csq = modem.getSignalQuality();
  DBG("Signal quality:", csq);

  // This is NOT supported on M590
  int battLevel = modem.getBattPercent();
  DBG("Battery level:", battLevel);

  // This is only supported on SIMxxx series
  float battVoltage = modem.getBattVoltage() / 1000.0F;
  DBG("Battery voltage:", battVoltage);

  // This is only supported on SIMxxx series
  String gsmLoc = modem.getGsmLocation();
  DBG("GSM location:", gsmLoc);

#if defined(TINY_GSM_MODEM_SIM808)
  float gps_lat, gps_lon, gps_speed=0;
  int gps_alt=0, gps_vsat=0, gps_usat=0;

  modem.enableGPS();

  while (!modem.getGPS(&gps_lat, &gps_lon, &gps_speed, &gps_alt, &gps_vsat, &gps_usat));
  DBG("GPS lat:", gps_lat);
  DBG("GPS lon:", gps_lon);
  DBG("GPS speed:", gps_speed);
  DBG("GPS alt:", gps_alt);
  DBG("GPS vsat:", gps_vsat);
  DBG("GPS usat:", gps_usat);
  
  
  String gps_raw = modem.getGPSraw();
  DBG("GPS raw data:", gps_raw);

  modem.disableGPS();
  DBG("GPS raw data:", gps_raw);
#endif

#if defined(SMS_TARGET)
  res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
  DBG("SMS:", res ? "OK" : "fail");

  // This is only supported on SIMxxx series
  res = modem.sendSMS_UTF16(SMS_TARGET, u"Привіііт!", 9);
  DBG("UTF16 SMS:", res ? "OK" : "fail");
#endif

#if defined(CALL_TARGET)
  DBG("Calling:", CALL_TARGET);

  // This is NOT supported on M590
  res = modem.callNumber(CALL_TARGET);
  DBG("Call:", res ? "OK" : "fail");

  if (res) {
    delay(5000L);
  
    res = modem.callHangup();
    DBG("Hang up:", res ? "OK" : "fail");
  }
#endif

  modem.gprsDisconnect();
  DBG("GPRS disconnected");

  // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
  modem.poweroff();
  DBG("Poweroff.");

  // Do nothing forevermore
  while (true) {
    modem.maintain();
  }
}

