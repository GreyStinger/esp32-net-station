#ifndef _STATION_CONFIG
#define _STATION_CONFIG

// Hostname and MDNS instance name for local machines
#define MDNS_INSTANCE "Example MDNS Instance"
#define HOSTNAME "Example Local Hostname"

// Local wifi configurations
#define ESP_WIFI_SSID "Example SSID"
#define ESP_WIFI_PASSWORD "Example Password"

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