#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"

#include "LCD_i2c/lcd.h"

#include "web_server.c"
#include "wifi_connection.h"
#include "plc.c"
//#include "main.h"

#define I2C_MASTER_SCL_IO           14      /*!< Pin SCL (Reloj) */
#define I2C_MASTER_SDA_IO           27    /*!< Pin SDA (Datos) */
#define TEST_I2C_PORT              0
//#define I2C_CLK_SRC_DEFAULT        100000 /*!< Frecuencia de I2C */
#define LCD_ADDR                    0x27    /*!< Dirección I2C del LCD (puede variar) */


// configurar el LCD
i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = TEST_I2C_PORT,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

i2c_master_bus_handle_t bus_handle;


i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = LCD_ADDR,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;

void app_main(){

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    lcd_init(dev_handle, 0x27);
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    // Limpio LCD
    lcd_string("Connecting wifi");

    connect_wifi();

    lcd_clear();
    lcd_string("Starting Web");
    lcd_set_cursor(1,0);
    lcd_string("Server...");

    //subo servidor web
    start_webserver();

    lcd_clear();
    lcd_string("Running PLC.");
    lcd_set_cursor(1,0);
    lcd_string("18/11 CxJ");
    
    //subo plc-program --- revisar
    app();
    //xTaskCreatePinnedToCore(TaskScan,"TaskScan",4096,NULL,2,NULL,ARDUINO_RUNNING_CORE);

}























