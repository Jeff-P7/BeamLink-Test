#include "BeamLink.h"
#include "Uuids.h"

// Callback classes
class BeamLink::ServerCallbacks : public NimBLEServerCallbacks {
public:
  explicit ServerCallbacks(BeamLink* beamLink) : beamLink(beamLink) {}
  
  void onConnect(NimBLEServer* pServer) override {
    if (beamLink) {
      beamLink->deviceConnected = true;
      Serial.println("Client connected");
    }
  }

  void onDisconnect(NimBLEServer* pServer) override {
    if (beamLink) {
      beamLink->deviceConnected = false;
      Serial.println("Client disconnected, restarting advertising");
      NimBLEDevice::startAdvertising();
    }
  }
  
private:
  BeamLink* beamLink;
};

class BeamLink::RxCallbacks : public NimBLECharacteristicCallbacks {
public:
  explicit RxCallbacks(BeamLink* beamLink) : beamLink(beamLink) {}
  
  void onWrite(NimBLECharacteristic* pCharacteristic) override {
    if (!beamLink) return;
    
    std::string rxValue = pCharacteristic->getValue();
    
    if (rxValue.length() > 0) {
      beamLink->messagesReceived++;
      Serial.printf("RX [%u]: %s\n", beamLink->messagesReceived, rxValue.c_str());
      
      if (beamLink->messageHandler) {
        // Reply function that sends via TX notify
        auto reply = [this](const std::string& msg) {
          if (beamLink) {
            beamLink->notify(msg);
          }
        };
        
        beamLink->messageHandler(rxValue, reply);
      }
    }
  }
  
private:
  BeamLink* beamLink;
};

// BeamLink implementation
BeamLink::BeamLink() : deviceConnected(false), initialized(false) {
  // Initialize callback objects
  serverCallbacks = std::make_unique<ServerCallbacks>(this);
  rxCallbacks = std::make_unique<RxCallbacks>(this);
}

BeamLink::~BeamLink() {
  end();
}

bool BeamLink::begin(const char* deviceName, int8_t advPowerDbm, uint16_t advIntervalMs,
                     const char* serviceUuid, const char* characteristicUuid) {
  if (initialized) {
    Serial.println("BeamLink already initialized");
    return false;
  }
  
  if (!deviceName || strlen(deviceName) == 0) {
    Serial.println("Invalid device name");
    return false;
  }
  
  // Validate parameters
  if (advPowerDbm < -12 || advPowerDbm > 9) {
    Serial.printf("Warning: Invalid advertising power %d dBm, clamping to range [-12, 9]\n", advPowerDbm);
    advPowerDbm = std::max((int8_t)-12, std::min((int8_t)9, advPowerDbm));
  }
  
  if (advIntervalMs < 20 || advIntervalMs > 10240) {
    Serial.printf("Warning: Invalid advertising interval %d ms, clamping to range [20, 10240]\n", advIntervalMs);
    advIntervalMs = std::max((uint16_t)20, std::min((uint16_t)10240, advIntervalMs));
  }
  
  this->deviceName = deviceName;
  
  // Set UUIDs (use defaults from Uuids.h if not provided)
  this->serviceUuid = serviceUuid ? serviceUuid : BMLK_SERVICE_UUID;
  this->characteristicUuid = characteristicUuid ? characteristicUuid : BMLK_CHARACTERISTIC_UUID;
  
  Serial.println("Initializing BeamLink BLE...");
  Serial.printf("  Advertising Power: %d dBm\n", advPowerDbm);
  Serial.printf("  Advertising Interval: %d ms\n", advIntervalMs);
  
  // Initialize BLE device
  NimBLEDevice::init(deviceName);
  Serial.println("BLE device initialized");
  
  // Set BLE power
  NimBLEDevice::setPower(static_cast<esp_power_level_t>(advPowerDbm), ESP_BLE_PWR_TYPE_ADV);
  NimBLEDevice::setPower(static_cast<esp_power_level_t>(advPowerDbm), ESP_BLE_PWR_TYPE_DEFAULT);
  
  // Create BLE Server
  pServer = NimBLEDevice::createServer();
  if (!pServer) {
    Serial.println("Failed to create BLE server");
    return false;
  }
  
  pServer->setCallbacks(serverCallbacks.get());
  
  // Set MTU to maximum supported value for larger message support
  // Default is 23 bytes, we request 512 bytes (maximum for NimBLE)
  NimBLEDevice::setMTU(512);
  
  // Setup service and characteristics
  if (!setupService()) {
    Serial.println("Failed to setup BLE service");
    return false;
  }
  
  // Start advertising with configured interval
  if (!startAdvertising(advIntervalMs)) {
    Serial.println("Failed to start advertising");
    return false;
  }
  
  initialized = true;
  startTime = millis();
  messagesReceived = 0;
  messagesSent = 0;
  errorCount = 0;
  
  Serial.printf("BeamLink ready, advertising as: %s\n", deviceName);
  Serial.printf("Service UUID: %s\n", this->serviceUuid.c_str());
  Serial.printf("Characteristic UUID: %s\n", this->characteristicUuid.c_str());
  Serial.printf("MTU: %d bytes\n", NimBLEDevice::getMTU());
  
  return true;
}

