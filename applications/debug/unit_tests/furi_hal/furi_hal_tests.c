#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <lp5562_reg.h>
#include "../minunit.h"

#define DATA_SIZE 4

static void furi_hal_i2c_int_setup() {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_power);
}

static void furi_hal_i2c_int_teardown() {
    furi_hal_i2c_release(&furi_hal_i2c_handle_power);
}

MU_TEST(furi_hal_i2c_int_1b) {
    bool ret = false;
    uint8_t data_one = 0;

    // 1 byte: read, write, read
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "0 read_reg_8 failed");
    mu_assert(data_one != 0, "0 invalid data");
    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "1 write_reg_8 failed");
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "2 read_reg_8 failed");
    mu_assert(data_one != 0, "2 invalid data");
}

MU_TEST(furi_hal_i2c_int_3b) {
    bool ret = false;
    uint8_t data_many[DATA_SIZE] = {0};

    // 3 byte: read, write, read
    data_many[0] = LP5562_CHANNEL_BLUE_CURRENT_REGISTER;
    ret = furi_hal_i2c_tx(
        &furi_hal_i2c_handle_power, LP5562_ADDRESS, data_many, 1, LP5562_I2C_TIMEOUT);
    mu_assert(ret, "3 tx failed");
    ret = furi_hal_i2c_rx(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        data_many + 1,
        DATA_SIZE - 1,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "4 rx failed");
    for(size_t i = 0; i < DATA_SIZE; i++) mu_assert(data_many[i] != 0, "4 invalid data_many");

    ret = furi_hal_i2c_tx(
        &furi_hal_i2c_handle_power, LP5562_ADDRESS, data_many, DATA_SIZE, LP5562_I2C_TIMEOUT);
    mu_assert(ret, "5 tx failed");

    ret = furi_hal_i2c_tx(
        &furi_hal_i2c_handle_power, LP5562_ADDRESS, data_many, 1, LP5562_I2C_TIMEOUT);
    mu_assert(ret, "6 tx failed");
    ret = furi_hal_i2c_rx(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        data_many + 1,
        DATA_SIZE - 1,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "7 rx failed");
    for(size_t i = 0; i < DATA_SIZE; i++) mu_assert(data_many[i] != 0, "7 invalid data_many");
}

MU_TEST(furi_hal_i2c_int_1b_fail) {
    bool ret = false;
    uint8_t data_one = 0;

    // 1 byte: fail, read, fail, write, fail, read
    data_one = 0;
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS + 0x10,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(!ret, "8 read_reg_8 failed");
    mu_assert(data_one == 0, "8 invalid data");
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "9 read_reg_8 failed");
    mu_assert(data_one != 0, "9 invalid data");
}

MU_TEST(furi_hal_dma_aq_rel) {
    const FuriHalDma* dma_1 = furi_hal_dma_acquire_channel();
    const FuriHalDma* dma_2 = furi_hal_dma_acquire_channel();
    mu_assert_pointers_not_eq(dma_1, NULL);
    mu_assert_pointers_not_eq(dma_2, NULL);
    mu_assert_pointers_not_eq(dma_1, dma_2);
    furi_hal_dma_release_channel(dma_1);
    furi_hal_dma_release_channel(dma_2);
}

MU_TEST(furi_hal_dma_aq_full) {
    // STM32WB55 has 14 DMA channels, so I assume that there are less than 32 DMA channels
    const size_t dma_channels_max = 32;
    const FuriHalDma* dma[dma_channels_max];

    for(size_t i = 0; i < dma_channels_max; i++) {
        dma[i] = furi_hal_dma_acquire_channel();
    }

    // check that some channels are acquired
    mu_assert_pointers_not_eq(dma[0], NULL);

    // check that some channels are not acquired
    mu_assert_pointers_eq(dma[dma_channels_max - 1], NULL);

    // release all channels
    for(size_t i = 0; i < dma_channels_max; i++) {
        if(dma[i] != NULL) {
            furi_hal_dma_release_channel(dma[i]);
        }
    }
}

MU_TEST_SUITE(furi_hal_i2c_int_suite) {
    MU_SUITE_CONFIGURE(&furi_hal_i2c_int_setup, &furi_hal_i2c_int_teardown);
    MU_RUN_TEST(furi_hal_i2c_int_1b);
    MU_RUN_TEST(furi_hal_i2c_int_3b);
    MU_RUN_TEST(furi_hal_i2c_int_1b_fail);
}

MU_TEST_SUITE(furi_hal_dma_suite) {
    MU_RUN_TEST(furi_hal_dma_aq_rel);
    MU_RUN_TEST(furi_hal_dma_aq_full);
}

int run_minunit_test_furi_hal() {
    MU_RUN_SUITE(furi_hal_i2c_int_suite);
    MU_RUN_SUITE(furi_hal_dma_suite);
    return MU_EXIT_CODE;
}
