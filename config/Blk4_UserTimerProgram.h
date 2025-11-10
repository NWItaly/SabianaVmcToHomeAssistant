#pragma once
#include <string>
#include <vector>

// Genera JSON per un singolo giorno (1-7)
inline std::string parse_user_timer_program(const std::vector<uint8_t> &data, int day_number)
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

        uint8_t hour = (time_value >> 8) & 0xFF;
        uint8_t minute = time_value & 0xFF;

        int speed_reg = speed_before_reg + interval + 1;
        uint16_t speed = readUnsigned16(data, speed_reg * 2);

        char time_str[6];
        snprintf(time_str, sizeof(time_str), "%02d:%02d", hour, minute);

        json += "{\"t\":\"" + std::string(time_str) + "\","; // time
        json += "\"s\":" + std::to_string(speed) + "}"; // speed
    }

    json += "]}";

    ESP_LOGD("VMC_Schedule", "Day %d JSON: %s", day_number, json.c_str());

    return json;
}