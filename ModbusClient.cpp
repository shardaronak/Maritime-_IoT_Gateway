#include "ModbusClient.hpp"
#include <stdexcept>

ModbusClient::ModbusClient(const std::string& ip, int port) {
    ctx = modbus_new_tcp(ip.c_str(), port);
    if (!ctx) throw std::runtime_error("Modbus context alloc failed");
    if (modbus_connect(ctx) == -1)
        throw std::runtime_error("Modbus connect failed");
}

ModbusClient::~ModbusClient() {
    if (ctx) {
        modbus_close(ctx);
        modbus_free(ctx);
    }
}

bool ModbusClient::readTemperatures(uint16_t* buffer, int count) {
    return modbus_read_registers(ctx, 0, count, buffer) == count;
}

