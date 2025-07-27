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

---

## **Data Flow**

- Reads **temperature data** from Modbus registers (0–3).
- Reads **ROT data** from TCP server streaming NMEA sentences: $MGROT,2.0,A*33


- `2.0` → Rate of Turn (°/min)
- `A` → Valid data
- Applies filtering:
- Publish if **value changes > 1** or **5 minutes passed since last update**.
- Publishes to **MQTT topics** on HiveMQ broker.
---

## **MQTT Topics**

-  ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-1
-  ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-2
-  ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-3
-  ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-4
-  ows-challenge/mv-sinking-boat/main-crane/rot

---

## **Example MQTT Payload**
26.0°C, Valid, 2025-07-27 at 12:30 UTC

---

## **Project Structure**

-  SensorManager.* -> Handles filtering and last-sent tracking
-  MQTTClient.* -> MQTT connection and publishing logic
-  ModbusClient.* -> Modbus TCP client for temperature sensors
-  TCPClient.* -> TCP client for ROT sensor (parses NMEA)
-  main.cpp -> Entry point, orchestrates threads

---
## **Compile**

g++ -std=c++17 \
    main.cpp SensorManager.cpp MQTTClient.cpp ModbusClient.cpp TcpClient.cpp \
    -lpaho-mqttpp3 -lpaho-mqtt3as \
    -lmodbus \
    -lboost_system -lboost_thread \
    -pthread \
    -o MaritimeIoTGateway
---

## **Run**
./MaritimeIoTGateway
---

## **Input Console Prints**
```text
[root@node1 crane_simulation]# python crane_simulation.py
Measured Value: [56, 94, 6, 93]
Serving on ('127.0.0.1', 8888)
Measured Value: [43, 91, 4, 85]
Client Connected: ('127.0.0.1', 54862)
Send: '$MGROT,288.0,A*2E'
Measured Value: [12, 2, 11, 9]
Send: '$MGROT,7.0,A*2B'
Measured Value: [5, 2, 58, 38]
Send: '$MGROT,248.0,A*22'
Measured Value: [22, 28, 87, 71]
Send: '$MGROT,215.0,V*3D'
Measured Value: [8, 43, 96, 37]
Send: '$MGROT,134.0,A*2A'
Measured Value: [74, 13, 51, 96]
Send: '$MGROT,133.0,A*2D'
Measured Value: [20, 43, 44, 8]
Send: '$MGROT,285.0,A*23'
Measured Value: [49, 68, 77, 31]
Send: '$MGROT,292.0,A*25'
Measured Value: [54, 96, 37, 16]
Send: '$MGROT,347.0,A*2C'
Measured Value: [95, 50, 78, 41]
Send: '$MGROT,316.0,A*28'
Measured Value: [35, 14, 99, 67]
Send: '$MGROT,208.0,A*26'
Measured Value: [88, 42, 18, 87]
Send: '$MGROT,10.0,A*1D'
Measured Value: [74, 31, 34, 55]
Send: '$MGROT,231.0,A*2C'
Measured Value: [14, 80, 3, 15]
```text

## **Output Console Prints**
```text

[root@node1 new_gateway]# ./MaritimeIoTGateway
MQTT Connected
[WebSocketClient] Connected to 127.0.0.1:8888
[WebSocketClient] Received: $MGROT,288.0,A*2E
[MQTT] Published: 43°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-1
[Modbus] Published temp1: 43°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 288°/min, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/rot
[MQTT] Published ROT: 288°/min, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 91°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-2
[Modbus] Published temp2: 91°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 4°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-3
[Modbus] Published temp3: 4°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 85°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-4
[Modbus] Published temp4: 85°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 12°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-1
[Modbus] Published temp1: 12°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 2°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-2
[Modbus] Published temp2: 2°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 11°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-3
[Modbus] Published temp3: 11°C, Valid, 2025-07-27 at 18:38 UTC
[MQTT] Published: 9°C, Valid, 2025-07-27 at 18:38 UTC to topic: ows-challenge/mv-sinking-boat/main-crane/luffing/temp-mot-4
[Modbus] Published temp4: 9°C, Valid, 2025-07-27 at 18:38 UTC
[WebSocketClient] Received: $MGROT,7.0,A*2B


