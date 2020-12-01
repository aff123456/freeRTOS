#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

SemaphoreHandle_t mutex;

QueueHandle_t integerQueue;

// variaveis globais
const TickType_t xDelay100 = 100 / portTICK_PERIOD_MS; // delay de 100ms
const TickType_t xDelay250 = 250 / portTICK_PERIOD_MS; // delay de 250ms
const TickType_t xDelay500 = 500 / portTICK_PERIOD_MS; // delay de 500ms

int randNumber = 0;

int valorRecebido = 0;
int valorFinal = 0;

const UBaseType_t queueLen = 50;  // tamanho da fila

void setup() {
  Serial.begin(115200);
  while(!Serial) {;}
  Serial.println("Serial inicializado");

  randomSeed(analogRead(0));

  mutex = xSemaphoreCreateMutex();
  if (mutex != NULL) {
    Serial.println("Mutex criado");
  }

  integerQueue = xQueueCreate(queueLen, // Queue length
                              sizeof(int) // Queue item size
  );
  
  if(integerQueue != NULL) {    
    /*
    xTaskCreate(TaskSerial, // Task function
                "Serial", // A name just for humans
                128,  // This stack size can be checked & adjusted by reading the Stack Highwater
                NULL, 
                2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                NULL);
    */
    Serial.println("Fila criada");
    xTaskCreate(TaskRandomNumber, // Task function
                "RandomNumber", // Task name
                128,  // Stack size
                NULL, 
                2, // Priority
                NULL);
  } else {
    Serial.println("Erro na criação da fila!");
  }  
  xTaskCreate(TaskReadQueue,
              "L1",
              128,
              NULL,
              1,
              NULL);
  
  xTaskCreate(TaskReadQueue,
              "L2",
              128,
              NULL,
              1,
              NULL);
  
  xTaskCreate(TaskBlink, // Task function
              "Blink", // Task name
              128, // Stack size 
              NULL, 
              1, // Priority
              NULL);
}

void loop() {}

void TaskRandomNumber (void *pvParameters) {
  (void) pvParameters;

  while(true) {
    //randNumber = analogRead(A0);
    randNumber = random(100);
    xQueueSend(integerQueue, &randNumber, portMAX_DELAY);
    //Serial.println(randNumber);
    //vTaskDelay(1);
  }
}

void TaskReadQueue (void *pvParameters) {
  (void) pvParameters;

  while(true) {
    if(xSemaphoreTake(mutex, 10) == pdTRUE) {
      if(xQueueReceive(integerQueue, &valorRecebido, portMAX_DELAY) == pdPASS) {
        Serial.print(pcTaskGetName(NULL)); // Get task name
        Serial.print(", numero : ");
        Serial.println(valorRecebido);
        if(checkPrime(valorRecebido)) {
          Serial.println("PRIMO!");
        }
        xSemaphoreGive(mutex);
      }
    }
    //vTaskDelay(1);
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

bool checkPrime(int number) {
  int numberDiv = 0;
  for(int i = 1; i <= number; i++) {
    if(number % i == 0) {
      numberDiv += 1;
      }
    }
  if(numberDiv == 2) {
    return true;
  } else {
    return false;
  }
}
