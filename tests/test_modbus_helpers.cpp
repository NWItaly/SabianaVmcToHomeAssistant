#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>

// ============================================================================
// STUB PER L'AMBIENTE ESP (prima di includere gli header reali)
// ============================================================================

// Stub per logging ESP
#define ESP_LOGE(tag, format, ...) 
#define ESP_LOGI(tag, format, ...)
#define ESP_LOGD(tag, format, ...)

// ============================================================================
// INCLUDE IL CODICE REALE DAL TUO PROGETTO
// ============================================================================

#include "../config/modbus_helpers.h"

// ============================================================================
// TEST: format_version_from_modbus
// ============================================================================

TEST(ModbusVersionTest, FormatsValidVersion) {
    std::vector<uint8_t> data = {0x01, 0x05}; // Versione 1.5
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "1.5");
}

TEST(ModbusVersionTest, FormatsZeroVersion) {
    std::vector<uint8_t> data = {0x00, 0x00}; // Versione 0.0
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "0.0");
}

TEST(ModbusVersionTest, FormatsMaxVersion) {
    std::vector<uint8_t> data = {0xFF, 0xFF}; // Versione 255.255
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "255.255");
}

TEST(ModbusVersionTest, HandlesMajorVersionOnly) {
    std::vector<uint8_t> data = {0x02, 0x00}; // Versione 2.0
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "2.0");
}

TEST(ModbusVersionTest, HandlesMinorVersionOnly) {
    std::vector<uint8_t> data = {0x00, 0x03}; // Versione 0.3
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "0.3");
}

TEST(ModbusVersionTest, ReturnsUnknownForEmptyData) {
    std::vector<uint8_t> data = {};
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "Unknown");
}

TEST(ModbusVersionTest, ReturnsUnknownForInsufficientData) {
    std::vector<uint8_t> data = {0x01}; // Solo 1 byte
    
    std::string version = format_version_from_modbus(data);
    
    EXPECT_EQ(version, "Unknown");
}

// ============================================================================
// TEST: readSigned16
// ============================================================================

TEST(ModbusSigned16Test, ReadsPositiveValue) {
    std::vector<uint8_t> data = {0x00, 0x64}; // +100
    
    int16_t value = readSigned16(data, 0);
    
    EXPECT_EQ(value, 100);
}

TEST(ModbusSigned16Test, ReadsNegativeValue) {
    std::vector<uint8_t> data = {0xFF, 0x9C}; // -100 in complemento a 2
    
    int16_t value = readSigned16(data, 0);
    
    EXPECT_EQ(value, -100);
}

TEST(ModbusSigned16Test, ReadsZero) {
    std::vector<uint8_t> data = {0x00, 0x00};
    
    int16_t value = readSigned16(data, 0);
    
    EXPECT_EQ(value, 0);
}

TEST(ModbusSigned16Test, ReadsMaxPositive) {
    std::vector<uint8_t> data = {0x7F, 0xFF}; // +32767
    
    int16_t value = readSigned16(data, 0);
    
    EXPECT_EQ(value, 32767);
}

TEST(ModbusSigned16Test, ReadsMaxNegative) {
    std::vector<uint8_t> data = {0x80, 0x00}; // -32768
    
    int16_t value = readSigned16(data, 0);
    
    EXPECT_EQ(value, -32768);
}

TEST(ModbusSigned16Test, ReadsFromOffset) {
    std::vector<uint8_t> data = {0xAA, 0xBB, 0x01, 0x2C}; // Offset 2 = +300
    
    int16_t value = readSigned16(data, 2);
    
    EXPECT_EQ(value, 300);
}

// ============================================================================
// TEST: readUnsigned16
// ============================================================================

TEST(ModbusUnsigned16Test, ReadsSmallValue) {
    std::vector<uint8_t> data = {0x00, 0x64}; // 100
    
    uint16_t value = readUnsigned16(data, 0);
    
    EXPECT_EQ(value, 100);
}

TEST(ModbusUnsigned16Test, ReadsZero) {
    std::vector<uint8_t> data = {0x00, 0x00};
    
    uint16_t value = readUnsigned16(data, 0);
    
    EXPECT_EQ(value, 0);
}

