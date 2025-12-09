#!/bin/bash
set -e

echo "==================================="
echo "Building ESPHome Schedule Tests"
echo "==================================="

# Compila test per Blk4_UserTimerProgram
echo "Building test_Blk4_UserTimerProgram..."
g++ -std=c++11 \
    test_Blk4_UserTimerProgram.cpp \
    -lgtest \
    -lgmock \
    -lgtest_main \
    -pthread \
    -o test_Blk4_UserTimerProgram

# Compila test per modbus_helpers
echo "Building test_modbus_helpers..."
g++ -std=c++11 \
    test_modbus_helpers.cpp \
    -lgtest \
    -lgtest_main \
    -pthread \
    -o test_modbus_helpers

echo ""
echo "==================================="
echo "Running Tests"
echo "==================================="
echo ""

echo "Running Blk4_UserTimerProgram tests..."
./test_Blk4_UserTimerProgram

echo ""

# Esegui test per modbus_helpers
echo "Running modbus_helpers tests..."
./test_modbus_helpers

echo ""
echo "==================================="
echo "Tests Completed Successfully!"
echo "==================================="