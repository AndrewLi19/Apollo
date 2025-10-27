#include "unity_fixture.h"
#include "mt25ql512abb.h"
#include <stdio.h> // for snprintf

#define TEST_4BYTES_ADDR
#define TEST_MT25QU02_ERASE_WRITE_SIZE 64

/* 辅助函数：从 flash 读取并与期望缓冲区比较 */
static void verify_memory_equals(uint32_t addr, const uint8_t *expected, uint32_t len)
{
	uint8_t buf[len];
    for(uint32_t i = 0; i < len; i++) buf[i] = 0;
	int32_t status = MT25QU02_ReadSTR(buf, addr, len);
	if (status != MT25QU02_OK) {
		char msg[128];
		snprintf(msg, sizeof(msg), "MT25QU02_ReadSTR failed at 0x%06X len %u: status %ld", (unsigned)addr, (unsigned)len, (long)status);
		TEST_FAIL_MESSAGE(msg);
		return;
	}
	for (uint32_t i = 0; i < len; i++) {
		if (buf[i] != expected[i]) {
			char msg[128];
			snprintf(msg, sizeof(msg), "Data mismatch at 0x%06X+%u: expected 0x%02X got 0x%02X", (unsigned)addr, (unsigned)i, expected[i], buf[i]);
			TEST_FAIL_MESSAGE(msg);
			return;
		}
	}
}

/* 辅助函数：从 flash 读取并与某个填充值比较（例如 0xFF） */
static void verify_memory_filled(uint32_t addr, uint8_t value, uint32_t len)
{
	uint8_t buf[len];
    for(uint32_t i = 0; i < len; i++) buf[i] = 0;
	int32_t status = MT25QU02_ReadSTR(buf, addr, len);
	if (status != MT25QU02_OK) {
		char msg[128];
		snprintf(msg, sizeof(msg), "MT25QU02_ReadSTR failed at 0x%06X len %u: status %ld", (unsigned)addr, (unsigned)len, (long)status);
		TEST_FAIL_MESSAGE(msg);
		return;
	}
	for (uint32_t i = 0; i < len; i++) {
		if (buf[i] != value) {
			char msg[128];
			snprintf(msg, sizeof(msg), "Unexpected value at 0x%06X+%u: expected 0x%02X got 0x%02X", (unsigned)addr, (unsigned)i, value, buf[i]);
			TEST_FAIL_MESSAGE(msg);
			return;
		}
	}
}

/* --- 辅助函数：写一页并等待完成 --- */
static void program_page_and_wait(uint32_t addr, const uint8_t *data, uint32_t len)
{
	if (MT25QU02_WriteEnable() != MT25QU02_OK) {
		TEST_FAIL_MESSAGE("MT25QU02_WriteEnable failed");
	}
	/* MT25QU02_PageProgram 接受非 const 指针，强制转换 */
	if (MT25QU02_PageProgram((uint8_t *)data, addr, len) != MT25QU02_OK) {
		TEST_FAIL_MESSAGE("MT25QU02_PageProgram failed");
	}
	if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK) {
		TEST_FAIL_MESSAGE("MT25QU02_AutoPollingMemReady failed");
	}
}

/* --- 辅助函数：在多个地址写入相同数据并等待完成 --- */
static void program_multiple_pages(const uint32_t addrs[], uint32_t count, const uint8_t *data, uint32_t len)
{
	for (uint32_t i = 0; i < count; i++) {
		program_page_and_wait(addrs[i], data, len);
	}
}

/* --- 辅助函数：配置QPI模式 --- */
static void configure_qpi_mode(void)
{
    MT25QU02_HandleTypeDef dev = MT25QU02_GetHandle();
    dev.interface_mode = MT25QU02_QPI_MODE;
    MT25QU02_Update(&dev);
}

static void ut_read_id(void)
{
    uint8_t id[3] = {0,0,0};
    if (MT25QU02_ReadID(&id[0]) != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_ReadID failed");
    }
    TEST_ASSERT_EQUAL_UINT8(0x20, id[0]); // Manufacturer ID for Micron
    TEST_ASSERT_EQUAL_UINT8(0xBB, id[1]); // Memory Type
    TEST_ASSERT_EQUAL_UINT8(0x22, id[2]); // Capacity
}

static void ut_read_memory_erase_then_verify(uint32_t addr, uint32_t size)
{
    // MT25QU02_Init();

    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(addr, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();

    uint8_t read_data[size];
    for(uint32_t i = 0; i < size; i++) read_data[i] = 0;
    int32_t status = MT25QU02_ReadSTR(read_data, addr, size);
    TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
    for(uint32_t i = 0; i < size; i++) {
    	TEST_ASSERT_EQUAL_UINT8(0xFF, read_data[i]);
    }
}

static void ut_write_memory_and_verify(uint32_t addr, const uint8_t *data, uint32_t size)
{
    // MT25QU02_Init();

    if(MT25QU02_WriteEnable() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_WriteEnable failed");
    }
    if(MT25QU02_PageProgram((uint8_t*)data, addr, size) != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_PageProgram failed");
    }
    if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_AutoPollingMemReady failed");
    }

    uint8_t read_data[size];
    for(uint32_t i = 0; i < size; i++) read_data[i] = 0;
    int32_t status = MT25QU02_ReadSTR(read_data, addr, size);
    TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
    for(uint32_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(data[i], read_data[i]);
    }
}

