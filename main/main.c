#include <driver/i2c.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include "peripherals/tft.h"
#include "lvgl_helpers.h"
#include "lvgl_i2c/i2c_manager.h"
#include "model/model.h"
#include "view/view.h"
#include "peripherals/storage.h"
#include "controller/controller.h"
#include "controller/standby.h"
#include "controller/gui.h"
#include "controller/persistance.h"

static const char *TAG   = "Main";
static mut_model_t model = {0};


void app_main(void) {
    tft_init(standby_poke);
    storage_init();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    
    model_updater_t updater = model_updater_init(&model);
    persistance_load(&model);
    view_init(updater, controller_process_message, disp_driver_flush, tft_touch_read_cb);
    controller_init(updater);

    ESP_LOGI(TAG, "Begin main loop");
    for (;;) {
        controller_manage(updater);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
