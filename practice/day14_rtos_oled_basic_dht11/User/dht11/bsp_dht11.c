
#include "bsp_dht11.h"
#include "bsp_dwt.h"


/**
 * @brief  DHT11 DATA 寮曡剼 IO 鍒濆鍖?
 * @retval 鏃?
 */
void DHT11_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 浣胯兘GPIOB绔彛鏃堕挓 */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* 璁剧疆寮曡剼涓烘帹鎸借緭鍑?*/
    GPIO_InitStruct.Pin = DHT11_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // 鎺ㄦ尳杈撳嚭
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // 涓嶄笂鎷変笉涓嬫媺
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // 浣庨€熻緭鍑?
    HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);

    /* 鍒濆鍖朌HT11鏁版嵁寮曡剼 */
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET);
}


/**
 * @brief  閰嶇疆 DHT11 鏁版嵁寮曡剼鐨勫伐浣滄ā寮?
 * @param  mode 寮曡剼妯″紡锛屽杈撳叆銆佽緭鍑虹瓑锛屼娇鐢?HAL 搴撲腑鐨?GPIO_MODE_XXX 瀹?
 * @param  pull 涓婁笅鎷夐厤缃紝浣跨敤 HAL 搴撲腑鐨?GPIO_PULL_XXX 瀹?
 */
void DHT11_SetGPIOMode(uint32_t mode, uint32_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_DATA_Pin;        	// 閫夋嫨DHT11鏁版嵁寮曡剼
    GPIO_InitStruct.Mode = mode;                  // 璁剧疆寮曡剼妯″紡锛堣緭鍏ャ€佽緭鍑虹瓑锛?
    GPIO_InitStruct.Pull = pull;                  // 璁剧疆涓婃媺鎴栦笅鎷?
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; 	// 璁剧疆寮曡剼閫熷害

    HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);  // 鍒濆鍖朑PIO
}



/**
 * @brief  璇诲彇DHT11涓€涓瓧鑺傛暟鎹?
 * @retval 杩斿洖8浣嶆暟鎹紙1瀛楄妭锛?
 */
uint8_t DHT11_ReadByte(void)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        // 绛夊緟淇″彿绾垮彉楂橈紝琛ㄧず寮€濮嬩紶杈撶i浣嶆暟鎹?
        while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_RESET);

        DWT_DelayUs(40);  // 寤舵椂40寰锛屽垽鏂暟鎹綅鏄?杩樻槸1

        // 濡傛灉40us鍚庣嚎浠嶇劧涓洪珮锛岃〃绀鸿浣嶄负1
        if (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET)
        {
            value |= (1 << (7 - i));  // 鎶婂搴斾綅璁剧疆涓?

            // 绛夊緟淇″彿绾垮彉浣庯紝鍑嗗鎺ユ敹涓嬩竴浣?
            while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET);
        }
        // 濡傛灉40us鍚庣嚎涓轰綆锛屽垯璇ヤ綅涓?锛岀洿鎺ョ户缁笅涓€浣?
    }

    return value;
}


/**
 * @brief  璇诲彇DHT11浼犳劅鍣ㄦ暟鎹?
 * @param  data 鎸囧悜瀛樻斁娓╂箍搴︽暟鎹殑缁撴瀯浣撴寚閽?
 * @retval HAL_OK 鎴愬姛锛孒AL_ERROR 澶辫触
 */
HAL_StatusTypeDef DHT11_ReadData(DHT11_DATA_TYPEDEF *data)
{
    uint8_t retry = 0;

    // 1. 涓绘満鎷変綆鎬荤嚎锛屽彂閫佽捣濮嬩俊鍙凤紙鑷冲皯18ms锛?
    DHT11_SetGPIOMode(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);     // 璁剧疆涓烘帹鎸借緭鍑?
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_RESET);
    DWT_DelayMs(20);  // 淇濇寔浣庣數骞?0ms锛岄€氱煡DHT11寮€濮嬩紶杈?
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET);
    DWT_DelayUs(30); 	// 寤舵椂30寰

    // 2. 璁剧疆寮曡剼涓鸿緭鍏ワ紝绛夊緟DHT11鍝嶅簲淇″彿
    DHT11_SetGPIOMode(GPIO_MODE_INPUT, GPIO_PULLUP);

    // 3. 绛夊緟DHT11鎷変綆鍝嶅簲淇″彿锛堟渶澶х瓑寰?00us锛?
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET)
    {
        if (++retry > 100)
            return HAL_ERROR;    // 瓒呮椂鏃犲搷搴旓紝璇诲彇澶辫触
        DWT_DelayUs(1);
    }

    // 绛夊緟DHT11鎷夐珮淇″彿锛堟渶澶х瓑寰?00us锛?
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_RESET)
    {
        if (++retry > 100)
            return HAL_ERROR;    // 瓒呮椂锛岃鍙栧け璐?
        DWT_DelayUs(1);
    }

    // 绛夊緟DHT11鍐嶆鎷変綆淇″彿锛堟渶澶х瓑寰?00us锛?
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET)
    {
        if (++retry > 100)
            return HAL_ERROR;    // 瓒呮椂锛岃鍙栧け璐?
        DWT_DelayUs(1);
    }

    // 4. 璇诲彇5瀛楄妭鏁版嵁锛堟箍搴︽暣鏁般€佸皬鏁帮紝娓╁害鏁存暟銆佸皬鏁帮紝鏍￠獙鍜岋級
    data->humi_int   = DHT11_ReadByte();
    data->humi_deci  = DHT11_ReadByte();
    data->temp_int   = DHT11_ReadByte();
    data->temp_deci  = DHT11_ReadByte();
    data->check_sum  = DHT11_ReadByte();

    // 5. 鏍￠獙鏁版嵁鏄惁姝ｇ‘
    uint8_t sum = data->humi_int + data->humi_deci + data->temp_int + data->temp_deci;
    return (sum == data->check_sum) ? HAL_OK : HAL_ERROR;
}


/*********************************************END OF FILE**********************/
