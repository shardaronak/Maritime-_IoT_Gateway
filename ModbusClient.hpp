#ifndef MODBUS_CLIENT_HPP
#define MODBUS_CLIENT_HPP

#include <string>
#include <modbus/modbus.h>

class ModbusClient {
	public:
		ModbusClient(const std::string& ip, int port);
		~ModbusClient();
		bool readTemperatures(uint16_t* buffer, int count);
	private:
		modbus_t* ctx;
};

#endif

