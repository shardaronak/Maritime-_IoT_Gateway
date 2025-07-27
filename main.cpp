#include "SensorManager.hpp"
#include "MQTTClient.hpp"
#include "ModbusClient.hpp"
#include "TCPClient.hpp"
#include <thread>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iostream>

using namespace std;

string currentTimeUTC() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d at %H:%M UTC", gmtime(&now));
    return buf;
}

int main() {
    SensorManager manager;
    MQTTClient mqtt("tcp://broker.hivemq.com:1883", "ows-challenge-270725");
    //MQTTClient mqtt("tcp://localhost:1883", "ows-challenge-270725");
    mqtt.connect();
    if (!mqtt.isConnected()) {   
        std::cerr << "MQTT connection failed. Exiting.\n";
        return 1;
    }

    ModbusClient modbus("127.0.0.1", 8889);

    // Modbus thread
    thread t1([&](){
        uint16_t regs[4];
        while (true) {
            if (modbus.readTemperatures(regs, 4)) {
                for (int i = 0; i < 4; ++i) {
                    string key = "temp" + to_string(i+1);
                    double val = regs[i];
                    if (manager.shouldPublish(key, val)) {
                        ostringstream msg;
                         msg << val << "°C, Valid, " << currentTimeUTC();
                         mqtt.publish("ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-" + to_string(i+1), msg.str());
                         manager.updateSensor(key, val, true);
                         cout << "[Modbus] Published " << key << ": " << msg.str() << endl;
                    }
                }
            }
            this_thread::sleep_for(chrono::seconds(2));
        }
    });

    // NMEA thread
    std::thread t2([&]() {
            startTCPClient(manager,mqtt);
            });

    t1.join();
    t2.join();
    mqtt.disconnect();
    return 0;
}

