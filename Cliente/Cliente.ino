#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Definiciones de pines y frecuencia
#define SS 18       // Pin de selección del esclavo para el módulo LoRa
#define RST 14      // Pin de reinicio para el módulo LoRa
#define DI0 26      // Pin de interrupción para el módulo LoRa
#define BAND 915E6  // Frecuencia de operación (915 MHz)

TaskHandle_t Task1;  // Recibir datos
TaskHandle_t Task2;  // Conexión WiFi

String ssid;        // SSID de la red WiFi
String password;    // Contraseña de la red WiFi

void setup() {
  Serial.begin(115200);
  delay(1000); 

  // Solicita el SSID y la contraseña 
  Serial.println("Ingrese SSID:");
  while (Serial.available() == 0) {}
  ssid = Serial.readString();

  Serial.println("Ingrese Password:");
  while (Serial.available() == 0) {}
  password = Serial.readString();

  // Creación de la tarea para recibir datos usando el núcleo 1
  xTaskCreatePinnedToCore(
    Task1code,   // Función de la tarea
    "Task1",     // Nombre de la tarea
    10000,       // Tamaño de la pila en palabras
    NULL,        // Parámetro de entrada de la tarea
    1,           // Prioridad de la tarea
    &Task1,      // Manejador de la tarea
    1);          // Núcleo donde se ejecutará la tarea (1)

  // Creación de la tarea para la conexión WiFi usando el núcleo 0
  xTaskCreatePinnedToCore(
    Task2code,   // Función de la tarea
    "Task2",     // Nombre de la tarea
    10000,       // Tamaño de la pila en palabras
    NULL,        // Parámetro de entrada de la tarea
    1,           // Prioridad de la tarea
    &Task2,      // Manejador de la tarea
    0);          // Núcleo donde se ejecutará la tarea (0)
}

// Función loop vacía ya que no se usa en este esquema
void loop() {}

// Recibir datos
void Task1code(void * taskParams) {
  LoRa.setPins(SS, RST, DI0);  // Configura los pines 
  if (!LoRa.begin(BAND)) {     // Inicia LoRa 
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  while (1) {
    int packetSize = LoRa.parsePacket();  
    if (packetSize) {
      String received = "";
      while (LoRa.available()) {
        received += (char)LoRa.read();    // Lee el paquete recibido
      }
      Serial.println("Recibido: " + received);  // Imprime el mensaje recibido por serial
      sendToServer(received);  // Envía el mensaje al servidor
    }
    delay(10);  // Pequeña espera para no sobrecargar el procesador
  }
}

// Función de la tarea para conectar a WiFi
void Task2code(void * taskParams) {
  while (1) {
    connectToWiFi();  // Conecta a la red WiFi
    delay(10000);     // Verifica la conexión cada 10 segundos
  }
}

// Función para conectar a la red WiFi
void connectToWiFi() {
  Serial.println("Conectando...");
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");
}


