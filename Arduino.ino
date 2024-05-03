#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h> 
#define DHTPIN 15     
#define DHTTYPE DHT22  

#define triggerPin 5   
#define echoPin 18      
bool executed = false;

long duration;
int distance;
int realdistance;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.print("Conectando al WiFi");
  
  // Conexión a la red WiFi
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Conectado!");

  dht.begin();
}

void loop() {
  // Verificar si el código aún no se ha ejecutado
  if (!executed) {
    // Cambiar el estado de la variable ejecutada
    executed = true;

    // Crear un objeto JSON para almacenar los datos
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["distance"] = realdistance;
    jsonDoc["temperature"] = dht.readTemperature();
    jsonDoc["humidity"] = dht.readHumidity();

    // Serializar el objeto JSON a una cadena
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    Serial.println(jsonString);
    
    // Realizar las solicitudes HTTP
    HTTPClient http;

    // Método POST
    http.begin("AQUI VA LA URL DE TU SERVIDOR"); // Especificar la URL
    http.addHeader("Content-Type", "application/json"); // Establecer el tipo de contenido JSON
    int httpCode = http.POST(jsonString); // Enviar los datos JSON
    
    if (httpCode > 0) { // Verificar el código de respuesta
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error en la solicitud HTTP");
      Serial.println(httpCode);
    }
    http.end(); // Liberar los recursos HTTP

    // Medición de distancia con el sensor ultrasónico

    // Enviamos un pulso ultrasónico
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    // Medimos la duración del eco
    duration = pulseIn(echoPin, HIGH);

    // Calculamos la distancia en centímetros
    distance = ((duration * 0.034 / 2) + 10);
    realdistance = 200 - distance + 20;

    // Mostramos la distancia en el monitor serial
    Serial.print("Distancia: ");
    Serial.print(realdistance);
    Serial.println(" cm");
  }

  // Medición de temperatura y humedad
  Serial.print("El ambiente de la habitacion es:... ");
  
  float temperature = dht.readTemperature(); // Leer temperatura en grados Celsius
  float humidity = dht.readHumidity(); // Leer humedad relativa
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Fallo el sensor!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print(" °C, Humedad: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Esperar 30 minutos antes de la próxima medición
  delay(30 * 60 * 1000);  // 30 minutos en milisegundos
}

