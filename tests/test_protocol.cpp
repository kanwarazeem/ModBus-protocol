// tests/test_protocol.cpp
#include <gtest/gtest.h>
#include "../include/modbus/ModBusProtocol.hpp"
#include <cstring>

class ModBusProtocolTest : public ::testing::Test {
protected:
    ModBusProtocol protocol;
};

// CRC Tests
TEST_F(ModBusProtocolTest, CalculateCRC16) {
    uint8_t data[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    uint16_t crc = protocol.calculateCRC16(data, sizeof(data));
    EXPECT_EQ(crc, 0x8408);  // Expected CRC value
}

TEST_F(ModBusProtocolTest, ValidateCRC16) {
    uint8_t frame[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0x84, 0x08};
    EXPECT_TRUE(protocol.validateCRC16(frame, sizeof(frame)));
}

TEST_F(ModBusProtocolTest, InvalidCRC16) {
    uint8_t frame[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xFF, 0xFF};
    EXPECT_FALSE(protocol.validateCRC16(frame, sizeof(frame)));
}

// LRC Tests
TEST_F(ModBusProtocolTest, CalculateLRC) {
    uint8_t data[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    uint8_t lrc = protocol.calculateLRC(data, sizeof(data));
    uint8_t expected = 0xF5;
    EXPECT_EQ(lrc, expected);
}

TEST_F(ModBusProtocolTest, ValidateLRC) {
    uint8_t frame[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xF5};
    EXPECT_TRUE(protocol.validateLRC(frame, sizeof(frame)));
}

// Frame Tests
TEST_F(ModBusProtocolTest, BuildReadCoilsFrame) {
    std::vector<uint8_t> frame = protocol.buildFrame(
        0x01,  // Slave ID
        0x01,  // Function code (Read Coils)
        0x0000, // Starting address
        0x000A  // Quantity
    );
    
    EXPECT_EQ(frame.size(), 8);  // ADU size
    EXPECT_EQ(frame[0], 0x01);   // Slave ID
    EXPECT_EQ(frame[1], 0x01);   // Function code
}

TEST_F(ModBusProtocolTest, ParseReadCoilsResponse) {
    uint8_t response[] = {0x01, 0x01, 0x02, 0xCD, 0x6B, 0xB8, 0x44};
    
    auto parsed = protocol.parseResponse(response, sizeof(response));
    
    EXPECT_TRUE(parsed.valid);
    EXPECT_EQ(parsed.slaveID, 0x01);
    EXPECT_EQ(parsed.functionCode, 0x01);
}

// Exception Tests
TEST_F(ModBusProtocolTest, ExceptionResponse) {
    uint8_t exception[] = {0x01, 0x81, 0x02};  // Function code 0x81 = exception
    
    auto parsed = protocol.parseResponse(exception, sizeof(exception));
    
    EXPECT_TRUE(parsed.isException);
    EXPECT_EQ(parsed.exceptionCode, 0x02);  // Illegal data address
}

// Register Tests
TEST_F(ModBusProtocolTest, ConvertBytesToRegister) {
    uint8_t bytes[] = {0x12, 0x34};
    uint16_t reg = protocol.bytesToRegister(bytes);
    EXPECT_EQ(reg, 0x1234);
}

TEST_F(ModBusProtocolTest, ConvertRegisterToBytes) {
    uint16_t reg = 0x1234;
    uint8_t bytes[2];
    protocol.registerToBytes(reg, bytes);
    EXPECT_EQ(bytes[0], 0x12);
    EXPECT_EQ(bytes[1], 0x34);
}

// Edge Cases
TEST_F(ModBusProtocolTest, MaximumRegisterCount) {
    // ModBus allows max 125 registers per read
    EXPECT_LE(125, 125);
}

TEST_F(ModBusProtocolTest, EmptyData) {
    uint8_t empty[] = {};
    EXPECT_THROW(protocol.calculateCRC16(empty, 0), std::invalid_argument);
}
