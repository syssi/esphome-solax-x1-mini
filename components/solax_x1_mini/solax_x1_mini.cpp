#include "solax_x1_mini.h"
#include "esphome/core/log.h"

namespace esphome {
namespace solax_x1_mini {

static const char *const TAG = "solax_x1_mini";

static const uint8_t FUNCTION_STATUS_REPORT = 0x82;
static const uint8_t FUNCTION_DEVICE_INFO = 0x83;
static const uint8_t FUNCTION_CONFIG_SETTINGS = 0x84;

// SolaxPower Single Phase External Communication Protocol - X1 Series V1.2.pdf
// SolaxPower Single Phase External Communication Protocol - X1 Series V1.8.pdf
static const uint8_t MODES_SIZE = 7;
static const std::string MODES[MODES_SIZE] = {
    "Wait",             // 0
    "Check",            // 1
    "Normal",           // 2
    "Fault",            // 3
    "Permanent Fault",  // 4
    "Update",           // 5
    "Self Test",        // 6
};

// SolaxPower Single Phase External Communication Protocol - X1 Series V1.8.pdf
static const uint8_t ERRORS_SIZE = 32;
static const char *const ERRORS[ERRORS_SIZE] = {
    "TZ Protect Fault",                          // 0000 0000 0000 0000 0000 0000 0000 0001 (1)
    "Grid Lost Fault",                           // 0000 0000 0000 0000 0000 0000 0000 0010 (2)
    "Grid Voltage Fault",                        // 0000 0000 0000 0000 0000 0000 0000 0100 (3)
    "Grid Frequency Fault",                      // 0000 0000 0000 0000 0000 0000 0000 1000 (4)
    "PLL Lost Fault",                            // 0000 0000 0000 0000 0000 0000 0001 0000 (5)
    "Bus Voltage Fault",                         // 0000 0000 0000 0000 0000 0000 0010 0000 (6)
    "Error (Bit 6)",                             // 0000 0000 0000 0000 0000 0000 0100 0000 (7)
    "Oscillator Fault",                          // 0000 0000 0000 0000 0000 0000 1000 0000 (8)
    "DCI Over Current Protection Fault",         // 0000 0000 0000 0000 0000 0001 0000 0000 (9)
    "Residual Current Fault",                    // 0000 0000 0000 0000 0000 0010 0000 0000 (10)
    "PV Voltage Fault",                          // 0000 0000 0000 0000 0000 0100 0000 0000 (11)
    "AC voltage out of range since 10 minutes",  // 0000 0000 0000 0000 0000 1000 0000 0000 (12)
    "Isolation Fault",                           // 0000 0000 0000 0000 0001 0000 0000 0000 (13)
    "Over Temperature Fault",                    // 0000 0000 0000 0000 0010 0000 0000 0000 (14)
    "Fan Fault",                                 // 0000 0000 0000 0000 0100 0000 0000 0000 (15)
    "Error (Bit 15)",                            // 0000 0000 0000 0000 1000 0000 0000 0000 (16)
    "SPI Communication Fault",                   // 0000 0000 0000 0001 0000 0000 0000 0000 (17)
    "SCI Communication Fault",                   // 0000 0000 0000 0010 0000 0000 0000 0000 (18)
    "Error (Bit 18)",                            // 0000 0000 0000 0100 0000 0000 0000 0000 (19)
    "Input Configuration Fault",                 // 0000 0000 0000 1000 0000 0000 0000 0000 (20)
    "EEPROM Fault",                              // 0000 0000 0001 0000 0000 0000 0000 0000 (21)
    "Relay Fault",                               // 0000 0000 0010 0000 0000 0000 0000 0000 (22)
    "Sample Consistence Fault",                  // 0000 0000 0100 0000 0000 0000 0000 0000 (23)
    "Residual Current Device Fault",             // 0000 0000 1000 0000 0000 0000 0000 0000 (24)
    "Error (Bit 24)",                            // 0000 0001 0000 0000 0000 0000 0000 0000 (25)
    "Error (Bit 25)",                            // 0000 0010 0000 0000 0000 0000 0000 0000 (26)
    "Error (Bit 26)",                            // 0000 0100 0000 0000 0000 0000 0000 0000 (27)
    "Error (Bit 27)",                            // 0000 1000 0000 0000 0000 0000 0000 0000 (28)
    "Error (Bit 28)",                            // 0001 0000 0000 0000 0000 0000 0000 0000 (29)
    "DCI Device Fault",                          // 0010 0000 0000 0000 0000 0000 0000 0000 (30)
    "Other Device Fault",                        // 0100 0000 0000 0000 0000 0000 0000 0000 (31)
    "Error (Bit 31)",                            // 1000 0000 0000 0000 0000 0000 0000 0000 (32)
};

void SolaxX1Mini::on_solax_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) {
  switch (function) {
    case FUNCTION_DEVICE_INFO:
      this->decode_device_info_(data);
      break;
    case FUNCTION_STATUS_REPORT:
      this->decode_status_report_(data);
      break;
    case FUNCTION_CONFIG_SETTINGS:
      this->decode_config_settings_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unhandled solax frame: %s", format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void SolaxX1Mini::decode_device_info_(const std::vector<uint8_t> &data) {
  if (data.size() != 58) {
    ESP_LOGW(TAG, "Invalid response size: %zu", data.size());
    return;
  }

  ESP_LOGI(TAG, "Device info frame received");
  ESP_LOGI(TAG, "  Device type: %d", data[0]);
  ESP_LOGI(TAG, "  Rated power: %s", std::string(data.begin() + 1, data.begin() + 1 + 6).c_str());
  ESP_LOGI(TAG, "  Firmware version: %s", std::string(data.begin() + 7, data.begin() + 7 + 5).c_str());
  ESP_LOGI(TAG, "  Module name: %s", std::string(data.begin() + 12, data.begin() + 12 + 14).c_str());
  ESP_LOGI(TAG, "  Manufacturer: %s", std::string(data.begin() + 26, data.begin() + 26 + 14).c_str());
  ESP_LOGI(TAG, "  Serial number: %s", std::string(data.begin() + 40, data.begin() + 40 + 14).c_str());
  ESP_LOGI(TAG, "  Rated bus voltage: %s", std::string(data.begin() + 54, data.begin() + 54 + 4).c_str());

  this->no_response_count_ = 0;
}

void SolaxX1Mini::decode_config_settings_(const std::vector<uint8_t> &data) {
  if (data.size() != 68) {
    ESP_LOGW(TAG, "Invalid response size: %zu", data.size());
    return;
  }

  auto solax_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Config settings frame received");
  ESP_LOGI(TAG, "  wVpvStart [9.10]: %f V", solax_get_16bit(0) * 0.1f);
  ESP_LOGI(TAG, "  wTimeStart [11.12]: %d S", solax_get_16bit(2));
  ESP_LOGI(TAG, "  wVacMinProtect [13.14]: %f V", solax_get_16bit(4) * 0.1f);
  ESP_LOGI(TAG, "  wVacMaxProtect [15.16]: %f V", solax_get_16bit(6) * 0.1f);
  ESP_LOGI(TAG, "  wFacMinProtect [17.18]: %f Hz", solax_get_16bit(8) * 0.01f);
  ESP_LOGI(TAG, "  wFacMaxProtect [19.20]: %f Hz", solax_get_16bit(10) * 0.01f);
  ESP_LOGI(TAG, "  wDciLimits [21.22]: %d mA", solax_get_16bit(12));
  ESP_LOGI(TAG, "  wGrid10MinAvgProtect [23,24]: %f V", solax_get_16bit(14) * 0.1f);
  ESP_LOGI(TAG, "  wVacMinSlowProtect [25.26]: %f V", solax_get_16bit(16) * 0.1f);
  ESP_LOGI(TAG, "  wVacMaxSlowProtect [27.28]: %f V", solax_get_16bit(18) * 0.1f);
  ESP_LOGI(TAG, "  wFacMinSlowProtect [29.30]: %f Hz", solax_get_16bit(20) * 0.01f);
  ESP_LOGI(TAG, "  wFacMaxSlowProtect [31.32]: %f Hz", solax_get_16bit(22) * 0.01f);
  ESP_LOGI(TAG, "  wSafety [33.34]: %d", solax_get_16bit(24));
  // Supported safety values:
  //
  // 0: VDE0126
  // 1: VDE4105
  // 2: AS4777
  // 3: G98
  // 4: C10_11
  // 5: TOR
  // 6: EN50438_NL
  // 7: Denmark2019_W
  // 8: CEB
  // 9: Cyprus2019
  // 10: cNRS097_2_1
  // 11: VDE0126_Greece
  // 12: UTE_C15_712_Fr
  // 13: IEC61727
  // 14: G99
  // 15: CQC
  // 16: VDE0126_Greece_is
  // 17: C15_712_Fr_island_50
  // 18: C15_712_Fr_island_60
  // 19: Guyana
  // 20: MEA_Thailand
  // 21: PEA_Thailand
  // 22: cNewZealand
  // 23: cIreland
  // 24: cCE10_21
  // 25: cRD1699
  // 26: EN50438_Sweden
  // 27: EN50549_PL
  // 28: Czech PPDS
  // 29: EN50438_Norway
  // 30: EN50438_Portug
  // 31: cCQC_WideRange
  // 32: BRAZIL
  // 33: EN50438_CEZ
  // 34: IEC_Chile
  // 35: Sri_Lanka
  // 36: BRAZIL_240
  // 37: EN50549-SK
  // 38: EN50549_EU
  // 39: G98/NI
  // 40: Denmark2019_E
  // 41: RD1699_island
  ESP_LOGI(TAG, "  wPowerfactor_mode [35]: %d", data[26]);
  ESP_LOGI(TAG, "  wPowerfactor_data [36]: %d", data[27]);
  ESP_LOGI(TAG, "  wUpperLimit [37]: %d", data[28]);
  ESP_LOGI(TAG, "  wLowerLimit [38]: %d", data[29]);
  ESP_LOGI(TAG, "  wPowerLow [39]: %d", data[30]);
  ESP_LOGI(TAG, "  wPowerUp [40]: %d", data[31]);
  ESP_LOGI(TAG, "  Qpower_set [41.42]: %d", solax_get_16bit(32));
  ESP_LOGI(TAG, "  WFreqSetPoint [43.44]: %f Hz", solax_get_16bit(34) * 0.01f);
  ESP_LOGI(TAG, "  WFreqDropRate [45.46]: %d", solax_get_16bit(36));
  ESP_LOGI(TAG, "  QuVupRate [47.48]: %d", solax_get_16bit(38));
  ESP_LOGI(TAG, "  QuVlowRate [49.50]: %d", solax_get_16bit(40));
  ESP_LOGI(TAG, "  WPowerLimitsPercent [51.52]: %d", solax_get_16bit(42));
  ESP_LOGI(TAG, "  WWgra [53.54]: %f %%", solax_get_16bit(44) * 0.01f);
  ESP_LOGI(TAG, "  wWv2 [55.56]: %f V", solax_get_16bit(46) * 0.1f);
  ESP_LOGI(TAG, "  wWv3 [57.58]: %f V", solax_get_16bit(48) * 0.1f);
  ESP_LOGI(TAG, "  wWv4 [59.60]: %f V", solax_get_16bit(50) * 0.1f);
  ESP_LOGI(TAG, "  wQurangeV1 [61.62]: %d %%", solax_get_16bit(52));
  ESP_LOGI(TAG, "  wQurangeV4 [63.64]: %d %%", solax_get_16bit(54));
  ESP_LOGI(TAG, "  BVoltPowerLimit [65.66]: %d", solax_get_16bit(56));
  ESP_LOGI(TAG, "  WPowerManagerEnable [67.68]: %d", solax_get_16bit(58));
  ESP_LOGI(TAG, "  WGlobalSearchMPPTStartFlag [69.70]: %d", solax_get_16bit(60));
  ESP_LOGI(TAG, "  WFreqProtectRestrictive [71.72]: %d", solax_get_16bit(62));
  ESP_LOGI(TAG, "  WQuDelayTimer [73.74]: %d S", solax_get_16bit(64));
  ESP_LOGI(TAG, "  WFreqActivePowerDelayTimer [75.76]: %d ms", solax_get_16bit(66));

  this->no_response_count_ = 0;
}

void SolaxX1Mini::decode_status_report_(const std::vector<uint8_t> &data) {
  if (data.size() != 52 && data.size() != 50 && data.size() != 56) {
    // Solax X1 mini status report (data_len 0x34: 52 bytes):
    // AA.55.00.0A.01.00.11.82.34.00.1A.00.02.00.00.00.00.00.00.00.00.00.00.09.21.13.87.00.00.FF.FF.
    // 00.00.00.12.00.00.00.15.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.04.D6

    // Solax X1 mini g2 status report (data_len 0x32: 50 bytes):
    // AA.55.00.0A.01.00.11.82.32.00.21.00.02.07.EC.00.00.00.1D.00.00.00.18.09.55.13.80.02.2B.FF.FF.
    // 00.00.5D.AF.00.00.10.50.00.02.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.07.A4

    // Solax X1 mini g3 status report (data_len 0x38: 56 bytes):
    // AA.55.00.0A.01.00.11.82.38.00.1A.00.03.04.0C.00.00.00.19.00.00.00.0B.08.FC.13.8A.00.F8.FF.FF.
    // 00.00.00.2B.00.00.00.0D.00.02.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.8A.00.DE.08.5F
    ESP_LOGW(TAG, "Invalid response size: %zu", data.size());
    ESP_LOGW(TAG, "Your device is probably not supported. Please create an issue here: "
                  "https://github.com/syssi/esphome-solax-x1-mini/issues");
    ESP_LOGW(TAG, "Please provide the following status response data: %s",
             format_hex_pretty(&data.front(), data.size()).c_str());
    return;
  }

  auto solax_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto solax_get_32bit = [&](size_t i) -> uint32_t {
    return uint32_t((data[i] << 24) | (data[i + 1] << 16) | (data[i + 2] << 8) | data[i + 3]);
  };
  auto solax_get_error_bitmask = [&](size_t i) -> uint32_t {
    return uint32_t((data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i]);
  };

  ESP_LOGI(TAG, "Status frame received");

  this->publish_state_(this->temperature_sensor_, (int16_t) solax_get_16bit(0));
  this->publish_state_(this->energy_today_sensor_, solax_get_16bit(2) * 0.1f);
  this->publish_state_(this->dc1_voltage_sensor_, solax_get_16bit(4) * 0.1f);
  this->publish_state_(this->dc2_voltage_sensor_, solax_get_16bit(6) * 0.1f);
  this->publish_state_(this->dc1_current_sensor_, solax_get_16bit(8) * 0.1f);
  this->publish_state_(this->dc2_current_sensor_, solax_get_16bit(10) * 0.1f);
  this->publish_state_(this->ac_current_sensor_, solax_get_16bit(12) * 0.1f);
  this->publish_state_(this->ac_voltage_sensor_, solax_get_16bit(14) * 0.1f);
  this->publish_state_(this->ac_frequency_sensor_, solax_get_16bit(16) * 0.01f);
  this->publish_state_(this->ac_power_sensor_, solax_get_16bit(18));

  // register 20 is not used

  uint32_t raw_energy_total = solax_get_32bit(22);
  // The inverter publishes a zero once per day on boot-up. This confuses the energy dashboard.
  if (raw_energy_total > 0) {
    this->publish_state_(this->energy_total_sensor_, raw_energy_total * 0.1f);
  }

  uint32_t raw_runtime_total = solax_get_32bit(26);
  if (raw_runtime_total > 0) {
    this->publish_state_(this->runtime_total_sensor_, (float) raw_runtime_total);
  }

  uint8_t mode = (uint8_t) solax_get_16bit(30);
  this->publish_state_(this->mode_sensor_, mode);
  this->publish_state_(this->mode_name_text_sensor_, (mode < MODES_SIZE) ? MODES[mode] : "Unknown");

  this->publish_state_(this->grid_voltage_fault_sensor_, solax_get_16bit(32) * 0.1f);
  this->publish_state_(this->grid_frequency_fault_sensor_, solax_get_16bit(34) * 0.01f);
  this->publish_state_(this->dc_injection_fault_sensor_, solax_get_16bit(36) * 0.001f);
  this->publish_state_(this->temperature_fault_sensor_, (float) solax_get_16bit(38));
  this->publish_state_(this->pv1_voltage_fault_sensor_, solax_get_16bit(40) * 0.1f);
  this->publish_state_(this->pv2_voltage_fault_sensor_, solax_get_16bit(42) * 0.1f);
  this->publish_state_(this->gfc_fault_sensor_, solax_get_16bit(44) * 0.001f);

  uint32_t error_bits = solax_get_error_bitmask(46);
  this->publish_state_(this->error_bits_sensor_, error_bits);
  this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(error_bits));

  if (data.size() > 50) {
    ESP_LOGD(TAG, "  CT Pgrid: %d W", solax_get_16bit(50));
  }

  this->no_response_count_ = 0;
}

void SolaxX1Mini::publish_device_offline_() {
  this->publish_state_(this->mode_sensor_, -1);
  this->publish_state_(this->mode_name_text_sensor_, "Offline");

  this->publish_state_(this->temperature_sensor_, NAN);
  this->publish_state_(this->dc1_voltage_sensor_, 0);
  this->publish_state_(this->dc2_voltage_sensor_, 0);
  this->publish_state_(this->dc1_current_sensor_, 0);
  this->publish_state_(this->dc2_current_sensor_, 0);
  this->publish_state_(this->ac_current_sensor_, 0);
  this->publish_state_(this->ac_voltage_sensor_, NAN);
  this->publish_state_(this->ac_frequency_sensor_, NAN);
  this->publish_state_(this->ac_power_sensor_, 0);
  this->publish_state_(this->grid_voltage_fault_sensor_, NAN);
  this->publish_state_(this->grid_frequency_fault_sensor_, NAN);
  this->publish_state_(this->dc_injection_fault_sensor_, NAN);
  this->publish_state_(this->temperature_fault_sensor_, NAN);
  this->publish_state_(this->pv1_voltage_fault_sensor_, NAN);
  this->publish_state_(this->pv2_voltage_fault_sensor_, NAN);
  this->publish_state_(this->gfc_fault_sensor_, NAN);
}

void SolaxX1Mini::update() {
  if (this->no_response_count_ >= REDISCOVERY_THRESHOLD) {
    this->publish_device_offline_();
    ESP_LOGD(TAG, "The device is or was offline. Broadcasting discovery for address configuration...");
    this->discover_devices();
    //    this->query_device_info(this->address_);
    // Try to query live data on next update again. The device doesn't
    // respond to the discovery broadcast if it's already configured.
    this->no_response_count_ = 0;
  } else {
    this->no_response_count_++;
    this->query_status_report(this->address_);
  }
}

void SolaxX1Mini::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void SolaxX1Mini::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void SolaxX1Mini::dump_config() {
  ESP_LOGCONFIG(TAG, "SolaxX1Mini:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("", "Energy today", this->energy_today_sensor_);
  LOG_SENSOR("", "DC1 voltage", this->dc1_voltage_sensor_);
  LOG_SENSOR("", "DC2 voltage", this->dc2_voltage_sensor_);
  LOG_SENSOR("", "DC1 current", this->dc1_current_sensor_);
  LOG_SENSOR("", "DC2 current", this->dc2_current_sensor_);
  LOG_SENSOR("", "AC current", this->ac_current_sensor_);
  LOG_SENSOR("", "AC voltage", this->ac_voltage_sensor_);
  LOG_SENSOR("", "AC frequency", this->ac_frequency_sensor_);
  LOG_SENSOR("", "AC power", this->ac_power_sensor_);
  LOG_SENSOR("", "Energy total", this->energy_total_sensor_);
  LOG_SENSOR("", "Runtime total", this->runtime_total_sensor_);
  LOG_SENSOR("", "Mode", this->mode_sensor_);
  LOG_SENSOR("", "Error bits", this->error_bits_sensor_);
  LOG_SENSOR("", "Grid voltage fault", this->grid_voltage_fault_sensor_);
  LOG_SENSOR("", "Grid frequency fault", this->grid_frequency_fault_sensor_);
  LOG_SENSOR("", "DC injection fault", this->dc_injection_fault_sensor_);
  LOG_SENSOR("", "Temperature fault", this->temperature_fault_sensor_);
  LOG_SENSOR("", "PV1 voltage fault", this->pv1_voltage_fault_sensor_);
  LOG_SENSOR("", "PV2 voltage fault", this->pv2_voltage_fault_sensor_);
  LOG_SENSOR("", "GFC fault", this->gfc_fault_sensor_);
  LOG_TEXT_SENSOR("  ", "Mode name", this->mode_name_text_sensor_);
  LOG_TEXT_SENSOR("  ", "Errors", this->errors_text_sensor_);
}

std::string SolaxX1Mini::error_bits_to_string_(const uint32_t mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < ERRORS_SIZE; i++) {
      if (mask & (1 << i)) {
        values.append(ERRORS[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

}  // namespace solax_x1_mini
}  // namespace esphome
