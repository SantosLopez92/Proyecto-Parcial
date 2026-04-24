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

#define ADC_CHANNEL    ADC1_CHANNEL_4   // GPIO32
#define ADC_ATTEN      ADC_ATTEN_DB_11  // 
#define ADC_WIDTH      ADC_WIDTH_BIT_12 // Resolucion de 12 bits



#define CONFIG_SDA_GPIO		GPIO_NUM_21
#define CONFIG_SCL_GPIO		GPIO_NUM_22
#define CONFIG_RESET_GPIO	-1

#define CONFIG_SSD1306_128x64

#define tag "SSD1306"


void app_main(void)
{
	int center;
	char lineChar[20];
	
	SSD1306_t dev;
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
	ssd1306_init(&dev, 128, 64);
	

	center = 3;
	
	
    // Configurar ADC
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);

    // Bucle de lectura
    while (1) {
        int raw = adc1_get_raw(ADC_CHANNEL);

        // Convertir a voltaje (aprox)
        float voltage = (raw / 4095.0) * 3.3;
		
		
		
		sprintf(lineChar, "Voltaje: %.d F", raw);
        // Mostrar resultados
		ssd1306_clear_screen(&dev, false);
		ssd1306_contrast(&dev, 0xff);
		ssd1306_display_text(&dev, center, lineChar, strlen(lineChar), false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