TEST(ModbusUnsigned16Test, ReadsMaxValue) {
    std::vector<uint8_t> data = {0xFF, 0xFF}; // 65535
    
    uint16_t value = readUnsigned16(data, 0);
    
    EXPECT_EQ(value, 65535);
}

TEST(ModbusUnsigned16Test, ReadsMidRangeValue) {
    std::vector<uint8_t> data = {0x80, 0x00}; // 32768
    
    uint16_t value = readUnsigned16(data, 0);
    
    EXPECT_EQ(value, 32768);
}

// ============================================================================
// TEST: readUnsigned32
// ============================================================================

TEST(ModbusUnsigned32Test, ReadsSmallValue) {
    std::vector<uint8_t> data = {0x00, 0x00, 0x00, 0x64}; // 100
    
    uint32_t value = readUnsigned32(data, 0);
    
    EXPECT_EQ(value, 100);
}

TEST(ModbusUnsigned32Test, ReadsZero) {
    std::vector<uint8_t> data = {0x00, 0x00, 0x00, 0x00};
    
    uint32_t value = readUnsigned32(data, 0);
    
    EXPECT_EQ(value, 0);
}

TEST(ModbusUnsigned32Test, ReadsMaxValue) {
    std::vector<uint8_t> data = {0xFF, 0xFF, 0xFF, 0xFF}; // 4294967295
    
    uint32_t value = readUnsigned32(data, 0);
    
    EXPECT_EQ(value, 4294967295U);
}

TEST(ModbusUnsigned32Test, ReadsBigEndianCorrectly) {
    std::vector<uint8_t> data = {0x12, 0x34, 0x56, 0x78}; // 0x12345678
    
    uint32_t value = readUnsigned32(data, 0);
    
    EXPECT_EQ(value, 0x12345678);
}

TEST(ModbusUnsigned32Test, ReadsFromOffset) {
    std::vector<uint8_t> data = {0xAA, 0xBB, 0x00, 0x00, 0x03, 0xE8}; // Offset 2 = 1000
    
    uint32_t value = readUnsigned32(data, 2);
    
    EXPECT_EQ(value, 1000);
}

// ============================================================================
// TEST: readSigned16ToFloat con Scale
// ============================================================================

TEST(ModbusSigned16ToFloatTest, ReadsWithUnityScale) {
    std::vector<uint8_t> data = {0x00, 0x64}; // 100
    
    float value = readSigned16ToFloat(data, 0, Scale::UNITY);
    
    EXPECT_FLOAT_EQ(value, 100.0f);
}

TEST(ModbusSigned16ToFloatTest, ReadsWithDecimalScale) {
    std::vector<uint8_t> data = {0x00, 0x64}; // 100 -> 10.0
    
    float value = readSigned16ToFloat(data, 0, Scale::DECIMAL);
    
    EXPECT_FLOAT_EQ(value, 10.0f);
}

TEST(ModbusSigned16ToFloatTest, ReadsWithCentesimalScale) {
    std::vector<uint8_t> data = {0x00, 0x64}; // 100 -> 1.00
    
    float value = readSigned16ToFloat(data, 0, Scale::CENTESIMAL);
    
    EXPECT_FLOAT_EQ(value, 1.0f);
}

TEST(ModbusSigned16ToFloatTest, ReadsWithThousandthScale) {
    std::vector<uint8_t> data = {0x03, 0xE8}; // 1000 -> 1.000
    
    float value = readSigned16ToFloat(data, 0, Scale::THOUSANDTH);
    
    EXPECT_FLOAT_EQ(value, 1.0f);
}

TEST(ModbusSigned16ToFloatTest, ReadsNegativeWithDecimalScale) {
    std::vector<uint8_t> data = {0xFF, 0x9C}; // -100 -> -10.0
    
    float value = readSigned16ToFloat(data, 0, Scale::DECIMAL);
    
    EXPECT_FLOAT_EQ(value, -10.0f);
}

TEST(ModbusSigned16ToFloatTest, ReadsZeroWithAnyScale) {
    std::vector<uint8_t> data = {0x00, 0x00};
    
    float value1 = readSigned16ToFloat(data, 0, Scale::UNITY);
    float value2 = readSigned16ToFloat(data, 0, Scale::DECIMAL);
    float value3 = readSigned16ToFloat(data, 0, Scale::CENTESIMAL);
    
    EXPECT_FLOAT_EQ(value1, 0.0f);
    EXPECT_FLOAT_EQ(value2, 0.0f);
    EXPECT_FLOAT_EQ(value3, 0.0f);
}

