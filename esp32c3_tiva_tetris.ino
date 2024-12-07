//ESP32 C3 CON GOOGLE SHEETS Y TIVA

#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFi.h>

HardwareSerial SerialUART(1);  // Definimos un objeto Serial para el puerto UART1

#define RX_PIN 3   // Pin RX del ESP32-C3 (ajustar según necesidad)
#define TX_PIN 1   // Pin TX del ESP32-C3 (ajustar según necesidad)

// Replace with your SSID and Password
const char* ssid = "WLAN_5B27";
const char* password = "N0007268E24B0";
const char* serverName = "https://script.google.com/macros/s/AKfycbz11LNykJRsKWs5a1FWpaxFI9xCLcN3rLxqK1_tOoMKzmmdVxW5afFHqWw4FYHDp1KX/exec";

// Procesar los datos para extraer USER y SCORE
String user = "";
String score = "";

void setup() {
    SerialUART.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Configuración UART1 con 9600 baudios
    initWifi();
}

void loop() {
  if (SerialUART.available()) {                  // Espera a que haya datos desde la Tiva
    String receivedData = "";                  // Inicializa una cadena para recibir datos
    while (SerialUART.available()) {           // Mientras haya datos en el buffer UART
        char c = SerialUART.read();            // Lee el siguiente carácter
        receivedData += c;                     // Almacena el carácter en la cadena
        delay(10);                             // Pausa breve para asegurar la recepción completa
    }

    // Encontrar y extraer el valor de USER
    int userIndex = receivedData.indexOf("USER:");
    if (userIndex != -1) {
        int endIndex = receivedData.indexOf(";", userIndex);  // Encontrar el fin del valor
        user = receivedData.substring(userIndex + 5, endIndex); // Extraer el valor de USER
    }

    // Encontrar y extraer el valor de SCORE
    int scoreIndex = receivedData.indexOf("SCORE:");
    if (scoreIndex != -1) {
        int endIndex = receivedData.indexOf(";", scoreIndex);  // Encontrar el fin del valor
        score = receivedData.substring(scoreIndex + 6, endIndex); // Extraer el valor de SCORE
    }
    //sendToGoogleSheet();
    if (user.length() > 0 && score.length() > 0) {
        sendToGoogleSheet();
    }
    delay(200);
  }
}

void initWifi() {

  WiFi.begin(ssid, password);

  int timeout = 10 * 4;  // 10 seconds
  while (WiFi.status() != WL_CONNECTED && (timeout-- > 0)) {
    delay(250);
  }

}

void sendToGoogleSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"user\":\"" + String(user) + "\", \"score\":\"" + String(score) + "\"}";

    int httpResponseCode = http.POST(jsonData);
    http.end();
  }
}
