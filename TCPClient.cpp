#include "TCPClient.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

extern std::string currentTimeUTC();

void startTCPClient(SensorManager& manager, MQTTClient& mqtt) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[WebSocketClient] Socket creation failed\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[WebSocketClient] Connection failed\n";
        close(sock);
        return;
    }

    std::cout << "[WebSocketClient] Connected to 127.0.0.1:8888\n";

     char buffer[512];
     while (true) {
         ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
         if (bytes_received > 0) {
             buffer[bytes_received] = '\0';  // Null terminate
             std::string payload(buffer);
             std::cout << "[WebSocketClient] Received: " << payload << std::endl;

             // Extract value from payload like you do in websocket handler
             size_t c1 = payload.find(',');
             size_t c2 = payload.find(',', c1 + 1);
             if (c1 != std::string::npos && c2 != std::string::npos) {
                 try {
                     double val = std::stod(payload.substr(c1 + 1, c2 - c1 - 1));
                     bool valid = (payload.find(",A*") != std::string::npos);
                     if (manager.shouldPublish("rot", val)) {
                         std::ostringstream msg;
                         msg << val << "°/min, " << (valid ? "Valid" : "Invalid") << ", " << currentTimeUTC();
                         //Publish to MQTT broker
                         mqtt.publish("ows-challenge/mv-sinking-boat/main-crane/rot", msg.str());
                         std::cout << "[MQTT] Published ROT: " << msg.str() << std::endl;
                         manager.updateSensor("rot", val, valid);
                     }
                 } catch (const std::exception& e) {
                     std::cerr << "Parse error: " << e.what() << std::endl;
                 }
             }
         }
         else if (bytes_received == 0) {
             std::cout << "[WebSocketClient] Server closed connection" << std::endl;
             break;
         }
         else {
             perror("recv");
             break;
         }
         std::this_thread::sleep_for(std::chrono::milliseconds(100));  // prevent busy loop
     }

    close(sock);
}

