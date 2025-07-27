#include "MQTTClient.hpp"
#include <iostream>
#include <mqtt/will_options.h>

MQTTClient::MQTTClient(const std::string& serverURI, const std::string& clientID)
    : client(serverURI, clientID) {
    connOpts.set_keep_alive_interval(600);
    connOpts.set_clean_session(true);
    mqtt::will_options will(
            "ows-challenge/mv-sinking-boat/status",  // topic
            std::string("connection lost"),    // payload
            1,                                       // qos
            false                                    // retained
            );
    connOpts.set_will(will);
}

void MQTTClient::connect() {
    try {
        client.connect(connOpts)->wait();
        std::cout << "MQTT Connected\n";
    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT Connection failed: " << exc.what() << "\n";
    }
}

void MQTTClient::publish(const std::string& topic, const std::string& payload) {
    if (!client.is_connected()) {
        std::cerr << "[MQTT] Not connected. Skipping publish.\n";
        return;
    }
    try {
        auto tok = client.publish(topic, payload.c_str(), payload.size(), 1, false);
        tok->wait();
        std::cout << "[MQTT] Published: " << payload << " to topic: " << topic << "\n";
    } catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT] Publish failed: " << exc.what() << "\n";
    }
}

void MQTTClient::disconnect() {
    client.disconnect()->wait();
}

