#include "mbed.h"
#include "BLEDevice.h"

DigitalOut relayPIN(LED1);
BLEDevice  ble;

uint8_t relayState;

bool rxPayloadUpdated = false;

const char *deviceName = "Smartlet";

char rxPayload[1] = {0,};

void disconnectionCallback(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    ble.startAdvertising();
}

void onDataWritten(const GattCharacteristicWriteCBParams *params)
{
    strncpy(rxPayload, (const char *)params->data, 1);
    rxPayloadUpdated = true;
}

int main() {

    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);

    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME, (const uint8_t *)deviceName, strlen(deviceName));
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    
    ble.startAdvertising();

    GattCharacteristic toggleCharacteristic(0x2222, 
                                            &relayState, 
                                            sizeof(relayState), 
                                            sizeof(relayState), 
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);
                         
    GattCharacteristic *charTable[] = {&toggleCharacteristic};
    GattService toggleService(0x2220, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
    ble.addService(toggleService);

    while (true) {
        if (rxPayloadUpdated)
            relayPIN = rxPayload[0];
        ble.waitForEvent();
    }
}
