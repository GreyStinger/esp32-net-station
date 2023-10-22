# Wifi

The wifi system is very simple. If the device is fully and correctly configured and the device is set to connect to an AP then it will do so and get a dynamic IP address but will have a custom MDNS instance setup if the client configured it so.

The device will then look through a list of servers trying to connect to them in the order that they exist in its memory. Once a connection to a server has been established, configs can be changed through the server instead.

There will be a button and an associated LED light to turn on and off the devices AP. The user can set a custom name for the AP, but by default it will just have a hash. and the username and password will be admin until changed.
