
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"

adc_oneshot_unit_handle_t adc_handle;// handle del adc
adc_oneshot_chan_cfg_t config_channel = {
  .bitwidth = ADC_BITWIDTH_DEFAULT,
  .atten = ADC_ATTEN_DB_12,
}; // creo el struct para la config de los canales
  

//pin inputs
#define PIN_I01 36
#define PIN_I02 39
#define PIN_I03 34
#define PIN_I04 35
#define PIN_I05 32
#define PIN_I06 33

int list_pins[6]= {PIN_I01,PIN_I02,PIN_I03,PIN_I04,PIN_I05,PIN_I06};
adc_channel_t list_channels[6];

//pin outpus
#define PIN_Q01 19
#define PIN_Q02 18
#define PIN_Q03 5
#define PIN_Q04 17
//estate vars for every pin
uint8_t LD_I1 = 0;
uint8_t LD_I2 = 0;
uint8_t LD_I3 = 0;
uint8_t LD_I4 = 0;
uint8_t LD_I5 = 0;
uint8_t LD_I6 = 0;

uint8_t LD_Q1 = 0;
uint8_t LD_Q2 = 0;
uint8_t LD_Q3 = 0;
uint8_t LD_Q4 = 0;

union {
  uint32_t p[2];
  uint64_t v;
} LD_TIME;

uint64_t getTime(){
  return LD_TIME.v;
}

int32_t LD_MI01 = 0;


void refreshTime64bit(){
unsigned long now = esp_timer_get_time()/1000;  if(now < LD_TIME.p[0]){
    LD_TIME.p[1]++;
  }
  LD_TIME.p[0] = now;
}

void readInputs(){
  //pin 33 --> input/6
  adc_oneshot_read(adc_handle, list_channels[5], &LD_I6);vTaskDelay(1);
  //pin 32 --> input/5
  adc_oneshot_read(adc_handle, list_channels[4], &LD_I5);vTaskDelay(1);
  //pin 35 --> input/4
  adc_oneshot_read(adc_handle, list_channels[3], &LD_I4);vTaskDelay(1);
  //pin 34 --> input/3
  adc_oneshot_read(adc_handle, list_channels[2], &LD_I3);vTaskDelay(1);
  //pin 39 --> input/2
  adc_oneshot_read(adc_handle, list_channels[1], &LD_I2);vTaskDelay(1);
  //pin 36 --> input/1
  adc_oneshot_read(adc_handle, list_channels[0], &LD_I1);vTaskDelay(1);

}

void writeOutputs(){

  gpio_set_level(PIN_Q01,LD_Q1);
  gpio_set_level(PIN_Q02,LD_Q2);
  gpio_set_level(PIN_Q03,LD_Q3);
  gpio_set_level(PIN_Q04,LD_Q4);

}

void rung001(void){
  uint8_t _LD_S0;
  _LD_S0 = 1;
  if(LD_I1){
    _LD_S0 = 0;
  }
  if(_LD_S0){
    LD_MI01 = 1 + LD_MI01;
  }
}

void rung002(void){
  uint8_t _LD_S0;
  _LD_S0 = 1;
  if(_LD_S0){
    if(LD_MI01 != 1){
      _LD_S0 = 0;
    }
  }
  if(_LD_S0){
    LD_Q2 = 1;
  }
}

void rung003(void){
  uint8_t _LD_S0;
  _LD_S0 = 1;
  if(_LD_S0){
    if(LD_MI01 != 2){
      _LD_S0 = 0;
    }
  }
  if(_LD_S0){
    LD_Q2 = 0;
  }
  if(_LD_S0){
    LD_MI01 = 0;
  }
}

void rung004(void){
  uint8_t _LD_S0;
  _LD_S0 = 1;
}

void rung005(void){
  uint8_t _LD_S0;
  _LD_S0 = 1;
  if(_LD_S0){
    if(LD_I5 <= 150){
      _LD_S0 = 0;
    }
  }
  LD_Q1 = _LD_S0;
}

void rung006(void){
  uint8_t _LD_S0;
  _LD_S0 = 1;
  if(_LD_S0){
    if(LD_I6 <= 150){
      _LD_S0 = 0;
    }
  }
  LD_Q3 = _LD_S0;
}

void initContext(void){
}

void init(){
	LD_TIME.v = 0;
    refreshTime64bit();
   //INPUTS(with adc)-- inicializacion
  //adc_oneshot_unit_handle_t adc_handle;// handle del adc
  adc_oneshot_unit_init_cfg_t config_adc; //struct de config de canales
  config_adc.unit_id=ADC_UNIT_1;
  config_adc.ulp_mode= ADC_ULP_MODE_DISABLE;
  config_adc.clk_src= 0;

  adc_oneshot_new_unit(&config_adc, &adc_handle);//creo la unidad con el handle y config
  /*
  adc_oneshot_chan_cfg_t config_channel; // creo el struct para la config de los canales
  config_channel.bitwidth = ADC_BITWIDTH_DEFAULT;
  config_channel.atten = ADC_ATTEN_DB_12;
  */
  

  //adc_oneshot_config_channel(adc_handle, EXAMPLE_ADC1_CHAN0, config_adc);
  adc_channel_t c;adc_unit_t u;
  for(int in_pin =0;in_pin<6;in_pin++){
    
    adc_oneshot_io_to_channel(list_pins[in_pin],&u,&c);
    adc_oneshot_config_channel(adc_handle, c, &config_channel);
    //agrego cada channel al arrray
    list_channels[in_pin] = c;
    //muestro el canal asginado para cada pin.
    ESP_LOGI("ADC","pin: %d >> channel: %d",list_pins[in_pin],c);
  
  }
    /*
    pinMode(PIN_I01, INPUT);
    pinMode(PIN_I02, INPUT);
    pinMode(PIN_Q01, OUTPUT);
    */
    //OUTPUTS
    gpio_reset_pin(PIN_Q01);gpio_reset_pin(PIN_Q02);gpio_reset_pin(PIN_Q03);gpio_reset_pin(PIN_Q04);
    gpio_set_direction(PIN_Q01,GPIO_MODE_OUTPUT);gpio_set_direction(PIN_Q02,GPIO_MODE_OUTPUT);gpio_set_direction(PIN_Q03,GPIO_MODE_OUTPUT);gpio_set_direction(PIN_Q04,GPIO_MODE_OUTPUT);
}


void app(){
   init();
    initContext();
  for(;;){
    vTaskDelay(1);
    readInputs();
    refreshTime64bit();
    rung001();
    rung002();
    rung003();
    rung004();
    rung005();
    rung006();
    writeOutputs();
  }
}


