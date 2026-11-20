This is a simple example for a BLE pingpong using nimble.

Two nodes periodically transmit a "ping" message, and are otherwise in reception.
On reception of a "ping" message, they respond with a "pong".

In this example, the transmissions are broadcast, so do not require a BLE connection.
For more intricate usage, i.e. connecting with mobile phone, see example in ble_mobile

Further examples using nimble can be found at https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/ble_get_started/nimble

or, for general BLE example: https://github.com/espressif/esp-idf/tree/master/examples/bluetooth

Also refer to the Documentation:
https://mynewt.apache.org/latest/network/index.html
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/nimble/index.html
