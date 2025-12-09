#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

// ============================================================================
// STUB PER L'AMBIENTE ESP (prima di includere gli header reali)
// ============================================================================

// Stub per logging ESP
#define ESP_LOGE(tag, format, ...)
#define ESP_LOGI(tag, format, ...)
#define ESP_LOGW(tag, format, ...)
#define ESP_LOGD(tag, format, ...)

// Stub per delay
void delay(int ms) {}

// Mock del ModbusController e ModbusCommandItem
namespace modbus_controller
{
    // Mock di ModbusCommandItem che memorizza address, count e values
    class ModbusCommandItem
    {
    public:
        uint16_t address;
        std::vector<uint16_t> values;
        bool is_multiple;

        ModbusCommandItem() : address(0), is_multiple(false) {}
        ModbusCommandItem(uint16_t addr, const std::vector<uint16_t> &vals)
            : address(addr), values(vals), is_multiple(true) {}
        virtual ~ModbusCommandItem() = default;

        static std::shared_ptr<ModbusCommandItem> create_write_multiple_command(
            class ModbusController *controller,
            uint16_t address,
            uint16_t count,
            const std::vector<uint16_t> &values)
        {
            return std::make_shared<ModbusCommandItem>(address, values);
        }
    };

    class ModbusController
    {
    public:
        virtual ~ModbusController() = default;
        virtual void queue_command(std::shared_ptr<ModbusCommandItem> command) = 0;
    };
}

// Mock avanzato che cattura tutti i comandi
class MockModbusController : public modbus_controller::ModbusController
{
public:
    std::vector<std::pair<uint16_t, uint16_t>> written_values; // (address, value) - espanso dai batch

    void queue_command(std::shared_ptr<modbus_controller::ModbusCommandItem> command) override
    {
        // Espandi il comando batch in singole coppie (address, value)
        for (size_t i = 0; i < command->values.size(); i++)
        {
            written_values.push_back({command->address + i, command->values[i]});
        }
    }

    // Helper per verificare se un comando specifico è stato inviato
    bool was_written(uint16_t address, uint16_t value) const
    {
        for (const auto &pair : written_values)
        {
            if (pair.first == address && pair.second == value)
            {
                return true;
            }
        }
        return false;
    }

    // Helper per contare quante volte un indirizzo è stato scritto
    int count_writes_to_address(uint16_t address) const
    {
        int count = 0;
        for (const auto &pair : written_values)
        {
            if (pair.first == address)
            {
                count++;
            }
        }
        return count;
    }
};

// ============================================================================
// INCLUDE IL CODICE REALE DAL TUO PROGETTO
// ============================================================================

#include "../config/modbus_helpers.h"
#include "../config/Blk4_UserTimerProgram.h"

// ============================================================================
// TEST SUITE
// ============================================================================

class WriteScheduleTest : public ::testing::Test
{
protected:
    MockModbusController *controller;
    std::string valid_day_json;

    void SetUp() override
    {
        controller = new MockModbusController();
        valid_day_json = R"({"d":1,"sb":2,"i":[{"t":"06:00","s":3},{"t":"08:00","s":0},{"t":"17:00","s":2},{"t":"21:00","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0}]})";
    }

    void TearDown() override
    {
        delete controller;
    }

    std::vector<std::string> create_valid_week()
    {
        return std::vector<std::string>(7, valid_day_json);
    }
};

// ============================================================================
// TEST: Validazione helper functions
// ============================================================================

TEST(ValidationFunctionsTest, ValidatesTimeCorrectly)
{
    EXPECT_TRUE(is_valid_time(0x0000));  // 00:00
    EXPECT_TRUE(is_valid_time(0x173B));  // 23:59
    EXPECT_TRUE(is_valid_time(0x0C1E));  // 12:30
    EXPECT_FALSE(is_valid_time(0x183C)); // 24:60 (invalido)
    EXPECT_TRUE(is_valid_time(0x1700));  // 23:00 valido
    EXPECT_FALSE(is_valid_time(0x1A00)); // 26:00 (invalido)
}

TEST(ValidationFunctionsTest, ValidatesSpeedCorrectly)
{
    EXPECT_TRUE(is_valid_speed(0));
    EXPECT_TRUE(is_valid_speed(1));
    EXPECT_TRUE(is_valid_speed(2));
    EXPECT_TRUE(is_valid_speed(3));
    EXPECT_TRUE(is_valid_speed(4));
    EXPECT_FALSE(is_valid_speed(5));
    EXPECT_FALSE(is_valid_speed(10));
    EXPECT_TRUE(is_valid_speed(255));
    EXPECT_FALSE(is_valid_speed(254));
}