bool BeamLink::setupService() {
  if (!pServer) return false;
  
  // Create BLE Service
  NimBLEService* pService = pServer->createService(serviceUuid.c_str());
  if (!pService) {
    Serial.println("Failed to create BLE service");
    return false;
  }
  
  // Create Main Characteristic (Read + Write + WriteNoResponse + Notify)
  pChar = pService->createCharacteristic(
    characteristicUuid.c_str(),
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::NOTIFY
  );
  if (!pChar) {
    Serial.println("Failed to create characteristic");
    return false;
  }
  
  pChar->setCallbacks(rxCallbacks.get());
  
  // Start the service
  if (!pService->start()) {
    Serial.println("Failed to start BLE service");
    return false;
  }
  
  return true;
}

bool BeamLink::startAdvertising(uint16_t intervalMs) {
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  if (!pAdvertising) {
    Serial.println("Failed to get advertising object");
    return false;
  }
  
  pAdvertising->addServiceUUID(serviceUuid.c_str());
  pAdvertising->setScanResponse(true);
  
  // Convert ms to BLE units (0.625ms per unit)
  // intervalMs / 0.625 = intervalMs * 1.6
  uint16_t intervalUnits = (intervalMs * 16) / 10;
  
  // BLE spec: minimum is 20ms (32 units), maximum is 10240ms (16384 units)
  intervalUnits = std::max((uint16_t)32, std::min((uint16_t)16384, intervalUnits));
  
  pAdvertising->setMinInterval(intervalUnits);
  pAdvertising->setMaxInterval(intervalUnits);
  
  if (!NimBLEDevice::startAdvertising()) {
    Serial.println("Failed to start advertising");
    return false;
  }
  
  return true;
}

void BeamLink::onMessage(MessageHandler handler) {
  messageHandler = handler;
}

bool BeamLink::notify(const std::string& msg) {
  if (!initialized || !pChar || !deviceConnected) {
    errorCount++;
    return false;
  }
  
  if (msg.length() == 0) {
    errorCount++;
    return false;
  }
  
  // Validate message size (MTU - 3 bytes for ATT header)
  uint16_t maxSize = NimBLEDevice::getMTU() - 3;
  if (msg.length() > maxSize) {
    Serial.printf("Warning: Message size %zu exceeds MTU %u, truncating\n", msg.length(), maxSize);
    errorCount++;
    // Truncate message to fit MTU
    std::string truncated = msg.substr(0, maxSize);
    pChar->setValue(truncated);
  } else {
    pChar->setValue(msg);
  }
  
  pChar->notify();
  messagesSent++;
  Serial.printf("TX [%u]: %s\n", messagesSent, msg.c_str());
  
  return true;
}

uint16_t BeamLink::getMTU() const {
  if (!initialized) return 23; // Default BLE MTU
  return NimBLEDevice::getMTU();
}

unsigned long BeamLink::getUptime() const {
  if (!initialized) return 0;
  return millis() - startTime;
}

void BeamLink::resetStats() {
  messagesReceived = 0;
  messagesSent = 0;
  errorCount = 0;
  startTime = millis();
  Serial.println("Statistics reset");
}

void BeamLink::loop() {
  // Reserved for future use (timers, heartbeats, etc.)
  // Reduced delay for better responsiveness
  delay(1);
}

void BeamLink::end() {
  if (initialized) {
    deviceConnected = false;
    initialized = false;
    
    if (pServer) {
      pServer->getAdvertising()->stop();
    }
    
    pChar = nullptr;
    pServer = nullptr;
    
    NimBLEDevice::deinit(true);
    
    Serial.println("BeamLink ended");
  }
}
