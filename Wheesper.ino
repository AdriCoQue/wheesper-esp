#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <vector>
// Configuración de la red
const char* ssid = "esp32test";
const char* password = "cuaderno";

// Configuración de los pines
const int led_pin = 2;
const int WAITING_TIME = 250;

// Pines motores
const int motor1A = 14;
const int motor1B = 27;
const int motor2A = 26;
const int motor2B = 33;

// Configuración de la conexión Wi-Fi
void connect_wifi() {
    WiFi.begin(ssid, password);
    Serial.println("Conectando a la red Wi-Fi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conexión fallida. Reintentando...");
    }
    digitalWrite(led_pin, HIGH);
    Serial.println("Conexión establecida:");
    Serial.println(WiFi.localIP());
}

// Configuración del servidor web
WebServer server(80);

// Movimientos disponibles para el carro
void moveLeft() {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, HIGH);
    digitalWrite(motor2B, LOW);
    Serial.println("Izquierdeando...");
    delay(500);
    moveStop();
}
void moveRight() {
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, LOW);
    Serial.println("Derecheando...");
    delay(500);
    moveStop();
}
void moveForward() {
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, HIGH);
    digitalWrite(motor2B, LOW);
    Serial.println("Avanzando...");
    delay(500);
    moveStop();
}
void moveBackward() {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, HIGH);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
    Serial.println("Retrocediendo...");
    delay(500);
    moveStop();
}
void moveStop() {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, LOW);
    Serial.println("Deteniendo...");
}

void handleInstructions() {
    // Obtener el cuerpo de la solicitud HTTP
    String body = server.arg("plain");

    // Eliminar los corchetes "[" y "]" del cuerpo
    body.replace("[", "");
    body.replace("]", "");
    
    // Definir un delimitador para separar los elementos del arreglo
    const char delimiter = ',';

    // Crear un vector de Strings para almacenar los elementos del arreglo
    std::vector<String> instructions;

    // Variables auxiliares para el análisis del cuerpo de la solicitud
    String instruction = "";

    // Recorrer el cuerpo de la solicitud
    for (size_t i = 0; i < body.length(); i++) {
        char currentChar = body[i];

        // Si el carácter actual es un delimitador
        if (currentChar == delimiter) {
            // Agregar la instrucción actual al vector de instrucciones
            instructions.push_back(instruction);

            // Limpiar la variable de instrucción para la próxima instrucción
            instruction = "";
        } else {
            // Agregar el carácter actual a la instrucción actual
            instruction += currentChar;
        }
    }

    // Agregar la última instrucción al vector de instrucciones
    instructions.push_back(instruction);

    // Enviar una respuesta al cliente con las instrucciones recibidas
    server.send(200, "text/plain", "Instrucciones recibidas correctamente y ejecutadas con éxito");

    for (size_t i = 0; i < instructions.size(); i++) {
        String instruction = instructions[i];
        instruction.remove('"');
        Serial.println(instruction);

        // Procesar la instrucción
        if (instruction.equals("\"detente\"")) {
            moveStop();
        } else if (instruction.equals("\"izquierda\"")) {
            moveLeft();
        } else if (instruction.equals("\"derecha\"")) {
            moveRight();
        } else if (instruction.equals("\"avanza\"")) {
            moveForward();
        } else if (instruction.equals("\"retrocede\"")) {
            moveBackward();
        } else {
            // Manejar instrucciones desconocidas
        }

        // Imprimir el estado de los pines
        Serial.println(String(digitalRead(14)) + " " + String(digitalRead(27)) + " " + String(digitalRead(26)) + " " + String(digitalRead(33)));
        
        // Esperar un tiempo antes de procesar la siguiente instrucción
        delay(WAITING_TIME);
    }
}

void setup() {
    Serial.begin(9600);
    
    pinMode(motor1A, OUTPUT);
    pinMode(motor1B, OUTPUT);
    pinMode(motor2A, OUTPUT);
    pinMode(motor2B, OUTPUT);
    pinMode(led_pin, OUTPUT);

    connect_wifi();
    server.on("/instructions", HTTP_POST, handleInstructions);

    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

void loop() {
    server.handleClient();
}
