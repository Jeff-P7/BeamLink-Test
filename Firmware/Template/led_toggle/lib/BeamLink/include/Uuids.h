#pragma once
/**
 * @file Uuids.h
 * @brief BLE Service and Characteristic UUIDs for BeamLink
 * 
 * This file defines the BLE service and characteristic UUIDs used by the BeamLink library.
 * These UUIDs should remain stable across firmware updates to maintain compatibility.
 */

// Service UUID for BeamLink BLE service
// This is the main service that clients will discover
#define BMLK_SERVICE_UUID "12345678-1234-1234-1234-1234567890ab"

// Characteristic UUID (Read + Write + Notify)
// Used for bidirectional communication with the client
#define BMLK_CHARACTERISTIC_UUID "12345678-1234-1234-1234-1234567890ac"
