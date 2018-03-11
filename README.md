Simple walking direction sensor. Designed to send MQTT messages
back to HA.

You will need to define a file secret.h that contains definitions
for the following.

AIO_SERVER
AIO_USERNAME
AIO_KEY
AIIO_SERVERPORT
WLAN_SSID
WLAN_PASS
DIRECTION_FEED

The key and username may or maynot be important for your broker
but they are required to use the Adafruit MQTT library
