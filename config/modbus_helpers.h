#pragma once
#include <string>
#include <vector>

// Funzione per formattare una versione Modbus da un vettore di byte
// data: vettore contenente i dati Modbus raw
static std::string format_version_from_modbus(const std::vector<uint8_t>& data) {
  if (data.size() >= 2) {
    uint16_t value = (data[0] << 8) | data[1];
    uint8_t major = (value >> 8) & 0xFF;  // High byte
    uint8_t minor = value & 0xFF;         // Low byte
    return std::string(std::to_string(major) + "." + std::to_string(minor));
  }
  return std::string("Unknown");
}

// Funzione per leggere un valore signed 16-bit da un vettore di byte
// data: vettore contenente i dati Modbus raw 
// offset: posizione di partenza nel vettore (in byte)
auto readSigned16 = [](const std::vector<uint8_t>& data, unsigned int offset) -> int16_t {
    int16_t raw = (int16_t)((data[offset] << 8) | data[offset + 1]);
    return raw;
};

// Funzione per leggere un valore unsigned 16-bit da un vettore di byte
// data: vettore contenente i dati Modbus raw 
// offset: posizione di partenza nel vettore (in byte)
auto readUnsigned16 = [](const std::vector<uint8_t>& data, unsigned int offset) -> uint16_t {
    int16_t raw = (int16_t)((data[offset] << 8) | data[offset + 1]);
    return raw;
};

// Funzione per leggere un valore unsigned 32-bit da un vettore di byte
// data: vettore contenente i dati Modbus raw 
// offset: posizione di partenza nel vettore (in byte)
auto readUnsigned32 = [](const std::vector<uint8_t>& data, unsigned int offset) -> uint32_t {
    uint32_t raw = ((uint32_t)data[offset] << 24) | 
                   ((uint32_t)data[offset + 1] << 16) | 
                   ((uint32_t)data[offset + 2] << 8) | 
                   ((uint32_t)data[offset + 3]);
    return raw;
};

enum class Scale {
    UNITY = 0,     // *1.0
    DECIMAL = 1,   // *0.1
    CENTESIMAL = 2,// *0.01
    THOUSANDTH = 3 // *0.001
};

// Funzione per leggere un valore signed 16-bit e convertirlo in float con scala
// data: vettore contenente i dati Modbus raw
// offset: posizione di partenza nel vettore (in byte)
// scale: scala da applicare al valore (UNITY, DECIMAL, CENTESIMAL, THOUSANDTH)
auto readSigned16ToFloat = [](const std::vector<uint8_t>& data, unsigned int offset, Scale scale) -> float {
    int16_t raw = (int16_t)((data[offset] << 8) | data[offset + 1]);
    float scale_factor = 1.0f;
    for (int i = 0; i < (int)scale; i++) {
        scale_factor *= 0.1f;
    }
    return raw * scale_factor;
};

auto readFloat = [](const std::vector<uint8_t>& data, unsigned int offset) -> float {
    union {
        uint32_t i;
        float f;
    } converter;
    
    converter.i = ((uint32_t)data[offset] << 24) | 
                  ((uint32_t)data[offset + 1] << 16) | 
                  ((uint32_t)data[offset + 2] << 8) | 
                  (uint32_t)data[offset + 3];
    
    return converter.f;
};

// Enum per specificare l'ordine dei bit
enum class BitOrder {
    LSB_FIRST,  // Bit 0 = LSB (Standard)
    MSB_FIRST   // Bit 0 = MSB (Invertito)
};