// ============================================================================
// TEST: JSON parsing
// ============================================================================

TEST(JsonScheduleParsingTest, ParsesValidJsonCorrectly)
{
    std::string json = R"({"d":1,"sb":2,"i":[{"t":"06:00","s":3},{"t":"08:00","s":0},{"t":"17:00","s":2},{"t":"21:00","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0}]})";
    std::vector<uint16_t> time_regs;
    std::vector<uint16_t> speed_regs;

    bool result = json_to_schedule_registers(json, time_regs, speed_regs);

    EXPECT_TRUE(result);
    EXPECT_EQ(time_regs.size(), 8);
    EXPECT_EQ(speed_regs.size(), 9); // sb + 8 speeds

    // Verifica speed_before
    EXPECT_EQ(speed_regs[0], 2);

    // Verifica primo intervallo: 06:00 -> 0x0600
    EXPECT_EQ(time_regs[0], 0x0600);
    EXPECT_EQ(speed_regs[1], 3);

    // Verifica secondo intervallo: 08:00 -> 0x0800
    EXPECT_EQ(time_regs[1], 0x0800);
    EXPECT_EQ(speed_regs[2], 0);
}

TEST(JsonScheduleParsingTest, RejectsMissingSpeedBefore)
{
    std::string json = R"({"d":1,"i":[{"t":"06:00","s":3}]})";
    std::vector<uint16_t> time_regs;
    std::vector<uint16_t> speed_regs;

    bool result = json_to_schedule_registers(json, time_regs, speed_regs);

    EXPECT_FALSE(result);
}

TEST(JsonScheduleParsingTest, RejectsInvalidSpeedBefore)
{
    std::string json = R"({"d":1,"sb":10,"i":[{"t":"06:00","s":3}]})";
    std::vector<uint16_t> time_regs;
    std::vector<uint16_t> speed_regs;

    bool result = json_to_schedule_registers(json, time_regs, speed_regs);

    EXPECT_FALSE(result);
}

TEST(JsonScheduleParsingTest, RejectsInvalidTime)
{
    std::string json = R"({"d":1,"sb":2,"i":[{"t":"25:00","s":3},{"t":"08:00","s":0},{"t":"17:00","s":2},{"t":"21:00","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0}]})";
    std::vector<uint16_t> time_regs;
    std::vector<uint16_t> speed_regs;

    bool result = json_to_schedule_registers(json, time_regs, speed_regs);

    EXPECT_FALSE(result);
}

TEST(JsonScheduleParsingTest, RejectsInvalidSpeed)
{
    std::string json = R"({"d":1,"sb":2,"i":[{"t":"06:00","s":10},{"t":"08:00","s":0},{"t":"17:00","s":2},{"t":"21:00","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0}]})";
    std::vector<uint16_t> time_regs;
    std::vector<uint16_t> speed_regs;

    bool result = json_to_schedule_registers(json, time_regs, speed_regs);

    EXPECT_FALSE(result);
}

// ============================================================================
// TEST: write_complete_schedule
// ============================================================================

TEST_F(WriteScheduleTest, RejectsLessThan7Days)
{
    std::vector<std::string> days = {valid_day_json, valid_day_json};

    bool result = write_complete_schedule(controller, 1000, days);

    EXPECT_FALSE(result);
}

TEST_F(WriteScheduleTest, RejectsMoreThan7Days)
{
    auto days = create_valid_week();
    days.push_back(valid_day_json); // 8 giorni

    bool result = write_complete_schedule(controller, 1000, days);

    EXPECT_FALSE(result);
}

TEST_F(WriteScheduleTest, WritesExactly119RegistersForValidWeek)
{
    auto days = create_valid_week();

    bool result = write_complete_schedule(controller, 1000, days);

    EXPECT_TRUE(result);
    // 7 giorni * (8 time + 9 speed) = 119 registri
    EXPECT_EQ(controller->written_values.size(), 119);
}

TEST_F(WriteScheduleTest, WritesCorrectAddressesForDay0)
{
    auto days = create_valid_week();
    uint16_t base = 1000;

    write_complete_schedule(controller, base, days);

    // Time registers: 1000-1007
    for (int i = 0; i < 8; i++)
    {
        EXPECT_EQ(controller->count_writes_to_address(base + i), 1)
            << "Address " << (base + i) << " should be written once";
    }

    // Speed registers: 1056-1064 (base + 56 + 0*9)
    for (int i = 0; i < 9; i++)
    {
        EXPECT_EQ(controller->count_writes_to_address(base + 56 + i), 1)
            << "Address " << (base + 56 + i) << " should be written once";
    }
}

