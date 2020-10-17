#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "gatt_server.h"

static const char *manuf_name = "Artio";
static const char *model_num = "Zero";

struct GattCharacteristic {
  int uuid;
  int val_handle;
  int flags;
  int (*read_callback)(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg);
} gatt_characteristic;

struct GattService {
  int type;
  int uuid;
  struct GattCharacteristic characteristics[];
} gatt_service;

struct ble_gatt_svc_def ble_gatt_services[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
     .characteristics =
         (struct ble_gatt_chr_def[]){
             {
                 /* Characteristic: * Manufacturer name */
                 .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
                 .access_cb = gatt_access_device_info,
                 .flags = BLE_GATT_CHR_F_READ,
             },

             {
                 /* Characteristic: Model number string */
                 .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
                 .access_cb = gatt_access_device_info,
                 .flags = BLE_GATT_CHR_F_READ,
             },

             {
                 0, /* No more characteristics in this service */
             },
         }},
    {0},
};

static int gatt_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt,
                                   void *arg) {
  uint16_t uuid;
  int rc;

  uuid = ble_uuid_u16(ctxt->chr->uuid);

  if (uuid == GATT_MODEL_NUMBER_UUID) {
    rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  if (uuid == GATT_MANUFACTURER_NAME_UUID) {
    rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  assert(0);
  return BLE_ATT_ERR_UNLIKELY;
}

int gatt_server_init(void) {
  int rc;
  ble_svc_gap_init();
  ble_svc_gatt_init();

  rc = ble_gatts_count_cfg(ble_gatt_services);
  if (rc != 0) {
    return rc;
  }

  rc = ble_gatts_add_svcs(ble_gatt_services);
  if (rc != 0) {
    return rc;
  }

  return 0;
}