// Funzione per estrarre un singolo bit da un registro Modbus Uns16
// data: vettore contenente i dati Modbus raw
// offset: posizione di partenza nel vettore (in byte)
// bit_position: posizione del bit da estrarre (0-15, dove 0 è il LSB)
auto readBitFromUns16(const std::vector<uint8_t>& data, unsigned int offset, unsigned int bit_position, BitOrder order = BitOrder::LSB_FIRST) -> bool {
    // Verifica che ci siano almeno 2 byte disponibili dall'offset specificato
    if (offset + 1 >= (int)data.size()) {
        ESP_LOGE("modbus", "Offset (%zu) fuori dai limiti del vettore dati (%zu)", offset, data.size());
        return false;
    }
    
    // Verifica che la posizione del bit sia valida (0-15)
    //bit_position = 15 - bit_position; // Modbus usa big-endian, quindi il primo bit è il MSB
    if (bit_position < 0 || bit_position > 15) {
        ESP_LOGE("modbus", "Posizione bit non valida. Deve essere tra 0 e 15");
        return false;
    }
    
    // Ricostruisce il valore Uns16 dai due byte
    // Modbus usa big-endian, quindi il primo byte è il MSB
    uint16_t register_value = (data[offset] << 8) | data[offset + 1];

    // Calcola la posizione effettiva del bit in base all'ordine
    unsigned int actual_bit_position = (order == BitOrder::MSB_FIRST) ? (15 - bit_position) : bit_position;    
    
    // Estrae il bit specificato usando bit shifting e AND mask
    bool result = (register_value >> actual_bit_position) & 0x01;
    ESP_LOGD("modbus", "Bit %u del registro 0x%04X (%s): %d (pos_effettiva: %u, valore: 0x%04X)", 
             bit_position, offset/2 + 0x100, 
             (order == BitOrder::MSB_FIRST) ? "MSB_FIRST" : "LSB_FIRST",
             result ? 1 : 0, actual_bit_position, register_value);
   
    return result;
}

// Funzione per estrarre N bit da un registro Modbus Uns16
// data: vettore contenente i dati Modbus raw
// offset: posizione di partenza nel vettore (in byte)
// bit_position: posizione del primo bit da estrarre (0-15, dove 0 è il LSB)
// num_bits: numero di bit da leggere (2, 4, 6, o 8)
auto readNBitsFromUns16(const std::vector<uint8_t>& data, unsigned int offset, unsigned int bit_position, unsigned int num_bits, BitOrder order = BitOrder::LSB_FIRST) -> uint8_t {
    // Verifica che ci siano almeno 2 byte disponibili dall'offset specificato
    if (offset + 1 >= data.size()) {
        ESP_LOGE("modbus", "Offset (%u) fuori dai limiti del vettore dati (%zu)", offset, data.size());
        return 0;
    }
    
    // Verifica che il numero di bit sia valido
    if (num_bits < 1 || num_bits > 8 || bit_position + num_bits > 16) {
        ESP_LOGE("modbus", "Parametri non validi: bit_position=%u, num_bits=%u", bit_position, num_bits);
        return 0;
    }
    
    // Verifica che la posizione del bit sia valida (0-15)
    if (bit_position > 15) {
        ESP_LOGE("modbus", "Posizione bit non valida (%u). Deve essere tra 0 e 15", bit_position);
        return 0;
    }
    
    // Ricostruisce il valore Uns16 dai due byte (Modbus big-endian)
    uint16_t register_value = (data[offset] << 8) | data[offset + 1];
    
    // Per Modbus, manteniamo la numerazione standard dei bit (0-15 da LSB a MSB)
    // Non facciamo conversione di endianness sui bit positions
    
    // Crea una maschera per estrarre i bit specificati
    uint16_t mask = ((1 << num_bits) - 1);

    // Calcola la posizione effettiva del bit in base all'ordine
    unsigned int actual_bit_position = (order == BitOrder::MSB_FIRST) ? (15 - bit_position) : bit_position;
        
    // Estrae i bit specificati
    uint8_t result = (register_value >> actual_bit_position) & mask;
    
    ESP_LOGD("modbus", "Registro 0x%04X: valore=0x%04X, bit_pos=%u, num_bits=%u, maschera=0x%04X, risultato=%u", 
             offset/2 + 0x100, register_value, bit_position, num_bits, mask, result);
    
    return result;
}