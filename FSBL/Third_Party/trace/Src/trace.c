#include "trace.h"

// 1. Core Debug 寄存器
#define DEMCR             (*(volatile uint32_t *)0xE000EDFC)
#define TRCENA_BIT        (1 << 24)

// 2. TPIU (Trace Port Interface Unit) - 负责将数据输出到引脚
#define TPIU_BASE         0xE0040000
#define TPIU_CSPSR        (*(volatile uint32_t *)(TPIU_BASE + 0x004)) // Current Port Size
// #define TPIU_SPPR         (*(volatile uint32_t *)(TPIU_BASE + 0x0F0)) // Selected Pin Protocol
#define TPIU_FFCR         (*(volatile uint32_t *)(TPIU_BASE + 0x304)) // Formatter and Flush Control
#define TPIU_LAR          (*(volatile uint32_t *)(TPIU_BASE + 0xFB0)) // Lock Access

// 3. ETM (Embedded Trace Macrocell) - 负责生成指令流数据
#define ETM_BASE          0xE0041000
#define ETM_TRCPRGCTLR    (*(volatile uint32_t *)(ETM_BASE + 0x004))
#define ETM_TRCSTATR      (*(volatile uint32_t *)(ETM_BASE + 0x00C))
#define ETM_TRCCONFIGR    (*(volatile uint32_t *)(ETM_BASE + 0x010))
#define ETM_TRCEVENTCTL1R (*(volatile uint32_t *)(ETM_BASE + 0x024))
#define ETM_TRCVICTLR     (*(volatile uint32_t *)(ETM_BASE + 0x080)) // ViewInst Main Control
#define ETM_TRCTRACEIDR   (*(volatile uint32_t *)(ETM_BASE + 0x040))
// #define ETM_TRCLAR        (*(volatile uint32_t *)(ETM_BASE + 0xFB0))

// CoreSight 解锁魔数
#define CORESIGHT_UNLOCK  0xC5ACCE55
/*-------------------------------------------------------------------
** Define the function to enable the trace port
**-----------------------------------------------------------------*/
void Trace_Init_CortexM55(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Enable GPIOD Clock */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /* Enable GPIOB Clock */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* Configure pins: PD2, PB0, PB6, PB7, PB3 */
    /* PB3: TRACECLK, PD2: TRACED0, PB0: TRACED1, PB6: TRACED2, PB7: TRACED3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct = (GPIO_InitTypeDef){0};
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    LL_DBGMCU_EnableTPIUExportClock();

	 // 全局启用 Trace 组件
	 DEMCR |= TRCENA_BIT;

    // 配置 TPIU
//     TPIU_LAR = CORESIGHT_UNLOCK; // 解锁 TPIU
    // 设置协议
//     TPIU_SPPR = 0x00000002; 

    // 设置端口位宽: 1<<3 = 8 (表示 4-bit 模式)
//    TPIU_CSPSR = 0x00000008;

    // 格式化器配置
//     TPIU_FFCR = 0x00000100;

     // 进入ETM编程模式
     ETM_TRCPRGCTLR = 0;
     // 等待 ETM 进入空闲状态
     while ((ETM_TRCSTATR & 0x1) == 0);

     // 清除配置寄存器，防止之前的残留配置影响
     ETM_TRCCONFIGR = 0;

     // 设置 Trace ID
     ETM_TRCTRACEIDR = 0x10;
    
     // 设置事件选择器: Resource 1 通常硬连线为 "True/Always"
     ETM_TRCEVENTCTL1R = 0x00000000;
    
     // ViewInst Control: 启用指令追踪
     ETM_TRCVICTLR = 0x00000200;

     // 启用 ETM
     ETM_TRCPRGCTLR = 1; // Enable ETM
}
