/* BLE ping-pong
 * Each node listens for 5 s.
 *   - heard "ping"  -> answer with "pong"
 *   - heard nothing    -> send "ping"
 * Messages travel as non-connectable advertisements
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"

#define LISTEN_MS 5000
#define ADV_MS    1000

static const char *TAG = "pingpong";
static uint8_t own_addr_type;
static TaskHandle_t app_task; //DOC: https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate

/* Advertise "msg" for ADV_MS, then stop. */
static void send(const char *msg)
{
    uint8_t mfg[2 + 24] = {0xFF, 0xFF};         /* 2 bytes company code. 0xFFFF for test */

    // fill the rest of the array with the message to send
    size_t len = strlen(msg);
    memcpy(&mfg[2], msg, len);

    struct ble_hs_adv_fields fields = {0};
    fields.mfg_data = mfg;
    fields.mfg_data_len = 2 + len;
    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params params = {0};
    params.conn_mode = BLE_GAP_CONN_MODE_NON;
    params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &params, NULL, NULL); //DOC https://mynewt.apache.org/latest/network/ble_hs/ble_gap.html

    ESP_LOGI(TAG, "TX: %s", msg);
    vTaskDelay(pdMS_TO_TICKS(ADV_MS));
    ble_gap_adv_stop();
}

/* Scan callback: wake the main task when a "hello" arrives. */
static int on_scan(struct ble_gap_event *event, void *arg)
{
    struct ble_hs_adv_fields f;

    if (event->type != BLE_GAP_EVENT_DISC ||
        ble_hs_adv_parse_fields(&f, event->disc.data, event->disc.length_data) ||
        f.mfg_data_len < 2 || f.mfg_data[0] != 0xFF || f.mfg_data[1] != 0xFF) {
        return 0;
    }
    

    const char *msg = (const char *)&f.mfg_data[2];
    int len = f.mfg_data_len - 2;
    // const uint8_t *addr = event->disc.addr.val;   /* senders address, stored little-endian */
    // ESP_LOGI(TAG, "RX from %02x:%02x:%02x:%02x:%02x:%02x", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    ESP_LOGI(TAG, "Received msg: %.*s, respond pong", len, msg);

    if (len == 4 && memcmp(msg, "ping", 4) == 0) {
        xTaskNotifyGive(app_task);
    }
    return 0;
}


//  the main pingpong task
static void pingpong_task(void *arg)
{
    // discovery parameters
    struct ble_gap_disc_params scan = {0};
    scan.passive = 1;
    scan.filter_duplicates = 1;

    while (1) {
        TickType_t start = xTaskGetTickCount(); // get the tick count at the start of this routine
        TickType_t window = pdMS_TO_TICKS(LISTEN_MS); 

        ulTaskNotifyTake(pdTRUE, 0);    // clear any stale notification
        ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &scan, on_scan, NULL);// start discovering. Set on_scan as callback

        TickType_t elapsed;
        while ((elapsed = xTaskGetTickCount() - start) < window) { // scan for the remainder of the interval
            if (ulTaskNotifyTake(pdTRUE, window - elapsed)) {
                ble_gap_disc_cancel();
                send("pong");
                ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &scan, on_scan, NULL);  // start discovering. Set on_scan as callback
            }
        }

        ble_gap_disc_cancel();  //stop discovering
        send("ping");           // send ping
    }
}



static void on_sync(void)
{
    ble_hs_id_infer_auto(0, &own_addr_type); //DOC: https://mynewt.apache.org/latest/network/ble_hs/ble_hs_id.html
    xTaskCreate(pingpong_task, "pingpong", 4096, NULL, 5, &app_task);
}

static void host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void app_main(void)
{
    esp_log_level_set("NimBLE", ESP_LOG_NONE); //disable logging from NIMBLE
    nvs_flash_init();
    nimble_port_init();
    ble_hs_cfg.sync_cb = on_sync; //this callback is called once the nimble init is ready
    nimble_port_freertos_init(host_task);
}