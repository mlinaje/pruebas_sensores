#include <ESP8266WiFi.h>
#include "time.h"
#include "d:/workspace/nodemcu/wifi_ssid_pass.h"
/*file containing:
  const char* ssid="XXX";
  const char* pass="YY";*/

#define TZ 3600  //1h timezone in sec
#define DST 0 //1h daylightOffset in sec

time_t current_time, last_time;

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //La hora del sistema se actualiza automaticamente por defecto cada hora
  configTime(TZ, DST, "es.pool.ntp.org", "pool.ntp.org", "time.nist.gov");

  Serial.println();
  // esperar hasta sincronizar el reloj, aprovechando que time() devuelve 0 si no se ha inicializado
  while (!current_time) {
    time(&current_time);
    Serial.print("-");
    delay(100);
  }
  Serial.println();

  //Se puede descomentar esta línea para comprobar que incluso sin conexión, el reloj interno se mantiene
  //También se puede probar a desconectar el router/AP
  //WiFi.disconnect();
}

void loop() {
  time(&current_time);

  if ((current_time != last_time) && ((current_time % 60) == 0)) {
    Serial.print(ctime(&current_time));
    last_time = current_time;

    //Otro ejemplo para coger la hora
    /*int minutes = (current_time / 60) % 60;
    int hours = (current_time / 3600) % 12;
    Serial.print(hours);
    Serial.print(":");
    Serial.println(minutes);*/

  }
  delay(1000); //esperamos 1 segundo hasta volver a comprobar la hora

}

