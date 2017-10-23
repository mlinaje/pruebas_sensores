#ifdef ESP8266
  #define Presencia_GPIO D2
#else
  #define Presencia_GPIO 4
#endif

#define LED_GPIO LED_BUILTIN

bool val = 0;
bool old_val = 0;

void setup() {
  Serial.begin(115200);
  pinMode (Presencia_GPIO, INPUT);
  pinMode (LED_GPIO, OUTPUT);
  old_val = digitalRead(Presencia_GPIO);
  Serial.flush();
  Serial.println("");
}

void loop() {
  val = digitalRead(Presencia_GPIO);

  if (old_val != val){
    if (val)
      Serial.println("DETECTADO CUERPO EN MOVIMIENTO");
    else
      Serial.println("TODO TRANQUILO");

#ifdef ESP8266
      digitalWrite(LED_GPIO, !val); //lógica inversa del ESP8266
#else
      digitalWrite(LED_GPIO, val);
#endif
    old_val = val;
  }
  //delay(1000);
  //yield();
}
