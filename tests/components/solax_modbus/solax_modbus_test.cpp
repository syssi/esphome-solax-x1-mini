#include <gtest/gtest.h>
#include "common.h"

namespace esphome::solax_modbus::testing {

TEST(SolaxModbusTest, ValidFrameDispatchedToDevice) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  modbus.feed(STATUS_FRAME);

  EXPECT_EQ(device.call_count, 1);
}

TEST(SolaxModbusTest, FunctionCodePassedToDevice) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  modbus.feed(STATUS_FRAME);

  EXPECT_EQ(device.last_function, 0x02);
}

TEST(SolaxModbusTest, TwoFramesDispatchedTwice) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  modbus.feed(STATUS_FRAME);
  modbus.feed(STATUS_FRAME);

  EXPECT_EQ(device.call_count, 2);
}

TEST(SolaxModbusTest, BadChecksumRejected) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  std::vector<uint8_t> bad_frame = STATUS_FRAME;
  bad_frame.back() ^= 0xFF;  // corrupt checksum
  modbus.feed(bad_frame);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxModbusTest, UnknownAddressNotDispatched) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x99);
  modbus.register_device(&device);

  modbus.feed(STATUS_FRAME);  // address=0x0A != 0x99

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxModbusTest, NoRegisteredDeviceDoesNotCrash) {
  TestableSolaxModbus modbus;
  modbus.feed(STATUS_FRAME);
}

TEST(SolaxModbusTest, PartialFrameDoesNotDispatch) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  for (size_t i = 0; i < STATUS_FRAME.size() - 2; i++)
    modbus.parse_solax_modbus_byte_(STATUS_FRAME[i]);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxModbusTest, ErrorBitInSecondByteRejected) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  // Feed byte 0 (valid), then byte 1 with high bit set (error indicator)
  modbus.parse_solax_modbus_byte_(0xAA);
  bool result = modbus.parse_solax_modbus_byte_(0x85);  // high bit set -> error
  EXPECT_FALSE(result);
  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxModbusTest, NonDispatchControlCodeNoCallback) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device;
  device.set_address(0x0A);
  modbus.register_device(&device);

  // cc=0x10 -> device is found but on_solax_modbus_data is not called
  modbus.feed(WRONG_CC_FRAME);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SolaxModbusTest, AddressMatchSelectsCorrectDevice) {
  TestableSolaxModbus modbus;
  MockSolaxModbusDevice device_0a, device_01;
  device_0a.set_address(0x0A);
  device_01.set_address(0x01);
  modbus.register_device(&device_0a);
  modbus.register_device(&device_01);

  modbus.feed(STATUS_FRAME_ADDR01);

  EXPECT_EQ(device_0a.call_count, 0);
  EXPECT_EQ(device_01.call_count, 1);
}

}  // namespace esphome::solax_modbus::testing
