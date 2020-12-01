#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

SemaphoreHandle_t mutex;

QueueHandle_t integerQueue;

// variaveis globais
const TickType_t xDelay250 = 250 / portTICK_PERIOD_MS; // delay de 250ms
const TickType_t xDelay500 = 500 / portTICK_PERIOD_MS; // delay de 500ms

int randNumber = 0;

const UBaseType_t queueLen = 20;  // tamanho da fila

void setup() {
  Serial.begin(115200);

  randomSeed(analogRead(0));
  
  while(!Serial) {;}

  integerQueue = xQueueCreate(queueLen, // Queue length
                              sizeof(int) // Queue item size
  );
  
  if (integerQueue != NULL) {    
    /*
    xTaskCreate(TaskSerial, // Task function
                "Serial", // A name just for humans
                128,  // This stack size can be checked & adjusted by reading the Stack Highwater
                NULL, 
                2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                NULL);
    */
                
    xTaskCreate(TaskAnalogRead, // Task function
                "AnalogRead", // Task name
                128,  // Stack size
                NULL, 
                1, // Priority
                NULL);
    
  }
  
  xTaskCreate(TaskBlink, // Task function
              "Blink", // Task name
              128, // Stack size 
              NULL, 
              0, // Priority
              NULL);
  
  xTaskCreate(TaskReadQueue,
              "LeitorFila",
              128,
              NULL,
              1,
              NULL);
  
}

void loop() {}

void TaskAnalogRead (void *pvParameters) {
  (void) pvParameters;

  while(true) {
    //randNumber = analogRead(A0);
    randNumber = random(100);
    xQueueSend(integerQueue, &randNumber, portMAX_DELAY);
  }
}

void TaskBlink (void *pvParameters) {
  (void) pvParameters;

  pinMode(LED_BUILTIN, OUTPUT);
  
  while(true) {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(xDelay500);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(xDelay500);
  }
}

void TaskReadQueue (void *pvParameters) {
  (void) pvParameters;

  while(true) {
    
  }
}
