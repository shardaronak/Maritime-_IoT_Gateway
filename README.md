# Maritime IoT Gateway

## **Overview**
This project implements a **Maritime IoT Gateway** for a crane system on a vessel. The gateway collects sensor data from multiple sources, applies filtering logic, and publishes the processed information to an MQTT broker for cloud integration.

### **Sensors Supported**
- **4 Temperature Sensors** via **Modbus TCP**
- **Rate of Turn (ROT) Sensor** via **TCP Socket** (NMEA 0183 format)

The application is modular, multi-threaded, and optimized to reduce unnecessary data transmission.

---

## **Features**
✔ Reads multiple sensors concurrently (Modbus & TCP)  
✔ Filters updates to reduce bandwidth usage  
✔ Publishes structured data with timestamp and validity  
✔ Implements Last Will & Testament (LWT) for connection loss detection  
✔ Works with simulation scripts provided by MacGregor  

---

## **System Design**
### **Workflow**
1. **Modbus TCP (Temperature Sensors)**
   - Connects to a PLC-like Modbus TCP server
   - Reads registers 0–3 for motor temperatures every 2 seconds
2. **TCP Socket (ROT Sensor)**
   - Connects to a TCP server streaming NMEA sentences like:
     ```
     $MGROT,2.0,A*33
     ```
   - Extracts ROT value and validity
3. **Filtering**
   - Publish only if:
     - Difference > **1°C** (temperature) or **1°/min** (ROT), OR
     - **5 minutes** since last update
4. **MQTT Publishing**
   - Publishes to `broker.hivemq.com` under structured topics
   - Example message:
     ```
     "26.0°C, Valid, 2025-07-27 at 12:30 UTC"
     ```

## **System Architecture**
+-------------------+ +-------------------+ +-------------------+
| Temperature | | ROT | | |
| Sensors (x4) | | Sensor (NMEA) | | MQTT Broker |
| via Modbus TCP | | via TCP | | broker.hivemq.com |
+---------+---------+ +---------+---------+ +---------+---------+
| | |
v v |
+--------------+ +----------------+ |
| Modbus | | TCP Client | |
| Client | | (NMEA Parser) | |
+------+-------+ +-------+--------+ |
| | |
+------------+--------------+ |
v |
+--------------------+ |
| Filtering Logic | |
| (SensorManager) | |
+---------+----------+ |
| |
v v
+---------------+ +--------------------------+
| MQTT Client |------------------>| Cloud / Dashboard |
+---------------+ +--------------------------+



---

## **Data Flow**
- Reads **temperature data** from Modbus registers (0–3).
- Reads **ROT data** from TCP server streaming NMEA sentences:
$MGROT,2.0,A*33


- `2.0` → Rate of Turn (°/min)
- `A` → Valid data
- Applies filtering:
- Publish if **value changes > 1** or **5 minutes passed since last update**.
- Publishes to **MQTT topics** on HiveMQ broker.

---

## **MQTT Topics**
- Reads ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-1
- Reads ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-2
- Reads ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-3
- Reads ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-4
- Reads ows-challenge/mv-sinking-boat/main-crane/rot

---

## **Example MQTT Payload**
26.0°C, Valid, 2025-07-27 at 12:30 UTC

---

## **Project Structure**
- Reads SensorManager.* -> Handles filtering and last-sent tracking
- Reads MQTTClient.* -> MQTT connection and publishing logic
- Reads ModbusClient.* -> Modbus TCP client for temperature sensors
- Reads TCPClient.* -> TCP client for ROT sensor (parses NMEA)
- Reads main.cpp -> Entry point, orchestrates threads

