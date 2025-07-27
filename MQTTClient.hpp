#ifndef MQTT_CLIENT_HPP
#define MQTT_CLIENT_HPP

#include <string>
#include <mqtt/async_client.h>

class MQTTClient {
public:
    MQTTClient(const std::string& serverURI, const std::string& clientID);
    void connect();
    void publish(const std::string& topic, const std::string& payload);
    void disconnect();
    bool isConnected() const {
        return client.is_connected();
    }
private:
    mqtt::async_client client;
    mqtt::connect_options connOpts;
};

#endif