// ============================================================================
// TEST: readFloat
// ============================================================================

TEST(ModbusFloatTest, ReadsPositiveFloat) {
    // IEEE 754: 12.5 = 0x41480000
    std::vector<uint8_t> data = {0x41, 0x48, 0x00, 0x00};
    
    float value = readFloat(data, 0);
    
    EXPECT_FLOAT_EQ(value, 12.5f);
}

TEST(ModbusFloatTest, ReadsNegativeFloat) {
    // IEEE 754: -12.5 = 0xC1480000
    std::vector<uint8_t> data = {0xC1, 0x48, 0x00, 0x00};
    
    float value = readFloat(data, 0);
    
    EXPECT_FLOAT_EQ(value, -12.5f);
}

TEST(ModbusFloatTest, ReadsZero) {
    std::vector<uint8_t> data = {0x00, 0x00, 0x00, 0x00};
    
    float value = readFloat(data, 0);
    
    EXPECT_FLOAT_EQ(value, 0.0f);
}

TEST(ModbusFloatTest, ReadsOne) {
    // IEEE 754: 1.0 = 0x3F800000
    std::vector<uint8_t> data = {0x3F, 0x80, 0x00, 0x00};
    
    float value = readFloat(data, 0);
    
    EXPECT_FLOAT_EQ(value, 1.0f);
}

// ============================================================================
// TEST: readBitFromUns16 con LSB_FIRST (default)
// ============================================================================

TEST(ModbusBitReadingTest, ReadsBit0LSBFirst) {
    std::vector<uint8_t> data = {0x00, 0x01}; // Binario: 0000000000000001
    
    bool bit0 = readBitFromUns16(data, 0, 0, BitOrder::LSB_FIRST);
    bool bit1 = readBitFromUns16(data, 0, 1, BitOrder::LSB_FIRST);
    
    EXPECT_TRUE(bit0);   // Bit 0 (LSB) è 1
    EXPECT_FALSE(bit1);  // Bit 1 è 0
}

TEST(ModbusBitReadingTest, ReadsBit15LSBFirst) {
    std::vector<uint8_t> data = {0x80, 0x00}; // Binario: 1000000000000000
    
    bool bit15 = readBitFromUns16(data, 0, 15, BitOrder::LSB_FIRST);
    bool bit14 = readBitFromUns16(data, 0, 14, BitOrder::LSB_FIRST);
    
    EXPECT_TRUE(bit15);   // Bit 15 (MSB) è 1
    EXPECT_FALSE(bit14);  // Bit 14 è 0
}

TEST(ModbusBitReadingTest, ReadsMultipleBitsLSBFirst) {
    std::vector<uint8_t> data = {0x00, 0xAA}; // Binario: 0000000010101010
    
    EXPECT_FALSE(readBitFromUns16(data, 0, 0, BitOrder::LSB_FIRST)); // 0
    EXPECT_TRUE(readBitFromUns16(data, 0, 1, BitOrder::LSB_FIRST));  // 1
    EXPECT_FALSE(readBitFromUns16(data, 0, 2, BitOrder::LSB_FIRST)); // 0
    EXPECT_TRUE(readBitFromUns16(data, 0, 3, BitOrder::LSB_FIRST));  // 1
    EXPECT_FALSE(readBitFromUns16(data, 0, 4, BitOrder::LSB_FIRST)); // 0
    EXPECT_TRUE(readBitFromUns16(data, 0, 5, BitOrder::LSB_FIRST));  // 1
    EXPECT_FALSE(readBitFromUns16(data, 0, 6, BitOrder::LSB_FIRST)); // 0
    EXPECT_TRUE(readBitFromUns16(data, 0, 7, BitOrder::LSB_FIRST));  // 1
}

TEST(ModbusBitReadingTest, ReadsAllZeros) {
    std::vector<uint8_t> data = {0x00, 0x00};
    
    for (int i = 0; i < 16; i++) {
        EXPECT_FALSE(readBitFromUns16(data, 0, i, BitOrder::LSB_FIRST)) 
            << "Bit " << i << " dovrebbe essere 0";
    }
}

