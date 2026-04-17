#ifndef __BSP_MAX30102_H
#define __BSP_MAX30102_H


#include "main.h"

#define MAX30102_INT_Pin  				GPIO_PIN_8
#define MAX30102_INT_GPIO_Port	 	GPIOB

#define MAX30102_SLAVE_ADDR        (0x57)       //7Œªµÿ÷∑
#define MAX30102_WRITE_ADDR    (MAX30102_SLAVE_ADDR << 1)         	// 0xAE£¨–¥≤Ÿ◊˜
#define MAX30102_READ_ADDR     ((MAX30102_SLAVE_ADDR << 1) | 0x01)  // 0xAF£¨∂¡≤Ÿ◊˜



/* MAX30102ºƒ¥Ê∆˜µÿ÷∑º∞œ‡πÿ∫Í∂®“Â */
#define INTERRUPT_STATUS1           0X00    // ÷–∂œ◊¥Ã¨ºƒ¥Ê∆˜1µÿ÷∑
#define INTERRUPT_STATUS2           0X01    // ÷–∂œ◊¥Ã¨ºƒ¥Ê∆˜2µÿ÷∑
#define INTERRUPT_ENABLE1           0X02    // ÷–∂œ πƒ‹ºƒ¥Ê∆˜1µÿ÷∑
#define INTERRUPT_ENABLE2           0X03    // ÷–∂œ πƒ‹ºƒ¥Ê∆˜2µÿ÷∑

#define FIFO_WR_POINTER             0X04    // FIFO–¥÷∏’Îºƒ¥Ê∆˜
#define FIFO_OV_COUNTER             0X05    // FIFO“Á≥ˆº∆ ˝ºƒ¥Ê∆˜
#define FIFO_RD_POINTER             0X06    // FIFO∂¡÷∏’Îºƒ¥Ê∆˜
#define FIFO_DATA                   0X07    // FIFO ˝æ›ºƒ¥Ê∆˜

#define FIFO_CONFIGURATION          0X08    // FIFO≈‰÷√ºƒ¥Ê∆˜
#define MODE_CONFIGURATION          0X09    // ƒ£ Ω≈‰÷√ºƒ¥Ê∆˜
#define SPO2_CONFIGURATION          0X0A    // —™—ı≈‰÷√ºƒ¥Ê∆˜
#define LED1_PULSE_AMPLITUDE        0X0C    // LED1¬ˆ≥Â∑˘∂»…Ë÷√ºƒ¥Ê∆˜
#define LED2_PULSE_AMPLITUDE        0X0D    // LED2¬ˆ≥Â∑˘∂»…Ë÷√ºƒ¥Ê∆˜

#define MULTILED1_MODE             0X11    // ∂‡LEDƒ£ Ω1ºƒ¥Ê∆˜
#define MULTILED2_MODE             0X12    // ∂‡LEDƒ£ Ω2ºƒ¥Ê∆˜

#define TEMPERATURE_INTEGER         0X1F    // Œ¬∂»’˚ ˝ºƒ¥Ê∆˜
#define TEMPERATURE_FRACTION        0X20    // Œ¬∂»–° ˝ºƒ¥Ê∆˜
#define TEMPERATURE_CONFIG          0X21    // Œ¬∂»≈‰÷√ºƒ¥Ê∆˜

#define VERSION_ID                 0XFE    // ∞Ê±æ∫≈ºƒ¥Ê∆˜
#define PART_ID                    0XFF    // –æ∆¨IDºƒ¥Ê∆˜

extern uint8_t max30102_int_flag;

/*********** ∫Ø ˝…˘√˜ ***********/
void MAX30102_INT_Init(void);
ErrorStatus MAX30102_CheckDevice(void);
ErrorStatus MAX30102_WriteByte(uint8_t reg, uint8_t data);
ErrorStatus MAX30102_WriteBuffer(uint8_t reg, uint8_t *buffer, uint16_t len);
ErrorStatus MAX30102_ReadBuffer( uint8_t addr, uint8_t *rbuffer, uint16_t len);
void MAX30102_Init(void);
void MAX30102_fifo_read(float *output_data);
uint16_t MAX30102_getHeartRate(float *input_data, uint16_t cache_nums);
float MAX30102_getSpO2(float *ir_input_data, float *red_input_data, uint16_t cache_nums);


#endif
