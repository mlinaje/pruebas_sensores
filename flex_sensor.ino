#define DEBUG

#define FLEX_GPIO A0
#define BUZZER_GPIO 2
#define WINDOW_SIZE 20
#define FLEX_ALERT_THRESHOLD 20

int flexposition, min_flexposition, max_flexposition, avg_flexposition;    // Input value from the analog pin.
int window_flexposition[WINDOW_SIZE];

void read_and_update_flexvalues();
int avg_vector_calc(int *vector, size_t count);
void training_phase();
void detection_phase();

void setup(){ 
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  
  pinMode(FLEX_GPIO, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_GPIO, OUTPUT);

  min_flexposition = analogRead(FLEX_GPIO);
  max_flexposition = min_flexposition;

  #ifdef DEBUG
    Serial.println("Beginning training phase...");
  #endif
  training_phase();  

  #ifdef DEBUG
    Serial.println("Beginning detection phase...");
  #endif
} 

void loop(){ 
  detection_phase();
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
} 


void read_and_update_flexvalues(){
  // Read the position of the flex sensor (0 to 1023):
  flexposition = analogRead(FLEX_GPIO);

  if (flexposition<min_flexposition)
    min_flexposition=flexposition;

  if (flexposition>max_flexposition)
    max_flexposition=flexposition;

  #ifdef DEBUG
    Serial.print("sensor: ");
    Serial.print(flexposition);
    Serial.print(" min: ");
    Serial.print(min_flexposition);
    Serial.print(" max: ");
    Serial.println(max_flexposition);
  #endif
}

int avg_vector_calc(int *vector, size_t lenght){
  int total=0;
  for (int i=0; i<lenght; i++)
    total+=vector[i];
  return (total/lenght);
}

void training_phase(){

  //Getting for avg while not flexed
  for (int i=0; i<20; i++)
    window_flexposition[i]=analogRead(FLEX_GPIO);

  avg_flexposition=avg_vector_calc(window_flexposition, WINDOW_SIZE);    
  #ifdef DEBUG
    Serial.print("I got not mapped average flex of ");
    Serial.println(avg_flexposition);
  #endif
  
  //Wating to get enough min max variability
  while (max_flexposition-min_flexposition<100){ 
    read_and_update_flexvalues();
  }
  #ifdef DEBUG
    Serial.print("I got min, max: ");
    Serial.print(min_flexposition, ", ");
    Serial.println(max_flexposition);
  #endif

  avg_flexposition=map(avg_vector_calc(window_flexposition, WINDOW_SIZE), min_flexposition, max_flexposition, 0, 100);
  #ifdef DEBUG
    Serial.print(" mapped non-flex sensor value: ");
    Serial.println(avg_flexposition);
  #endif  

}

void detection_phase(){
  flexposition = map(analogRead(FLEX_GPIO), min_flexposition, max_flexposition, 0, 100);
  #ifdef DEBUG
    Serial.print(" mapped read flex sensor value: ");
    Serial.println(flexposition);
  #endif  

  if (abs(flexposition-avg_flexposition)>FLEX_ALERT_THRESHOLD){
    #ifdef DEBUG
     Serial.println("cuidado con la postura!!!");
    #endif
    digitalWrite(LED_BUILTIN,HIGH);  
    digitalWrite(BUZZER_GPIO,HIGH); 
    delay(100); 
    digitalWrite(BUZZER_GPIO,LOW);      
  }
}


