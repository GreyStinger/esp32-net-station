#pragma once
#ifndef _STATION_CONFIG
#define _STATION_CONFIG

// Hostname and MDNS instance name for local machines
#define MDNS_INSTANCE "esp-net"
#define HOSTNAME "sprite-1"

// Local wifi configurations
#define ESP_WIFI_SSID "Grey-2.4"
#define ESP_WIFI_PASSWORD "J@yd3n1Sec"
#define ESP_IP "192.168.1.60"
#define ESP_WEB_SOCKET_SERVER "192.168.1.37"

// Idk
#define N_GPIOS_IN 1
#define N_GPIOS_OUT 2

// heh
int GPIOS_IN[N_GPIOS_IN];
int GPIOS_OUT[N_GPIOS_OUT];

// Mount point in VFS
// Recommended to leave unless you know exactly what you are doing
#define SITE_MOUNT_POINT "/www"

#define PV_KEY "<key>"

#endif
