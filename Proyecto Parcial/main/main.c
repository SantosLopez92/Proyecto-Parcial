#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"


//Configuraciones para ADC
#define ADC_CHANNEL    ADC1_CHANNEL_4   // GPIO32
#define ADC_ATTEN      ADC_ATTEN_DB_11  
#define ADC_WIDTH      ADC_WIDTH_BIT_12 // Resolucion de 12 bits

//Configuraciones para OLED
#define CONFIG_SDA_GPIO		GPIO_NUM_21
#define CONFIG_SCL_GPIO		GPIO_NUM_22
#define CONFIG_RESET_GPIO	-1
#define CONFIG_SSD1306_128x64
#define tag "SSD1306"

//Configuraciones para Leds
#define LED_R 				GPIO_NUM_17 
#define LED_G 				GPIO_NUM_5
#define LED_B 				GPIO_NUM_18

//Configuracion botones
#define BTN_S 				GPIO_NUM_13//Boton siguiente Led
#define BTN_R 				GPIO_NUM_14//Boton resultado

int led_usado=0;




//Inicialización BTN
void in_btn(){
    gpio_reset_pin(BTN_S);
    gpio_set_direction(BTN_S,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_S,GPIO_PULLUP_ONLY);
    
    gpio_reset_pin(BTN_R);
    gpio_set_direction(BTN_R,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_R,GPIO_PULLUP_ONLY);
}


//Inicialización Adc
void in_adc(){
	
	adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
}

//Inicialización Leds
void in_leds(){

    gpio_reset_pin(LED_R);
    gpio_set_direction(LED_R,GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(LED_R,GPIO_FLOATING);
    gpio_set_level(LED_R, 0);
        
    gpio_reset_pin(LED_G);
    gpio_set_direction(LED_G,GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(LED_G,GPIO_FLOATING);
	gpio_set_level(LED_G, 0);
	
    gpio_reset_pin(LED_B);
    gpio_set_direction(LED_B,GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(LED_B,GPIO_FLOATING);
    gpio_set_level(LED_B, 0);  

}

void control_leds(){
	gpio_set_level(LED_R, 0); 
	gpio_set_level(LED_G, 0); 
	gpio_set_level(LED_B, 0); 
	
	if(led_usado==0){
	gpio_set_level(LED_R, 1);
	}
	if(led_usado==1){
	gpio_set_level(LED_G, 1); 
	}
	if(led_usado==2){
	gpio_set_level(LED_B, 1); 
	}
}

void app_main(void)
{
	//Ajuste Oled
	int center;
	char lineChar[20];	
	SSD1306_t dev;
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
	ssd1306_init(&dev, 128, 64);
	center = 3;
	in_leds();
	in_adc();
	in_btn();
	
	
    // Bucle de lectura
    while (1) {
        int raw = adc1_get_raw(ADC_CHANNEL);
		
		if(gpio_get_level(BTN_S)==0){
			
			led_usado++;
			if (led_usado>2){
				led_usado=0;
			}
			control_leds();
		}
		
		// Mostrar resultados
		sprintf(lineChar, "Salida : %.d ", raw);
		ssd1306_clear_screen(&dev, false);
		ssd1306_contrast(&dev, 0xff);
		ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
