#pragma once
#include <string>
#include "SensorManager.hpp"
#include "MQTTClient.hpp"


void startTCPClient(SensorManager& manager, MQTTClient& mqtt);

