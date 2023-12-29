#include "solax_meter_gateway.h"
#include "esphome/core/log.h"

namespace esphome {
namespace solax_meter_gateway {

static const char *const TAG = "solax_meter_gateway";

static const uint8_t REGISTER_HANDSHAKE = 0x0B;
static const uint8_t REGISTER_READ_POWER_16BIT_SINT = 0x0E;
static const uint8_t REGISTER_READ_POWER_32BIT_FLOAT = 0x0C;
static const uint8_t REGISTER_READ_TOTAL_ENERGY = 0x08;
static const uint8_t REGISTER_READ_TOTAL_ENERGY_IMPORT_32BIT_FLOAT = 0x48;
static const uint8_t REGISTER_READ_TOTAL_ENERGY_EXPORT_32BIT_FLOAT = 0x4A;

void SolaxMeterGateway::on_solax_meter_modbus_data(const std::vector<uint8_t> &data) {
  if (this->inactivity_timeout_()) {
    this->publish_state_(this->operation_mode_text_sensor_, "Meter fault");
    this->publish_state_(power_demand_sensor_, NAN);
    ESP_LOGW(TAG, "No power sensor update received since %d seconds. Triggering meter fault for safety reasons",
             this->power_sensor_inactivity_timeout_s_);
    return;
  }

  if (this->emergency_power_off_switch_ != nullptr && this->emergency_power_off_switch_->state) {
    this->publish_state_(this->operation_mode_text_sensor_, "Off");
    this->publish_state_(power_demand_sensor_, 0.0f);
    return;
  }

  if (this->manual_mode_switch_ != nullptr && this->manual_mode_switch_->state) {
    this->publish_state_(this->operation_mode_text_sensor_, "Manual");
    if (this->manual_power_demand_number_ != nullptr && this->manual_power_demand_number_->has_state()) {
      this->power_demand_ = this->manual_power_demand_number_->state;
    } else {
      this->power_demand_ = 0.0f;
    }
  } else {
    this->publish_state_(this->operation_mode_text_sensor_, "Auto");
  }

  uint8_t register_address = data[2];
  switch (register_address) {
    case REGISTER_HANDSHAKE:
      // Request: 0x01 0x03 0x00 0x0B 0x00 0x01 0xF5 0xC8
      //          addr func      reg       bytes*2
      this->send_raw({0x01, 0x03, 0x02, 0x00, 0x00});
      break;

    case REGISTER_READ_POWER_32BIT_FLOAT:
      // Request: 0x01 0x04 0x00 0x0C 0x00 0x02 0xB1 0xC8
      //          addr func      reg       bytes*2
      // this->send_raw({0x01, 0x04, 0x04, 0x45, 0x8e, 0x3c, 0x35});  // -4551.52W
      this->send(this->power_demand_);
      this->publish_state_(power_demand_sensor_, this->power_demand_);
      break;
    case REGISTER_READ_TOTAL_ENERGY_IMPORT_32BIT_FLOAT:
    case REGISTER_READ_TOTAL_ENERGY_EXPORT_32BIT_FLOAT:
      // Request: 0x01 0x04 0x00 0x48 0x00 0x02 0xF1 0xDD
      //          addr func      reg       bytes*2

      // Request: 0x01 0x04 0x00 0x4A 0x00 0x02 0x50 0x1D
      //          addr func      reg       bytes*2
      this->send_raw({0x01, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00});
      break;

    case REGISTER_READ_POWER_16BIT_SINT:
      // Request: 0x01 0x03 0x00 0x0E 0x00 0x01 0xE5 0xC9
      //          addr func      reg       bytes*2
      // this->send_raw({0x01, 0x03, 0x02, 0x00, 0x00});
      this->send((int16_t) this->power_demand_);
      this->publish_state_(power_demand_sensor_, this->power_demand_);
      break;
    case REGISTER_READ_TOTAL_ENERGY:
      // Request: 0x01 0x03 0x00 0x08 0x00 0x04 0xC5 0xCB
      //          addr func      reg       bytes*2
      this->send_raw({0x01, 0x03, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
      break;

    default:
      ESP_LOGW(TAG, "Unhandled register address (0x%02X) with length (%d) requested.", register_address, data[4]);
      ESP_LOGW(TAG, "Your device is probably not supported. Please create an issue here: "
                    "https://github.com/syssi/esphome-solax-x1-mini/issues");
      ESP_LOGW(TAG, "Please provide the following request data: %s",
               format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void SolaxMeterGateway::setup() {
  this->power_sensor_->add_on_state_callback([this](float state) {
    if (std::isnan(state)) {
      ESP_LOGVV(TAG, "Invalid power demand received: NaN");
      return;
    }

    // Skip updates in manual mode
    if (this->manual_mode_switch_ != nullptr && this->manual_mode_switch_->state) {
      return;
    }

    this->power_demand_ = state;
    this->last_power_demand_received_ = millis();
    ESP_LOGVV(TAG, "New power demand received (%.2f). Resetting inactivity timeout (%d)", this->power_demand_,
              this->last_power_demand_received_);
  });
}

void SolaxMeterGateway::dump_config() {
  ESP_LOGCONFIG(TAG, "SolaxMeterGateway:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("  ", "Power Demand", this->power_demand_sensor_);
  LOG_TEXT_SENSOR("  ", "Operation name", this->operation_mode_text_sensor_);
}

void SolaxMeterGateway::update() {}

bool SolaxMeterGateway::inactivity_timeout_() {
  if (this->power_sensor_inactivity_timeout_s_ == 0) {
    return false;
  }

  // Skip the inactivity timeout in manual mode
  if (this->manual_mode_switch_ != nullptr && this->manual_mode_switch_->state) {
    return false;
  }

  return millis() - this->last_power_demand_received_ > (this->power_sensor_inactivity_timeout_s_ * 1000);
}

void SolaxMeterGateway::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void SolaxMeterGateway::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

}  // namespace solax_meter_gateway
}  // namespace esphome