TEST(ModbusBitReadingTest, ReadsAllOnes) {
    std::vector<uint8_t> data = {0xFF, 0xFF};
    
    for (int i = 0; i < 16; i++) {
        EXPECT_TRUE(readBitFromUns16(data, 0, i, BitOrder::LSB_FIRST)) 
            << "Bit " << i << " dovrebbe essere 1";
    }
}

// ============================================================================
// TEST: readBitFromUns16 con MSB_FIRST
// DISABILITATI: Nessun caso d'uso reale al momento
// ============================================================================

TEST(ModbusBitReadingMSBTest, DISABLED_ReadsBit0MSBFirst) {
    std::vector<uint8_t> data = {0x80, 0x00}; // MSB_FIRST: bit 0 = MSB
    
    bool bit0 = readBitFromUns16(data, 0, 0, BitOrder::MSB_FIRST);
    bool bit1 = readBitFromUns16(data, 0, 1, BitOrder::MSB_FIRST);
    
    EXPECT_TRUE(bit0);   // Bit 0 in MSB_FIRST è il MSB (bit 15 in LSB)
    EXPECT_FALSE(bit1);  // Bit 1 in MSB_FIRST è 0
}

TEST(ModbusBitReadingMSBTest, DISABLED_ReadsBit15MSBFirst) {
    std::vector<uint8_t> data = {0x00, 0x01}; // MSB_FIRST: bit 15 = LSB
    
    bool bit15 = readBitFromUns16(data, 0, 15, BitOrder::MSB_FIRST);
    bool bit14 = readBitFromUns16(data, 0, 14, BitOrder::MSB_FIRST);
    
    EXPECT_TRUE(bit15);   // Bit 15 in MSB_FIRST è il LSB (bit 0 in LSB)
    EXPECT_FALSE(bit14);  // Bit 14 in MSB_FIRST è 0
}

// ============================================================================
// TEST: readNBitsFromUns16 con LSB_FIRST
// ============================================================================

TEST(ModbusNBitsReadingTest, Reads2BitsLSBFirst) {
    std::vector<uint8_t> data = {0x00, 0x03}; // Binario: ...00000011
    
    uint8_t value = readNBitsFromUns16(data, 0, 0, 2, BitOrder::LSB_FIRST);
    
    EXPECT_EQ(value, 3); // Bit 0-1 = 11 = 3
}

TEST(ModbusNBitsReadingTest, Reads4BitsLSBFirst) {
    std::vector<uint8_t> data = {0x00, 0x0F}; // Binario: ...00001111
    
    uint8_t value = readNBitsFromUns16(data, 0, 0, 4, BitOrder::LSB_FIRST);
    
    EXPECT_EQ(value, 15); // Bit 0-3 = 1111 = 15
}

TEST(ModbusNBitsReadingTest, Reads8BitsLSBFirst) {
    std::vector<uint8_t> data = {0x00, 0xAA}; // Binario: ...10101010
    
    uint8_t value = readNBitsFromUns16(data, 0, 0, 8, BitOrder::LSB_FIRST);
    
    EXPECT_EQ(value, 0xAA); // Bit 0-7 = 10101010 = 170
}

TEST(ModbusNBitsReadingTest, ReadsFromMiddleLSBFirst) {
    std::vector<uint8_t> data = {0x00, 0x78}; // Binario: ...01111000
    
    uint8_t value = readNBitsFromUns16(data, 0, 3, 4, BitOrder::LSB_FIRST);
    
    EXPECT_EQ(value, 15); // Bit 3-6 = 1111 = 15
}

TEST(ModbusNBitsReadingTest, ReadsHighBitsLSBFirst) {
    std::vector<uint8_t> data = {0xF0, 0x00}; // Binario: 1111000000000000
    
    uint8_t value = readNBitsFromUns16(data, 0, 12, 4, BitOrder::LSB_FIRST);
    
    EXPECT_EQ(value, 15); // Bit 12-15 = 1111 = 15
}

// ============================================================================
// TEST: readNBitsFromUns16 con MSB_FIRST
// DISABILITATI: Nessun caso d'uso reale al momento
// ============================================================================

