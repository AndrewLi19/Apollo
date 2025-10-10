#include "unity_fixture.h"
#include "mt25ql512abb.h"

#define TEST_SPI_MODE

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
    MT25QU02_ReadSTR(read_data, read_address, read_size);
    for(uint32_t i = 0; i < read_size; i++) {
    	TEST_ASSERT_EQUAL_UINT8(0xFF, read_data[i]);
    }
}

TEST_GROUP_RUNNER(MT25QU02_Driver) {
    RUN_TEST_CASE(MT25QU02_Driver, TEST_MT25QU02_ReadID);
    RUN_TEST_CASE(MT25QU02_Driver, TEST_MT25QU02_ReadMemory);
}
