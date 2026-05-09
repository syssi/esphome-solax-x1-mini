#include <gtest/gtest.h>
#include "common.h"

namespace esphome::solax_meter_modbus::testing {

TEST(SolaxMeterModbusTest, ValidFrameDispatchedToDevice) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(HANDSHAKE_FRAME);

  EXPECT_EQ(device.call_count, 1);
}

TEST(SolaxMeterModbusTest, FrameDataPassedToDevice) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(HANDSHAKE_FRAME);

  // data = raw[1..5] = [0x03, 0x00, 0x0B, 0x00, 0x01]
  ASSERT_EQ(device.received_data.size(), 5u);
  EXPECT_EQ(device.received_data[0], 0x03);
  EXPECT_EQ(device.received_data[2], 0x0B);
}

TEST(SolaxMeterModbusTest, TwoFramesDispatchedTwice) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(HANDSHAKE_FRAME);
  modbus.feed(READ_POWER_FRAME);

  EXPECT_EQ(device.call_count, 2);
}

TEST(SolaxMeterModbusTest, BadChecksumRejected) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  std::vector<uint8_t> bad_frame = HANDSHAKE_FRAME;
  bad_frame.back() ^= 0xFF;  // corrupt CRC
  modbus.feed(bad_frame);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxMeterModbusTest, UnknownAddressNotDispatched) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device;
  device.set_address(0x99);
  modbus.register_device(&device);

  modbus.feed(HANDSHAKE_FRAME);  // address=0x01 != 0x99

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxMeterModbusTest, NoRegisteredDeviceDoesNotCrash) {
  TestableSolaxMeterModbus modbus;
  modbus.feed(HANDSHAKE_FRAME);
}

TEST(SolaxMeterModbusTest, PartialFrameDoesNotDispatch) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  for (size_t i = 0; i < HANDSHAKE_FRAME.size() - 2; i++)
    modbus.parse_solax_meter_modbus_byte_(HANDSHAKE_FRAME[i]);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxMeterModbusTest, AddressMatchSelectsCorrectDevice) {
  TestableSolaxMeterModbus modbus;
  MockSolaxMeterModbusDevice device_01, device_02;
  device_01.set_address(0x01);
  device_02.set_address(0x02);
  modbus.register_device(&device_01);
  modbus.register_device(&device_02);

  modbus.feed(HANDSHAKE_FRAME_ADDR02);

  EXPECT_EQ(device_01.call_count, 0);
  EXPECT_EQ(device_02.call_count, 1);
}

}  // namespace esphome::solax_meter_modbus::testing
