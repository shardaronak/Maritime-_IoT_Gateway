#include "SensorManager.hpp"
#include <cmath>

SensorManager::SensorManager() {
    sensors = {
        {"temp1", {0.0, true, 0}},
        {"temp2", {0.0, true, 0}},
        {"temp3", {0.0, true, 0}},
        {"temp4", {0.0, true, 0}},
        {"rot",   {0.0, true, 0}}
    };
}

bool SensorManager::shouldPublish(const std::string& key, double newValue) {
    auto& s = sensors[key];
    time_t now = time(nullptr);
    if (s.lastSentTime == 0) return true;
    if (fabs(newValue - s.value) > 1.0) return true;
    if (difftime(now, s.lastSentTime) >= 300.0) return true;
    return false;
}

void SensorManager::updateSensor(const std::string& key, double newValue, bool valid) {
    auto& s = sensors[key];
    s.value = newValue;
    s.valid = valid;
    s.lastSentTime = time(nullptr);
}

