# Config

Configs are going to be kept in NVS storage.

If they are unable to be read or do not exist they will return null. In this case the configs that are not available will be checked and if they are vital we will flash a GPIO pin red.

This indicates that the device still has to be setup. For this the client will have to connect to the device either through a future bluetooth implementation with an app or currently through an open web interface. The device will automatically toggle on its wifi AP.
