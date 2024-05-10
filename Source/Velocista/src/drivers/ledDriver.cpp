#include "drivers/ledDriver.h"

#include <driver/rmt_tx.h>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <string.h>

static const char *TAG = "LedDriver";

ledDriver::ledDriver(gpio_num_t gpioNum, uint32_t numOfLeds) : gpioPin(gpioNum), numberOfLeds(numOfLeds)
{
    led_encoder = NULL;
    led_chan = NULL;
    memset(ledGBRs, 0, sizeof(ledGBRs));
}

void ledDriver::init()
{
    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = gpioPin,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .resolution_hz = 10000000,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .trans_queue_depth = 4,  // set the number of transactions that can be pending in the background
    };

    rmt_new_tx_channel(&tx_chan_config, &led_chan);

    ESP_LOGI(TAG, "Install led strip encoder");
    led_strip_encoder_config_t encoder_config = {
        .resolution = 10000000,
    };
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(led_chan));
}

void ledDriver::setColor(uint32_t ledIndex, uint8_t r, uint8_t g, uint8_t b)
{

    ledGBRs[ledIndex * 3] = g;
    ledGBRs[ledIndex * 3 + 1] = r;
    ledGBRs[ledIndex * 3 + 2] = b;
}

void ledDriver::update()
{
    rmt_transmit_config_t tx_config = {
        .loop_count = 0, // no transfer loop
    };
    rmt_transmit(led_chan, led_encoder, ledGBRs, 3 * numberOfLeds, &tx_config);
    rmt_tx_wait_all_done(led_chan, portMAX_DELAY);
}