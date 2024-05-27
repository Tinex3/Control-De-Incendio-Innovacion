#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <max6675.h>

// Definición de pines para MAX6675
int thermoDO1 = 19; // Termopar 1
int thermoCS1 = 5;
int thermoCLK1 = 18;

int thermoDO2 = 19; // Termopar 2
int thermoCS2 = 17;
int thermoCLK2 = 18;

// Definición de pines para LM35 y sensor de calidad de aire
const int lm35Pin = 34;
const int airQualityPin = 35;

// Inicialización de objetos MAX6675
MAX6675 thermocouple1(thermoCLK1, thermoCS1, thermoDO1);
MAX6675 thermocouple2(thermoCLK2, thermoCS2, thermoDO2);

// Configuración WiFi
const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";

// Crear servidor web
AsyncWebServer server(80);

// HTML de la página web
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Monitor de Sensores</title>
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f8f9fa;
    }
    .container {
      max-width: 600px;
      margin: 50px auto;
      padding: 20px;
      background-color: #fff;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }
    .sensor-data {
      font-size: 1.5em;
      margin: 20px 0;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1 class="text-center">Monitor de Sensores</h1>
    <div class="sensor-data">
      <p>Termopar 1: <span id="tempC1">--</span> °C</p>
      <p>Termopar 2: <span id="tempC2">--</span> °C</p>
      <p>LM35: <span id="tempC3">--</span> °C</p>
      <p>Calidad de aire: <span id="airQuality">--</span></p>
    </div>
  </div>
  <script>
    setInterval(function () {
      fetch('/sensor-data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('tempC1').innerText = data.tempC1;
          document.getElementById('tempC2').innerText = data.tempC2;
          document.getElementById('tempC3').innerText = data.tempC3;
          document.getElementById('airQuality').innerText = data.airQuality;
        });
    }, 2000);
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando lectura de sensores...");
  delay(500); // Espera inicial para estabilización de sensores

  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  Serial.println(WiFi.localIP());

  // Ruta para servir la página web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Ruta para obtener los datos de los sensores
  server.on("/sensor-data", HTTP_GET, [](AsyncWebServerRequest *request){
    double tempC1 = thermocouple1.readCelsius();
    double tempC2 = thermocouple2.readCelsius();
    int lm35Value = analogRead(lm35Pin);
    float voltage = lm35Value * (3.3 / 4095.0);
    float tempC3 = voltage * 100;
    int airQualityValue = analogRead(airQualityPin);

    String json = "{";
    json += "\"tempC1\":" + String(tempC1) + ",";
    json += "\"tempC2\":" + String(tempC2) + ",";
    json += "\"tempC3\":" + String(tempC3) + ",";
    json += "\"airQuality\":" + String(airQualityValue);
    json += "}";

    request->send(200, "application/json", json);
  });

  // Inicia el servidor
  server.begin();
}

void loop() {
  // No es necesario poner nada aquí ya que el servidor maneja las peticiones de manera asíncrona
}