TEST_F(WriteScheduleTest, WritesCorrectAddressesForDay6)
{
    auto days = create_valid_week();
    uint16_t base = 1000;

    write_complete_schedule(controller, base, days);

    // Time: base + 6*8 = 1048-1055
    for (int i = 0; i < 8; i++)
    {
        EXPECT_EQ(controller->count_writes_to_address(1048 + i), 1)
            << "Address " << (1048 + i) << " should be written once";
    }

    // Speed: base + 56 + 6*9 = 1110-1118
    for (int i = 0; i < 9; i++)
    {
        EXPECT_EQ(controller->count_writes_to_address(1110 + i), 1)
            << "Address " << (1110 + i) << " should be written once";
    }
}

TEST_F(WriteScheduleTest, VerifiesCorrectValuesWrittenForFirstInterval)
{
    auto days = create_valid_week();
    uint16_t base = 1000;

    write_complete_schedule(controller, base, days);

    // Primo intervallo del primo giorno: 06:00 -> 0x0600, speed 3
    EXPECT_TRUE(controller->was_written(base, 0x0600))
        << "First time register should contain 06:00";
    EXPECT_TRUE(controller->was_written(base + 56, 2))
        << "Speed_before should be 2";
    EXPECT_TRUE(controller->was_written(base + 56 + 1, 3))
        << "First speed should be 3";
}

TEST_F(WriteScheduleTest, StopsOnInvalidJsonAtDay3)
{
    auto days = create_valid_week();
    days[2] = R"({"d":3,"sb":10,"i":[]})"; // speed_before invalido

    bool result = write_complete_schedule(controller, 1000, days);

    EXPECT_FALSE(result);
    // Dovrebbe scrivere solo i primi 2 giorni (2 * 17 = 34 registri)
    EXPECT_EQ(controller->written_values.size(), 34);
}

TEST_F(WriteScheduleTest, WritesTimeRegistersBeforeSpeedRegisters)
{
    auto days = create_valid_week();
    uint16_t base = 1000;

    write_complete_schedule(controller, base, days);

    // Verifica che per il primo giorno, i time registers (1000-1007)
    // vengano scritti PRIMA degli speed registers (1056-1064)
    size_t last_time_index = 0;
    size_t first_speed_index = controller->written_values.size();

    for (size_t i = 0; i < controller->written_values.size(); i++)
    {
        uint16_t addr = controller->written_values[i].first;

        // Trova l'ultimo time register del primo giorno
        if (addr >= base && addr < base + 8)
        {
            last_time_index = i;
        }

        // Trova il primo speed register del primo giorno
        if (addr >= base + 56 && addr < base + 56 + 9)
        {
            if (i < first_speed_index)
            {
                first_speed_index = i;
            }
        }
    }

    EXPECT_LT(last_time_index, first_speed_index)
        << "All time registers should be written before speed registers";
}

TEST_F(WriteScheduleTest, HandlesEdgeCaseTimeValues)
{
    std::string edge_json = R"({"d":1,"sb":0,"i":[{"t":"00:00","s":0},{"t":"23:59","s":4},{"t":"12:30","s":2},{"t":"06:15","s":1},{"t":"18:45","s":3},{"t":"00:01","s":0},{"t":"23:58","s":4},{"t":"11:11","s":2}]})";
    auto days = std::vector<std::string>(7, edge_json);

    bool result = write_complete_schedule(controller, 1000, days);

    EXPECT_TRUE(result);
    EXPECT_TRUE(controller->was_written(1000, 0x0000)) << "00:00 should be written";
    EXPECT_TRUE(controller->was_written(1001, 0x173B)) << "23:59 should be written";
    EXPECT_TRUE(controller->was_written(1002, 0x0C1E)) << "12:30 should be written";
}

TEST_F(WriteScheduleTest, HandlesSpeed255)
{
    std::string json_255 = R"({"d":1,"sb":255,"i":[{"t":"06:00","s":255},{"t":"08:00","s":0},{"t":"17:00","s":2},{"t":"21:00","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0},{"t":"23:59","s":0}]})";
    auto days = std::vector<std::string>(7, json_255);

    bool result = write_complete_schedule(controller, 1000, days);

    EXPECT_TRUE(result);
    EXPECT_TRUE(controller->was_written(1056, 255)) << "Speed_before should be 255";
    EXPECT_TRUE(controller->was_written(1057, 255)) << "First speed should be 255";
}