#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#include "main.h"

/* 瀹氫箟 DHT11_DATA 杩炴帴鐨凣PIO绔彛锛岀敤鎴峰彧闇€瑕佷慨鏀逛笅闈㈢殑浠ｇ爜鍗冲彲鏀瑰彉鎺у埗鐨?DHT11_DATA 寮曡剼 */

// DHT11_DATA
#define DHT11_DATA_Pin 				GPIO_PIN_12                   /* GPIO绔彛 */
#define DHT11_DATA_GPIO_Port 	GPIOB              						/* 瀵瑰簲寮曡剼 */


/* DHT11鏁版嵁缁撴瀯浣?*/
typedef struct
{                            
   uint8_t humi_int;        // 婀垮害鏁存暟閮ㄥ垎
   uint8_t humi_deci;       // 婀垮害灏忔暟閮ㄥ垎
   uint8_t temp_int;        // 娓╁害鏁存暟閮ㄥ垎
   uint8_t temp_deci;       // 娓╁害灏忔暟閮ㄥ垎
   uint8_t check_sum;       // 鏍￠獙鍜?                             
} DHT11_DATA_TYPEDEF;


/* 鍑芥暟澹版槑 */
void DHT11_GPIO_Config(void);
void DHT11_SetGPIOMode(uint32_t mode, uint32_t pull);
uint8_t DHT11_ReadByte(void);
HAL_StatusTypeDef DHT11_ReadData(DHT11_DATA_TYPEDEF *dht11_data);


#endif /* __BSP_DHT11_H */
