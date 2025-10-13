#include "unity_fixture.h"
#include "mt25ql512abb.h"

#define TEST_SPI_MODE
#define TEST_MT25QU02_ERASE_WRITE_SIZE 64

/* 辅助函数：从 flash 读取并与期望缓冲区比较 */
static void verify_memory_equals(uint32_t addr, const uint8_t *expected, uint32_t len)
{
	/* 使用可变长度数组临时存放读取数据 */
	uint8_t buf[len];
	int32_t status = MT25QU02_ReadSTR(buf, addr, len);
	TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
	for (uint32_t i = 0; i < len; i++) {
		TEST_ASSERT_EQUAL_UINT8(expected[i], buf[i]);
	}
}

/* 辅助函数：从 flash 读取并与某个填充值比较（例如 0xFF） */
static void verify_memory_filled(uint32_t addr, uint8_t value, uint32_t len)
{
	uint8_t buf[len];
	int32_t status = MT25QU02_ReadSTR(buf, addr, len);
	TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
	for (uint32_t i = 0; i < len; i++) {
		TEST_ASSERT_EQUAL_UINT8(value, buf[i]);
	}
}

TEST_GROUP(MT25QU02_Driver);

TEST_SETUP(MT25QU02_Driver) {
}

TEST_TEAR_DOWN(MT25QU02_Driver) {
}

TEST(MT25QU02_Driver, TEST_MT25QU02_ReadID) {
    MT25QU02_Init();
    uint8_t id[3] = {0, 0, 0};
    MT25QU02_ReadID(&id[0]);
    TEST_ASSERT_EQUAL_UINT8(0x20, id[0]); // Manufacturer ID for Micron
    TEST_ASSERT_EQUAL_UINT8(0xBB, id[1]); // Memory Type
    TEST_ASSERT_EQUAL_UINT8(0x22, id[2]); // Capacity
}

TEST(MT25QU02_Driver,TEST_MT25QU02_ReadMemory){
    MT25QU02_Init();
    uint8_t read_data[256] = {0};
    uint32_t read_address = 0x000000;
    uint32_t read_size = sizeof(read_data);
    int32_t status = MT25QU02_ReadSTR(read_data, read_address, read_size);
    TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
    for(uint32_t i = 0; i < read_size; i++) {
    	TEST_ASSERT_EQUAL_UINT8(0xFF, read_data[i]);
    }
}

TEST(MT25QU02_Driver, TEST_MT25QU02_WriteMemory) {
    MT25QU02_Init();
    uint8_t write_data[256];
    for(uint32_t i = 0; i < sizeof(write_data); i++) {
        write_data[i] = (uint8_t)i;
    }
    uint32_t write_address = 0x000000;
    uint32_t write_size = sizeof(write_data);

    MT25QU02_WriteEnable();
    MT25QU02_PageProgram(write_data, write_address, write_size);

    // Read back the data to verify
    uint8_t read_data[256] = {0};
    int32_t status = MT25QU02_ReadSTR(read_data, write_address, write_size);
    TEST_ASSERT_EQUAL_INT32(MT25QU02_OK, status);
    for(uint32_t i = 0; i < write_size; i++) {
        TEST_ASSERT_EQUAL_UINT8(write_data[i], read_data[i]);
    }
}

TEST(MT25QU02_Driver, TEST_MT25QU02_Erase4K){
    MT25QU02_Init();

    uint32_t write_size = TEST_MT25QU02_ERASE_WRITE_SIZE;
    uint8_t write_data[TEST_MT25QU02_ERASE_WRITE_SIZE];
    for(uint32_t i = 0; i < sizeof(write_data); i++) {
        write_data[i] = (uint8_t)i;
    }
    uint32_t write_address = 0x000000;

    MT25QU02_WriteEnable();
    MT25QU02_PageProgram(write_data, write_address, write_size);

    uint32_t write_address_2 = 0x000100;
    MT25QU02_WriteEnable();
    MT25QU02_PageProgram(write_data, write_address_2, write_size);

    uint32_t write_address_3 = 0xF00000;
    MT25QU02_WriteEnable();
    MT25QU02_PageProgram(write_data, write_address_3, write_size);

    verify_memory_equals(write_address, write_data, write_size);
    verify_memory_equals(write_address_2, write_data, write_size);
    verify_memory_equals(write_address_3, write_data, write_size);

    // Erase 4K sector
    MT25QU02_WriteEnable();
    MT25QU02_BlockErase(write_address, MT25QU02_SUBSECTOR_4K);

    // Read back the data to verify it's erased
    verify_memory_filled(write_address, 0xFF, write_size);
    verify_memory_filled(write_address_2, 0xFF, write_size);
    verify_memory_equals(write_address_3, write_data, write_size);

    MT25QU02_BlockErase(write_address_3, MT25QU02_SUBSECTOR_4K);
}

TEST_GROUP_RUNNER(MT25QU02_Driver) {
//    RUN_TEST_CASE(MT25QU02_Driver, TEST_MT25QU02_ReadID);
//    RUN_TEST_CASE(MT25QU02_Driver, TEST_MT25QU02_ReadMemory);
//    RUN_TEST_CASE(MT25QU02_Driver, TEST_MT25QU02_WriteMemory);
    RUN_TEST_CASE(MT25QU02_Driver, TEST_MT25QU02_Erase4K);
}
