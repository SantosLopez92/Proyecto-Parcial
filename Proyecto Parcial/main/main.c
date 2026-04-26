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
#define LED_R 				GPIO_NUM_19 //Led Rojo
#define LED_G 				GPIO_NUM_18 //Led Verde
#define LED_B 				GPIO_NUM_5  //Led Azul

//Configuracion botones
#define BTN_S 				GPIO_NUM_13//Boton seleccion de modo
#define BTN_R 				GPIO_NUM_14//Boton cambio de led/inicio de proceso automático
#define BTN_A 				GPIO_NUM_27//Boton resultado de color

int led_usado=0;
int center=3;
char lineChar[20];
SSD1306_t dev;

//Valores mÃ¡ximos de Rojo, Verde y Azul
int RM=1240;
int GM=1000;
int BM=1195;
	
float r=0;
float g=0;
float b=0;

//InicializaciÃƒÂ³n BTN
void in_btn(){
    gpio_reset_pin(BTN_S);
    gpio_set_direction(BTN_S,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_S,GPIO_PULLUP_ONLY);
    
    gpio_reset_pin(BTN_R);
    gpio_set_direction(BTN_R,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_R,GPIO_PULLUP_ONLY);

    gpio_reset_pin(BTN_A);
    gpio_set_direction(BTN_A,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_A,GPIO_PULLUP_ONLY);
}


//InicializaciÃƒÂ³n Adc
void in_adc(){
	
	adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
}

//InicializaciÃƒÂ³n Leds
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

//Cambio de led prendido
void borrado_leds(){

	gpio_set_level(LED_R, 0); 
	gpio_set_level(LED_G, 0); 
	gpio_set_level(LED_B, 0); 
	
}

void manual(){
			

	led_usado=0;
	
	vTaskDelay(pdMS_TO_TICKS(300));
	
	while (gpio_get_level(BTN_S)==1){
		int raw = adc1_get_raw(ADC_CHANNEL);
		if(gpio_get_level(BTN_R)==0){
			led_usado++;
			if (led_usado>3){
				led_usado=0;
			}

			borrado_leds();
			vTaskDelay(pdMS_TO_TICKS(300));
		}
		
		
		
		if(led_usado==0){
			borrado_leds();
			gpio_set_level(LED_R, 1);
			r=((float)raw/RM)*100;
			sprintf(lineChar, "Rojo : %.f ", r);	
		}
		if(led_usado==1){
			gpio_set_level(LED_G, 1); 
			g=((float)raw/GM)*100;
			sprintf(lineChar, "Verde : %.f ", g);
		}
		if(led_usado==2){
			gpio_set_level(LED_B, 1); 
			b=((float)raw/BM)*100;
			sprintf(lineChar, "Azul : %.f ", b);			
		}
		if(led_usado==3){
			sprintf(lineChar, "---Listo---");
		}
		ssd1306_clear_screen(&dev, false);
		ssd1306_contrast(&dev, 0xff);
		ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
        vTaskDelay(pdMS_TO_TICKS(500));
        
        
        
        if(gpio_get_level(BTN_A)==0){
			
        	if(r>80 && g>80 && b>80){
				sprintf(lineChar, "Blanco");
			} 
			else if(r>80 && g>60 && g<=80 && b>20 && b<=40){
				sprintf(lineChar, "Amarillo");
			}
			else if(r>80 && g>20 && g<=40 && b<=40){
				sprintf(lineChar, "Rojo");
			}
			else if(r>50 && r<=70 && g>70 && b>20 && b<=40){
				sprintf(lineChar, "Verde");
			}
        	else if(r>40 && r<=60 && g>80 && b>80){
				sprintf(lineChar, "Cian");
			}
        	else if(r>20 && r<=40 && g>20 && g<=40 && b>50 && b<=80){
				sprintf(lineChar, "Azul");
			}
        	else if(r<20 && g<20 && b<20){
				sprintf(lineChar, "Negro");
			}
			/*else if(r>30 && r<50 && g>20 && g<=40 && b>40 && b<=60){
				sprintf(lineChar, "Morado");
			}*/
			else{
				sprintf(lineChar, "Desconocido");
			}	
			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
        	vTaskDelay(pdMS_TO_TICKS(500));	
		}
		}
	return;
	}
		
		
		
		
		
		
		
