#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP

#include <string>
#include <map>
#include <ctime>

struct SensorData {
    double value;
    bool valid;
    time_t lastSentTime;
};

class SensorManager {
public:
    SensorManager();
    bool shouldPublish(const std::string& key, double newValue);
    void updateSensor(const std::string& key, double newValue, bool valid);
private:
    std::map<std::string, SensorData> sensors;
};

#endif

