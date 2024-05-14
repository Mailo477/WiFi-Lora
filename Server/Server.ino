#include <SPI.h>
#include <LoRa.h>

// Definiciones de pines y frecuencia
#define SS 18       // Pin de selección del esclavo para el módulo LoRa
#define RST 14      // Pin de reinicio para el módulo LoRa
#define DI0 26      // Pin de interrupción para el módulo LoRa
#define BAND 915E6  // Frecuencia de operación (915 MHz)

TaskHandle_t Task1;  // Manejador de la tarea para multitasking

void setup() {
  xTaskCreatePinnedToCore(
    Task1code,   // Función de la tarea
    "Task1",     // Nombre de la tarea
    10000,       // Tamaño de la pila en palabras
    NULL,        // Parámetro de entrada de la tarea
    1,           // Prioridad de la tarea
    &Task1,      // Manejador de la tarea
    0);          // Núcleo donde se ejecutará la tarea (0)
}

void loop() {}

// Función de tarea para enviar datos
void Task1code(void * taskParams) {
  Serial.begin(115200);      // Inicia comunicación serial
  LoRa.setPins(SS, RST, DI0);  // Configura los pines para el módulo LoRa
  if (!LoRa.begin(BAND)) {     // Inicia el módulo LoRa en la frecuencia definida
    Serial.println("Inicio LoRa fallo");
    while (1);
  }

  while (1) {
    int temp = random(10, 41);        
    int hum = random(22, 81);    
    bool luz = random(0, 2);        

    // Mensaje a enviar
    String mensaje = String(temp) + "," + String(hum) + "," + String(luz);
    
    // Envía el mensaje a través de LoRa
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    Serial.println("Sent: " + mensaje);  // Imprime el mensaje enviado por el monitor
    delay(5000);  // 5 segundos antes del siguiente mensaje
  }
}
