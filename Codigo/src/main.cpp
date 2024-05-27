#include <Arduino.h>
#include "stdint.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "max6675.h"

typedef struct{
	TaskHandle_t main_Task_Handle;
	QueueHandle_t main_Task_CMD_Queue;
	TaskHandle_t calling_task;
}RTOS_API;


// Definimos los pines para los dos sensores MAX6675
const int CS_PIN1 = 5;   // Chip Select para el primer sensor
const int CS_PIN2 = 4;   // Chip Select para el segundo sensor
const int SCK_PIN = 18;  // Serial Clock (compartido)
const int SO_PIN = 19;   // Serial Output (compartido)

// Creación de las instancias del sensor
MAX6675 thermocouple1(SCK_PIN, CS_PIN1, SO_PIN);
MAX6675 thermocouple2(SCK_PIN, CS_PIN2, SO_PIN);

// Variables globales para almacenar las temperaturas
float temperature1 = 0.0;
float temperature2 = 0.0;

// Declaración de las tareas
void TaskReadSensors(void *pvParameters);
void TaskPrintData(void *pvParameters);

typedef struct {
  float temperature1;
  float temperature2;
} SensorData;

void setup() {
  // Inicializamos la comunicación serie
  Serial.begin(115200);
  while (!Serial) {
    ; // Esperamos a que se inicie el puerto serie
  }

  // Esperamos a que los sensores se estabilicen
  delay(500);

  // Creación de las tareas
  xTaskCreate(
    TaskReadSensors,     // Función de la tarea
    "ReadSensors Task",  // Nombre de la tarea (para depuración)
    2048,                // Tamaño de la pila (en palabras)
    NULL,                // Parámetros que se pasan a la tarea
    1,                   // Prioridad de la tarea
    NULL                 // Handle de la tarea (no se utiliza aquí)
  );

  xTaskCreate(
    TaskPrintData,       // Función de la tarea
    "PrintData Task",    // Nombre de la tarea (para depuración)
    2048,                // Tamaño de la pila (en palabras)
    NULL,                // Parámetros que se pasan a la tarea
    1,                   // Prioridad de la tarea
    NULL                 // Handle de la tarea (no se utiliza aquí)
  );
  xTaskCreate(
    TaskPrintData,       // Función de la tarea
    "PrintData Task",    // Nombre de la tarea (para depuración)
    2048,                // Tamaño de la pila (en palabras)
    NULL,                // Parámetros que se pasan a la tarea
    1,                   // Prioridad de la tarea
    NULL                 // Handle de la tarea (no se utiliza aquí)
  );
}

void loop() {
  // El loop se deja vacío ya que FreeRTOS maneja las tareas
  
}

// Implementación de la tarea TaskReadSensors
void TaskReadSensors(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    // Leer temperatura de los sensores
    temperature1 = thermocouple1.readCelsius();
    temperature2 = thermocouple2.readCelsius();

    // Simular un pequeño retardo para la lectura de sensores
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Esperamos 1 segundo
  }
}

// Implementación de la tarea TaskPrintData
void TaskPrintData(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    // Imprimir las temperaturas en el monitor serie
    Serial.print("Temperature 1: ");
    Serial.print(temperature1);
    Serial.println(" °C");

    Serial.print("Temperature 2: ");
    Serial.print(temperature2);
    Serial.println(" °C");

    // Simular un pequeño retardo para la impresión de datos
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Esperamos 2 segundos
  }
}