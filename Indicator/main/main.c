/**
 * @file main.c
 * @brief advanced example entry point
 * @version 0.0.1
 * @date 2023-08-09
 * @note
 * @attention
 * @author @Love4yzp
 */

#include "bsp_board.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "frame.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_port.h"
#include "radio.h"
#include "sen5x.h"
#include "ui.h"
static const char *TAG = "app_main";

#define VERSION  "v0.1.0"

#define SENSECAP "\n\
   _____                      _________    ____         \n\
  / ___/___  ____  ________  / ____/   |  / __ \\       \n\
  \\__ \\/ _ \\/ __ \\/ ___/ _ \\/ /   / /| | / /_/ /   \n\
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/         \n\
/____/\\___/_/ /_/____/\\___/\\____/_/  |_/_/           \n\
--------------------------------------------------------\n\
 Version: %s %s %s\n\
--------------------------------------------------------\n\
"

ESP_EVENT_DEFINE_BASE( VIEW_EVENT_BASE );
esp_event_loop_handle_t view_event_handle;

#define RF_FREQUENCY               868000000 // Hz
#define LORA_BANDWIDTH             0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR      12        // [SF7..SF12]
#define LORA_CODINGRATE            1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH       15        // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT        5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON       false

static RadioEvents_t RadioEvents;


enum {
    VIEW_EVENT_TOPICS_SEN5x = 0, // uint8_t, enum start_screen, which screen when start
    VIEW_EVENT_ALL,
};

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    SEN5xData_t sen5x_data;

    ESP_LOGI( TAG, "rssi:%d dBm, snr:%d dB, len:%d, payload:", rssi, snr, size );

    for ( int i = 0; i < size; i++ ) {
        printf( "0x%x ", payload[i] );
    }
    printf( "\n" );

    Frame_t *frame = parsePayload( payload, size );
    if ( frame == NULL ) {
        ESP_LOGE( TAG, "parsePayload error" );
        return;
    }
    ESP_LOGI( TAG, "frame->type: %s", dataIDToString( frame->topic ) );

    switch ( frame->topic ) {
        case TOPICS_SEN5x:
            phraseSEN5xData( frame->data, &sen5x_data );
            // 4. Post events to the loop. This queues the event on the event loop. At some point, the event loop executes the event handler registered to the posted event, in this case, run_on_event. To simplify the process, this example calls esp_event_post_to from app_main, but posting can be done from any other task (which is the more interesting use case).
            esp_event_post_to( view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TOPICS_SEN5x, &sen5x_data, sizeof( sen5x_data ), portMAX_DELAY ); // prinSEN5xData( &sen5x_data );
            break;

        default:
            break;
    }

    deleteFrame( frame );
}

// 1. Define an event handler function.
static void __view_event_handler( void *handler_args, esp_event_base_t base, int32_t id, void *event_data );

// #define USING_LVGL // if you want to use lvgl, you need to define USING_LVGL

void app_main( void )
{
    ESP_LOGI( "", SENSECAP, VERSION, __DATE__, __TIME__ );

    ESP_ERROR_CHECK( bsp_board_init() );

    ESP_LOGI( TAG, "APP MAIN START" );

    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );

    Radio.SetChannel( RF_FREQUENCY );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
    Radio.SetMaxPayloadLength( MODEM_LORA, 255 );

    Radio.Rx( 0 ); // Continuous Rx

    // 2. A configuration structure of type esp_event_loop_args_t is needed to specify the properties of the loop to be created.  \
    A handle of type esp_event_loop_handle_t is obtained, \
    which is needed by the other APIs to reference the loop to perform their operations.
    esp_event_loop_args_t data_show_task_args = {
        .queue_size      = 10,
        .task_name       = "view_event_task",
        .task_priority   = uxTaskPriorityGet( NULL ),
        .task_stack_size = 10240,
        .task_core_id    = tskNO_AFFINITY };

    ESP_ERROR_CHECK( esp_event_loop_create( &data_show_task_args, &view_event_handle ) );

    // 3. Register event handler defined in (1). MY_EVENT_BASE and MY_EVENT_ID specify a hypothetical event that handler run_on_event should execute when it gets posted to the loop.
    for ( int i = 0; i < VIEW_EVENT_ALL; i++ ) {
        ESP_ERROR_CHECK( esp_event_handler_instance_register_with( view_event_handle,
                                                                   VIEW_EVENT_BASE, i,
                                                                   __view_event_handler, NULL, NULL ) );
    }
#ifdef USING_LVGL
    /*Screen setup*/
    lv_port_init();
    ui_init();
#endif
    while ( 1 ) {
        vTaskDelay( pdMS_TO_TICKS( 10000 ) );
    }
}

static void __view_event_handler( void *handler_args, esp_event_base_t base, int32_t id, void *event_data )
{
    lv_port_sem_take();
    switch ( id ) {
        case VIEW_EVENT_TOPICS_SEN5x:
            SEN5xData_t *view_sen5x_data = (SEN5xData_t *)event_data;
            prinSEN5xData( view_sen5x_data );

#ifdef USING_LVGL
            char  data_buf[10];
            float pm25 = view_sen5x_data->massConcentrationPm2p5 / 10.0f;
            float voc  = view_sen5x_data->vocIndex / 10.0f;
            float humi = view_sen5x_data->ambientHumidity / 100.0f;
            float temp = view_sen5x_data->ambientTemperature / 200.0f;

            snprintf( data_buf, sizeof( data_buf ), "%.2f", pm25 );
            lv_label_set_text( ui_sensor_data_pm25, data_buf );

            snprintf( data_buf, sizeof( data_buf ), "%.f", voc );
            lv_label_set_text( ui_sensor_data_voc, data_buf );

            // // change the value of ui_Arc_humi
            snprintf( data_buf, sizeof( data_buf ), "%.2f", humi );
            lv_label_set_text( ui_sensor_data_humi, data_buf );

            // // ui_Slider_temp
            lv_slider_set_value( ui_Slider_temp, temp, LV_ANIM_OFF );
#endif
            break;
    }
    lv_port_sem_give();
}