static void ut_erase_NK_and_verify(uint32_t base, MT25QU02_Erase_t erase_type)
{
    // MT25QU02_Init();
    const uint32_t write_size = TEST_MT25QU02_ERASE_WRITE_SIZE;
    uint8_t write_data[TEST_MT25QU02_ERASE_WRITE_SIZE];
    for(uint32_t i = 0; i < sizeof(write_data); i++) write_data[i] = (uint8_t)i;

    uint32_t a1 = base;
    uint32_t a2 = base - write_size;
    uint32_t a4 = base + ((erase_type == MT25QU02_ERASE_64K) ? 0x10000 : (erase_type == MT25QU02_ERASE_32K) ? 0x8000 : (erase_type == MT25QU02_ERASE_4K) ? 0x1000 : 0);
    uint32_t a3 = a4 - write_size;

    const uint32_t addrs[] = { a1, a2, a3, a4 };
    program_multiple_pages(addrs, 4, write_data, write_size);

    verify_memory_equals(a1, write_data, write_size);
    verify_memory_equals(a2, write_data, write_size);
    verify_memory_equals(a3, write_data, write_size);
    verify_memory_equals(a4, write_data, write_size);

    if (MT25QU02_WriteEnable() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_WriteEnable failed");
    if (MT25QU02_BlockErase(base, erase_type) != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_BlockErase failed");
    if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_AutoPollingMemReady failed");

    /* 保持 a2, a4 为对比，a1, a3 应被擦除 */
    verify_memory_filled(a1, 0xFF, write_size);
    verify_memory_equals(a2, write_data, write_size);
    verify_memory_filled(a3, 0xFF, write_size);
    verify_memory_equals(a4, write_data, write_size);

    /* 清理边界块 */
    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(a2, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();
    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(a4, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();
}

static void ut_chip_erase_and_verify(void)
{
    // MT25QU02_Init();

    const uint32_t write_size = TEST_MT25QU02_ERASE_WRITE_SIZE;
    uint8_t write_data[TEST_MT25QU02_ERASE_WRITE_SIZE];
    for(uint32_t i = 0; i < sizeof(write_data); i++) write_data[i] = (uint8_t)i;

    uint32_t a1 = 0x000000;
    uint32_t a2 = 0x100000;
    uint32_t a3 = 0x200000;
    const uint32_t addrs[] = { a1, a2, a3 };
    program_multiple_pages(addrs, 3, write_data, write_size);

    verify_memory_equals(a1, write_data, write_size);
    verify_memory_equals(a2, write_data, write_size);
    verify_memory_equals(a3, write_data, write_size);

    if (MT25QU02_WriteEnable() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_WriteEnable failed");
    if (MT25QU02_DieErase(0) != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_DieErase failed");
    if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_AutoPollingMemReady failed");

    verify_memory_filled(a1, 0xFF, write_size);
    verify_memory_filled(a2, 0xFF, write_size);
    verify_memory_filled(a3, 0xFF, write_size);
}

static void ut_memory_mapped_read_verify(uint32_t write_address, const uint8_t *data, uint32_t size)
{
    // MT25QU02_Init();

    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(write_address, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();

    if(MT25QU02_WriteEnable() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_WriteEnable failed");
    if(MT25QU02_PageProgram((uint8_t*)data, write_address, size) != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_PageProgram failed");
    if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_AutoPollingMemReady failed");
    if(MT25QU02_EnableMemoryMappedModeSTR() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_EnableMemoryMappedModeSTR failed");

    uint8_t *mem_mapped_ptr = (uint8_t *)0x70000000;
    for(uint32_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(data[i], mem_mapped_ptr[write_address + i]);
    }

    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(write_address, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();

    HAL_XSPI_Abort(MT25QU02_GetHandle().xspi_handler);
}

/* --- 辅助函数：DTR模式下内存映射读取验证 --- */
static void ut_memory_mapped_read_dtr_verify(uint32_t write_address, const uint8_t *data, uint32_t size)
{
    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(write_address, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();

    if(MT25QU02_WriteEnable() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_WriteEnable failed");
    if(MT25QU02_PageProgram((uint8_t*)data, write_address, size) != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_PageProgram failed");
    if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_AutoPollingMemReady failed");
    if(MT25QU02_EnableMemoryMappedModeDTR() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_EnableMemoryMappedModeDTR failed");

    uint8_t *mem_mapped_ptr = (uint8_t *)0x70000000;
    for(uint32_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(data[i], mem_mapped_ptr[write_address + i]);
    }

    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(write_address, MT25QU02_SUBSECTOR_4K);
    MT25QU02_AutoPollingMemReady();

    HAL_XSPI_Abort(MT25QU02_GetHandle().xspi_handler);
}

/* --- 辅助函数：DTR模式下读取并验证 --- */
static void ut_read_dtr_memory_and_verify(uint32_t addr, const uint8_t *data, uint32_t size)
{
    uint8_t read_data[size];
    for(uint32_t i = 0; i < size; i++) read_data[i] = 0;
    int32_t status = MT25QU02_ReadDTR(read_data, addr, size);
    TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
    for(uint32_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(data[i], read_data[i]);
    }
}

static void ut_enter_qpi_mode_and_verify(void)
{
    // MT25QU02_Init();
    if(MT25QU02_EnterQPIMode() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_EnterQPIMode failed");
    ut_read_id();
}

static void ut_exit_qpi_mode_and_verify(void)
{
    // MT25QU02_Init();
    if(MT25QU02_ExitQPIMode() != MT25QU02_OK) TEST_FAIL_MESSAGE("MT25QU02_ExitQPIMode failed");
    ut_read_id();
}


/* --- SPI 模式测试组 --- */
TEST_GROUP(MT25QU02_1_1_1_MODE);
TEST_SETUP(MT25QU02_1_1_1_MODE) {}
TEST_TEAR_DOWN(MT25QU02_1_1_1_MODE) {}

TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_ReadID) { ut_read_id(); }
TEST(MT25QU02_1_1_1_MODE,TEST_MT25QU02_ReadMemory){ ut_read_memory_erase_then_verify(0x000000, 256); }
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_WriteMemory) {
    uint8_t write_data[256];
    for(uint32_t i = 0; i < sizeof(write_data); i++) write_data[i] = (uint8_t)i;
    ut_write_memory_and_verify(0x000000, write_data, sizeof(write_data));
}
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_Erase4K){ ut_erase_NK_and_verify(0x001000, MT25QU02_ERASE_4K); }
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_Erase32K){ ut_erase_NK_and_verify(0x010000, MT25QU02_ERASE_32K); }
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_Erase64K){ ut_erase_NK_and_verify(0x020000, MT25QU02_ERASE_64K); }
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_DieErase){ ut_chip_erase_and_verify(); }
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_MemoryMappedRead){
    uint8_t write_data[64];
    for(uint32_t i = 0; i < sizeof(write_data); i++) write_data[i] = (uint8_t)i;
    ut_memory_mapped_read_verify(0x001000, write_data, sizeof(write_data));
}
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_ReadDTR) {
    uint8_t write_data[256];
    for(uint32_t i = 0; i < sizeof(write_data); i++) write_data[i] = (uint8_t)i;
    ut_write_memory_and_verify(0x002000, write_data, sizeof(write_data));
    ut_read_dtr_memory_and_verify(0x002000, write_data, sizeof(write_data));
}
TEST(MT25QU02_1_1_1_MODE, TEST_MT25QU02_MemoryMappedReadDTR){
    uint8_t write_data[64];
    for(uint32_t i = 0; i < sizeof(write_data); i++) write_data[i] = (uint8_t)i;
    ut_memory_mapped_read_dtr_verify(0x003000, write_data, sizeof(write_data));
}

TEST_GROUP_RUNNER(MT25QU02_1_1_1_MODE) {
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_ReadID);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_ReadMemory);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_WriteMemory);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_Erase4K);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_Erase32K);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_Erase64K);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_DieErase);
   RUN_TEST_CASE(MT25QU02_1_1_1_MODE, TEST_MT25QU02_MemoryMappedRead);
}


/* --- MT25QU02 BSP整体测试组 --- */
TEST_GROUP(MT25QU02_BSP_TEST);
TEST_SETUP(MT25QU02_BSP_TEST) {}
TEST_TEAR_DOWN(MT25QU02_BSP_TEST) {}
TEST_GROUP_RUNNER(MT25QU02_BSP_TEST) {
	MT25QU02_Init();

    // 组合1: SPI模式, 3字节地址 (默认)
    RUN_TEST_GROUP(MT25QU02_1_1_1_MODE);

    // 组合2: SPI模式, 4字节地址
    if (MT25QU02_Enter4BytesAddressMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_Enter4BytesAddressMode failed");
    }
    RUN_TEST_GROUP(MT25QU02_1_1_1_MODE);
    if (MT25QU02_Exit4BytesAddressMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_Exit4BytesAddressMode failed");
    }

    // 组合3: QPI模式, 3字节地址
    if (MT25QU02_EnterQPIMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_EnterQPIMode failed");
    }
    RUN_TEST_GROUP(MT25QU02_1_1_1_MODE);
    if (MT25QU02_ExitQPIMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_ExitQPIMode failed");
    }

    // 组合4: QPI模式, 4字节地址
    if (MT25QU02_EnterQPIMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_EnterQPIMode failed");
    }
    if (MT25QU02_Enter4BytesAddressMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_Enter4BytesAddressMode failed");
    }
    RUN_TEST_GROUP(MT25QU02_1_1_1_MODE);
    if (MT25QU02_Exit4BytesAddressMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_Exit4BytesAddressMode failed");
    }
    if (MT25QU02_ExitQPIMode() != MT25QU02_OK) {
        TEST_FAIL_MESSAGE("MT25QU02_ExitQPIMode failed");
    }
}
