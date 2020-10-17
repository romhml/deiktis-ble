#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "modlog/modlog.h"
#include "nimble/ble.h"

#define GATT_DEVICE_INFO_UUID 0x180A
#define GATT_MANUFACTURER_NAME_UUID 0x2A29
#define GATT_MODEL_NUMBER_UUID 0x2A24

extern uint16_t hrs_hrm_handle;

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

void gatt_server_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_server_init(void);

uint16_t hrs_hrm_handle;

static int gatt_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt,
                                   void *arg);