void automatico(){
			int raw;
			int mediciones;
	
			vTaskDelay(pdMS_TO_TICKS(300));
	
			// Capturar valores y calcular el promedio

			// Led rojo encendido
			borrado_leds();
			gpio_set_level(LED_R, 1);

			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			sprintf(lineChar, "Estabilizando");
			ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
			vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos para estabilizaciÃ³n de valores
			
			mediciones = 0;
			for(int i=0; i<5; i++){
				raw = adc1_get_raw(ADC_CHANNEL);
				mediciones += raw;
				float valor = ((float)raw/RM)*100;

				ssd1306_clear_screen(&dev, false);
				ssd1306_contrast(&dev, 0xff);
				sprintf(lineChar, "Rojo: %.0f", valor);
				ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
				
				vTaskDelay(pdMS_TO_TICKS(1000));
			}
			r = ((float)(mediciones/5.0)/RM)*100;
			
			
			
			// Led verde encendido
			borrado_leds();
			gpio_set_level(LED_G, 1);

			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			sprintf(lineChar, "Estabilizando");
			ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
			vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos para estabilizaciÃ³n de valores
			
			mediciones = 0;
			for(int i=0; i<5; i++){
				raw = adc1_get_raw(ADC_CHANNEL);
				mediciones += raw;
				float valor = ((float)raw/GM)*100;

				ssd1306_clear_screen(&dev, false);
				ssd1306_contrast(&dev, 0xff);
				sprintf(lineChar, "Verde: %.0f", valor);
				ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
				
				vTaskDelay(pdMS_TO_TICKS(1000));
			}
			g = ((float)(mediciones/5.0)/GM)*100;
			
			
			
			
			// Led azul encendido
			borrado_leds();
			gpio_set_level(LED_B, 1);

			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			sprintf(lineChar, "Estabilizando");
			ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
			vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos para estabilizaciÃ³n de valores
			
			mediciones = 0;
			for(int i=0; i<5; i++){
				raw = adc1_get_raw(ADC_CHANNEL);
				mediciones += raw;
				float valor = ((float)raw/BM)*100;

				ssd1306_clear_screen(&dev, false);
				ssd1306_contrast(&dev, 0xff);
				sprintf(lineChar, "Azul: %.0f", valor);
				ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
				
				vTaskDelay(pdMS_TO_TICKS(1000));
			}
			b = ((float)(mediciones/5.0)/BM)*100;
			borrado_leds();

			// Resultados
        	if(r>80 && g>80 && b>80){
				sprintf(lineChar, "Blanco");
			} 
			else if(r>80 && g>60 && g<=80 && b>20 && b<=40){
				sprintf(lineChar, "Amarillo");
			}
			else if(r>80 && g>20 && g<=40 && b<=40){
				sprintf(lineChar, "Rojo");
			}
			else if(r>50 && r<=70 && g>70 && b>20 && b<=40){
				sprintf(lineChar, "Verde");
			}
        	else if(r>40 && r<=60 && g>80 && b>80){
				sprintf(lineChar, "Cian");
			}
        	else if(r>20 && r<=40 && g>20 && g<=40 && b>50 && b<=80){
				sprintf(lineChar, "Azul");
			}
        	else if(r<20 && g<20 && b<20){
				sprintf(lineChar, "Negro");
			}
			/*else if(r>30 && r<50 && g>20 && g<=40 && b>40 && b<=60){
				sprintf(lineChar, "Morado");
			}*/
			else{
				sprintf(lineChar, "Desconocido");
			}	
			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
        	vTaskDelay(pdMS_TO_TICKS(500));	
        	while (gpio_get_level(BTN_S)==1){
        		vTaskDelay(pdMS_TO_TICKS(200));	
				}
			return;
		}

void app_main(void)
{
	//cConfiguracion OLED

	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
	ssd1306_init(&dev, 128, 64);
	vTaskDelay(pdMS_TO_TICKS(300));
	
	//Inicializacion de leds, adc y btns
	in_leds();
	in_adc();
	in_btn();

    
    //Reinicio de variable
    borrado_leds();
    int seleccion=0;
    // Bucle de trabajo
    while (1) {
        if(gpio_get_level(BTN_S)==0){
			seleccion++;
			if (seleccion>1){
				seleccion=0;
			}

			vTaskDelay(pdMS_TO_TICKS(300));
		}
        
		if(seleccion==0){
			sprintf(lineChar, "Automatico");
		}
		if(seleccion==1){
			sprintf(lineChar, "Manual");
		}
		ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
        	vTaskDelay(pdMS_TO_TICKS(100));	
		
		 if(gpio_get_level(BTN_R)==0 && seleccion==0){
		 	automatico();
		 }
		 if(gpio_get_level(BTN_R)==0 && seleccion==1){
		 	manual();
		 }

    }
} 
