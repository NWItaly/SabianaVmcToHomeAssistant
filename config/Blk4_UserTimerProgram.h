#pragma once
#include <string>
#include <vector>

inline bool is_valid_time(uint16_t time_value)
{
  uint8_t hour = (time_value >> 8) & 0xFF;
  uint8_t minute = time_value & 0xFF;
  return (hour <= 23 && minute <= 59);
}

inline bool is_valid_speed(uint16_t speed)
{
  return (speed <= 4 || speed == 255); // Speed must be 0-4 or 255
}

// Genera JSON per un singolo giorno (1-7)
inline std::string parse_user_timer_program(const std::vector<uint8_t> &data, int program, int day_number)
{

  int day = day_number - 1; // Converti da 1-7 a 0-6

  std::string json = "{\"d\":" + std::to_string(day_number) + ","; // day

  // Velocità "before interval 1"
  int speed_before_reg = 56 + (day * 9);
  uint16_t speed_before = readUnsigned16(data, speed_before_reg * 2);
  json += "\"sb\":" + std::to_string(speed_before) + ","; // speed before

  json += "\"i\":["; // intervals

  // Loop sugli 8 intervalli
  for (int interval = 0; interval < 8; interval++)
  {
    if (interval > 0)
      json += ",";

    int time_reg = day * 8 + interval;
    uint16_t time_value = readUnsigned16(data, time_reg * 2);

    if (!is_valid_time(time_value))
    {
      ESP_LOGW("vmc_schedule", "Invalid time for program %d at day %d: 0x%04X", program, day + 1, time_value);
      return "{\"error\":\"invalid_data\"}";
    }

    uint8_t hour = (time_value >> 8) & 0xFF;
    uint8_t minute = time_value & 0xFF;

    int speed_reg = speed_before_reg + interval + 1;
    uint16_t speed = readUnsigned16(data, speed_reg * 2);

    if (!is_valid_speed(speed))
    {
      ESP_LOGW("vmc_schedule", "Invalid speed_before for program %d at day %d: %d", program, day + 1, speed);
      return "{\"error\":\"invalid_data\"}";
    }

    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", hour, minute);

    json += "{\"t\":\"" + std::string(time_str) + "\","; // time
    json += "\"s\":" + std::to_string(speed) + "}";      // speed
  }

  json += "]}";

  ESP_LOGD("VMC_Schedule", "Day %d JSON: %s", day_number, json.c_str());

  return json;
}

// Converte il JSON di un giorno in un vettore di registri
// Ritorna true se il parsing ha successo
inline bool json_to_schedule_registers(const std::string &json_str,
                                       std::vector<uint16_t> &time_registers,
                                       std::vector<uint16_t> &speed_registers)
{

  time_registers.clear();
  speed_registers.clear();

  // Trova "sb":X (speed_before)
  size_t sb_pos = json_str.find("\"sb\":");
  if (sb_pos == std::string::npos)
  {
    ESP_LOGE("json_parse", "Cannot find 'sb' in JSON");
    return false;
  }

  // Estrai il valore di speed_before
  size_t sb_value_start = sb_pos + 5;
  size_t sb_value_end = json_str.find_first_of(",}", sb_value_start);
  std::string sb_str = json_str.substr(sb_value_start, sb_value_end - sb_value_start);
  uint16_t speed_before = std::stoi(sb_str);

  if (!is_valid_speed(speed_before))
  {
    ESP_LOGE("json_parse", "Invalid speed_before: %d", speed_before);
    return false;
  }

  speed_registers.push_back(speed_before);

  // Trova gli intervalli "i":[...]
  size_t intervals_start = json_str.find("\"i\":[");
  if (intervals_start == std::string::npos)
  {
    ESP_LOGE("json_parse", "Cannot find intervals array");
    return false;
  }

  // Parsa i singoli intervalli
  size_t pos = intervals_start + 5;
  for (int interval = 0; interval < 8; interval++)
  {
    // Trova "t":"HH:MM"
    size_t t_pos = json_str.find("\"t\":\"", pos);
    if (t_pos == std::string::npos)
    {
      ESP_LOGE("json_parse", "Cannot find time for interval %d", interval);
      return false;
    }

    // Estrai HH:MM
    size_t time_start = t_pos + 5;
    std::string time_str = json_str.substr(time_start, 5); // "HH:MM"

    int hour = std::stoi(time_str.substr(0, 2));
    int minute = std::stoi(time_str.substr(3, 2));

    if (hour > 23 || minute > 59)
    {
      ESP_LOGE("json_parse", "Invalid time %02d:%02d", hour, minute);
      return false;
    }

    // Crea il registro: MSB=hour, LSB=minute
    uint16_t time_reg = (hour << 8) | minute;
    time_registers.push_back(time_reg);

    // Trova "s":X (speed)
    size_t s_pos = json_str.find("\"s\":", t_pos);
    if (s_pos == std::string::npos)
    {
      ESP_LOGE("json_parse", "Cannot find speed for interval %d", interval);
      return false;
    }

    size_t s_value_start = s_pos + 4;
    size_t s_value_end = json_str.find_first_of(",}", s_value_start);
    std::string s_str = json_str.substr(s_value_start, s_value_end - s_value_start);
    uint16_t speed = std::stoi(s_str);

    if (!is_valid_speed(speed))
    {
      ESP_LOGE("json_parse", "Invalid speed: %d", speed);
      return false;
    }

    speed_registers.push_back(speed);

    pos = s_value_end;
  }

  return true;
}

// Scrive un intero programma (tutti i 7 giorni) sui registri Modbus
// json_data contiene un array di 7 JSON (uno per giorno)
inline bool write_complete_schedule(modbus_controller::ModbusController *controller,
                                    uint16_t base_address,
                                    const std::vector<std::string> &days_json)
{
  if (days_json.size() != 7)
  {
    ESP_LOGE("write_schedule", "Expected 7 days, got %d", days_json.size());
    return false;
  }

  // Per ogni giorno
  for (int day = 0; day < 7; day++)
  {
    std::vector<uint16_t> time_regs;
    std::vector<uint16_t> speed_regs;

    // Converti il JSON in registri
    if (!json_to_schedule_registers(days_json[day], time_regs, speed_regs))
    {
      ESP_LOGE("write_schedule", "Failed to parse day %d", day + 1);
      return false;
    }
    else
    {
      ESP_LOGI("write_schedule", "Day %d parsed successfully: %s", day + 1, days_json[day].c_str());
    }

    auto cmdTime = modbus_controller::ModbusCommandItem::create_write_multiple_command(
        controller, base_address + (day * 8), time_regs.size(), time_regs);
    controller->queue_command(cmdTime);
    delay(50);

    auto cmdSpeed = modbus_controller::ModbusCommandItem::create_write_multiple_command(
        controller, base_address + 56 + (day * 9), speed_regs.size(), speed_regs);
    controller->queue_command(cmdSpeed);
    delay(50);
  }

  ESP_LOGI("write_schedule", "Successfully wrote complete schedule");
  return true;
}