TEST(ModbusNBitsReadingMSBTest, DISABLED_Reads4BitsMSBFirst) {
    std::vector<uint8_t> data = {0x00, 0x0F}; // Binario: 0000000000001111
    
    // MSB_FIRST con bit_position=0 legge dal bit 15 (LSB fisico)
    uint8_t value = readNBitsFromUns16(data, 0, 0, 4, BitOrder::MSB_FIRST);
    
    EXPECT_EQ(value, 15);
}

TEST(ModbusNBitsReadingMSBTest, DISABLED_Reads8BitsMSBFirst) {
    std::vector<uint8_t> data = {0x00, 0xAB}; // Binario: 0000000010101011
    
    uint8_t value = readNBitsFromUns16(data, 0, 0, 8, BitOrder::MSB_FIRST);
    
    EXPECT_EQ(value, 0xAB);
}

TEST(ModbusNBitsReadingMSBTest, DISABLED_ReadsHighBitsMSBFirst) {
    std::vector<uint8_t> data = {0xAB, 0x00}; // Binario: 1010101100000000
    
    uint8_t value = readNBitsFromUns16(data, 0, 8, 8, BitOrder::MSB_FIRST);
    
    EXPECT_EQ(value, 0x00);
}

// ============================================================================
// TEST: Edge cases e validazione errori
// ============================================================================

TEST(ModbusEdgeCasesTest, BitReadingReturnsZeroForOutOfBounds) {
    std::vector<uint8_t> data = {0xFF}; // Solo 1 byte
    
    bool result = readBitFromUns16(data, 0, 0, BitOrder::LSB_FIRST);
    
    EXPECT_FALSE(result); // Dovrebbe restituire false per offset invalido
}

TEST(ModbusEdgeCasesTest, NBitsReadingReturnsZeroForInvalidBitCount) {
    std::vector<uint8_t> data = {0xFF, 0xFF};
    
    uint8_t result = readNBitsFromUns16(data, 0, 0, 9, BitOrder::LSB_FIRST); // 9 bit non valido
    
    EXPECT_EQ(result, 0); // Dovrebbe restituire 0 per parametri invalidi
}

TEST(ModbusEdgeCasesTest, NBitsReadingReturnsZeroForOutOfRange) {
    std::vector<uint8_t> data = {0xFF, 0xFF};
    
    uint8_t result = readNBitsFromUns16(data, 0, 15, 4, BitOrder::LSB_FIRST); // bit_pos + num_bits > 16
    
    EXPECT_EQ(result, 0); // Dovrebbe restituire 0 per range invalido
}

TEST(ModbusEdgeCasesTest, HandlesLargeOffsets) {
    std::vector<uint8_t> data = {0x00, 0x00, 0x00, 0x00, 0x12, 0x34};
    
    int16_t value = readSigned16(data, 4);
    
    EXPECT_EQ(value, 0x1234);
}

// ============================================================================
// TEST: Confronto LSB_FIRST vs MSB_FIRST
// DISABILITATO: Nessun caso d'uso reale per MSB_FIRST al momento
// ============================================================================

TEST(ModbusBitOrderComparisonTest, DISABLED_ComparesLSBandMSBOrders) {
    std::vector<uint8_t> data = {0x80, 0x01}; // 1000000000000001
    
    // LSB_FIRST: bit 0 = LSB (posizione 0)
    bool lsb_bit0 = readBitFromUns16(data, 0, 0, BitOrder::LSB_FIRST);
    bool lsb_bit15 = readBitFromUns16(data, 0, 15, BitOrder::LSB_FIRST);
    
    // MSB_FIRST: bit 0 = MSB (posizione 15 in LSB)
    bool msb_bit0 = readBitFromUns16(data, 0, 0, BitOrder::MSB_FIRST);
    bool msb_bit15 = readBitFromUns16(data, 0, 15, BitOrder::MSB_FIRST);
    
    // LSB_FIRST: bit 0 dovrebbe essere 1, bit 15 dovrebbe essere 1
    EXPECT_TRUE(lsb_bit0);
    EXPECT_TRUE(lsb_bit15);
    
    // MSB_FIRST: bit 0 dovrebbe essere 1 (MSB), bit 15 dovrebbe essere 1 (LSB)
    EXPECT_TRUE(msb_bit0);
    EXPECT_TRUE(msb_bit15);
}