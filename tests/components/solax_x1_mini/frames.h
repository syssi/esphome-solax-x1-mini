#pragma once
#include <cstdint>
#include <vector>

namespace esphome::solax_x1_mini::testing {

// Source: solax_x1_mini.cpp comment, G2 status frame (data_len 0x32: 50 bytes)
// Full frame: AA.55.00.0A.01.00.11.82.32.00.21.00.02.07.EC.00.00.00.1D.00.00.00.18.09.55.13.80.02.2B.FF.FF.
//             00.00.5D.AF.00.00.10.50.00.02.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.07.A4
// Payload = frame bytes [9..58] (function 0x82 = FUNCTION_STATUS_REPORT)
// temperature=33°C  energy_today=0.2kWh  dc1_voltage=202.8V  dc2_voltage=0.0V
// dc1_current=2.9A  dc2_current=0.0A  ac_current=2.4A  ac_voltage=238.9V
// ac_frequency=49.92Hz  ac_power=555W  energy_total=2398.3kWh  runtime_total=4176s
// mode=2("Normal")  error_bits=0  errors=""
static const uint8_t FUNCTION_STATUS_REPORT = 0x82;
static const std::vector<uint8_t> G2_STATUS_FRAME = {
    0x00, 0x21, 0x00, 0x02, 0x07, 0xEC, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x18, 0x09, 0x55, 0x13,
    0x80, 0x02, 0x2B, 0xFF, 0xFF, 0x00, 0x00, 0x5D, 0xAF, 0x00, 0x00, 0x10, 0x50, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

}  // namespace esphome::solax_x1_mini::testing
