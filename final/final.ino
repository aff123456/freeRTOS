#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

// declarando o mutex
SemaphoreHandle_t mutex;

// handlers das filas, usados para escrever e ler dados das mesmas
QueueHandle_t integerQueue1;
QueueHandle_t integerQueue2;

// handlers das tarefas, usados para medir tamanho da stack das tarefas
TaskHandle_t Q1_handler;
TaskHandle_t L1_handler;
TaskHandle_t L2_handler;
TaskHandle_t LF_handler;

// variáveis globais 
//const TickType_t xDelay500 = 500 / portTICK_PERIOD_MS; // delay de 500ms

int randNumber = 0;     // número aleatório que vai ser gerado e colocado na fila 1
int maxNumber = 1000;   // teto do número aleatório

int valorRecebido = 0;  // variável que guarda o valor lido da fila 1
int valorFinal = 0;     // variável que guarda o valor lido da fila 2

const UBaseType_t queueLen = 10 + maxNumber/50; // tamanho das filas, dinâmico pois quanto maior o teto
                                                // maior o tempo para checar se é primo, e a fila acumula

void setup() {
  Serial.begin(115200);             // inicia comunicação serial

  //Serial.println(queueLen);       // print do tamanho da fila
  randomSeed(analogRead(A0));       // usa um valor aleatório lido da porta analógica para iniciar o gerador de números aleatórios

  mutex = xSemaphoreCreateMutex();  // cria o mutex
  if (mutex != NULL) {
    Serial.println("Mutex criado");
  }
  
  integerQueue1 = xQueueCreate(queueLen,  // tamanho da fila
                              sizeof(int) // tamanho dos itens da fila
  );
  integerQueue2 = xQueueCreate(queueLen,  // tamanho da fila
                              sizeof(int) // tamanho dos itens da fila
  );
  
  if (integerQueue1 != NULL) {    
    Serial.println("Fila 1 criada");
    xTaskCreate(TaskRandomNumber, // função da tarefa
                "RandomNumber",   // nome da tarefa
                80,               // tamanho da stack (regulado usando uxHighWaterMark)
                NULL,             // parâmetros da tarefa
                2,                // prioridade
                &Q1_handler);     // handler
  } else {
    Serial.println("Erro na criação da fila 1!");
  }

  if (integerQueue2 != NULL) {
    Serial.println("Fila 2 criada");
    xTaskCreate(TaskReadQueueFinal, // função da tarefa
                "LeitorFinal",      // nome da tarefa
                88,                 // tamanho da stack
                NULL,               // parâmetros da tarefa
                3,                  // prioridade
                &LF_handler);       // handler
  } else {
    Serial.println("Erro na criação da fila 2!");
  }
  
  xTaskCreate(TaskReadQueue,  // função da tarefa
              "L1",           // nome da tarefa
              128,            // tamanho da stack
              NULL,           // parâmetros da tarefa
              1,              // prioridade
              &L1_handler);   // handler

  xTaskCreate(TaskReadQueue,  // função da tarefa
              "L2",           // nome da tarefa
              128,            // tamanho da stack
              NULL,           // parâmetros da tarefa
              1,              // prioridade
              &L2_handler);   // handler
  Serial.println(); // print de uma linha em branco antes de começar a execução das tarefas
}

void loop() {}

void TaskRandomNumber (void *pvParameters) {
  (void) pvParameters;

  // mede o tamanho da stack da tarefa
  //UBaseType_t uxHighWaterMark;
  //uxHighWaterMark = uxTaskGetStackHighWaterMark(Q1_handler);
  //Serial.println(uxHighWaterMark);

  // vê quantos espaços tem sobrando na fila
  //UBaseType_t aval = uxQueueSpacesAvailable(integerQueue1);
  //Serial.println(aval);
  
  while(true) {
    //randNumber = analogRead(A0);    // número aleatório vem do barulho na leitura da porta analógica
    randNumber = random(maxNumber);   // número aleatório através da função random, entre 0 e maxNumber
    xQueueSend(integerQueue1, &randNumber, portMAX_DELAY);  // tenta colocar o número na fila, se a fila
                                                            // estiver cheia então espera. caso o tempo de
                                                            // espera ultrapasse portMAX_DELAY então desiste
    vTaskDelay(1);  // espera 1 tick para rodar novamente

    // mede o tamanho da stack da tarefa
    //uxHighWaterMark = uxTaskGetStackHighWaterMark(Q1_handler);
    //Serial.println(uxHighWaterMark);

    // vê quantos espaços tem sobrando na fila
    //aval = uxQueueSpacesAvailable(integerQueue1);
    //Serial.println(aval);
  }
}

void TaskReadQueue (void *pvParameters) {
  (void) pvParameters;

  // mede o tamanho das stacks das tarefas
  //UBaseType_t uxHighWaterMark;
  //uxHighWaterMark = uxTaskGetStackHighWaterMark(L1_handler);
  //Serial.println(uxHighWaterMark);
  //uxHighWaterMark = uxTaskGetStackHighWaterMark(L2_handler);
  //Serial.println(uxHighWaterMark);
  
  while(true) {
    if(xSemaphoreTake(mutex, 10) == pdTRUE) { // pdTRUE se conseguir pegar o mutex
      if(xQueueReceive(integerQueue1, &valorRecebido, portMAX_DELAY) == pdPASS) { // pdPASS/pdTRUE se recebeu um item da fila
        if(checkPrime(valorRecebido)) { // se o valor recebido for primo coloca na fila 2
          xQueueSend(integerQueue2, &valorRecebido, portMAX_DELAY); // idem procedimento anterior
        }
      }
    }
    xSemaphoreGive(mutex);  // libera o semáforo
    vTaskDelay(1);          // espera 1 tick para rodar novamente
    
    // mede o tamanho das stacks das tarefas
    //uxHighWaterMark = uxTaskGetStackHighWaterMark(L2_handler);
    //Serial.println(uxHighWaterMark);
  }
}

void TaskReadQueueFinal (void *pvParameters) {
  (void) pvParameters;

  // mede o tamanho da stack da tarefa
  //UBaseType_t uxHighWaterMark;
  //uxHighWaterMark = uxTaskGetStackHighWaterMark(LF_handler);
  //Serial.println(uxHighWaterMark);  
  
  while(true) {
    if(xQueueReceive(integerQueue2, &valorFinal, portMAX_DELAY) == pdPASS) { // idem procedimento anterior
      Serial.print("Fila 2: ");     // print do valor recebido da lista 2
      Serial.println(valorFinal);
      
    }
    // mede o tamanho da stack da tarefa
    //uxHighWaterMark = uxTaskGetStackHighWaterMark(LF_handler);
    //Serial.println(uxHighWaterMark);
  }
}

bool checkPrime(int number) {
  int numberDiv = 0;                  // variável que vai receber o número de divisões do número
  for(int i = 1; i <= number; i++) {  // laço for pra checar por quais números pode ser dividido
    if(number % i == 0) {             // se o módulo é 0 então pode ser dividido por i
      numberDiv += 1;                 // aumenta o número de divisões
      }
    if(numberDiv > 2) {               // se pode ser dividido por mais de 2 números pode sair do laço, não é primo
      break;
    }
  }
  if(numberDiv == 2) {                // se é divisível por apenas 2 números então é primo
    return true;
  } else {                            // caso contrário não é
    return false;
  }
}
