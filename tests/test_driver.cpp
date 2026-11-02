/**
 * @file test_driver.cpp
 * @brief Driver behaviour over the register model: identification, profiles, rail settings,
 * strap pins, indirect OTP reads, fault clearing, diagnostics and the datasheet code tables.
 */
#include "mock_pf1550.hpp"
#include "test_harness.hpp"

using pf1550::CtrlPin;
using pf1550::GpioSignal;
using pf1550_test::MockPf1550;
using Driver = pf1550::PF1550<MockPf1550>;

namespace {
uint16_t flag(Driver::Error e) {
  return static_cast<uint16_t>(e);
}
} // namespace

TEST_CASE("finds a PF1550 at 0x08 by its DEVICE_ID and initialises once") {
  MockPf1550 bus;
  Driver pmic(&bus);
  REQUIRE(pmic.EnsureInitialized());
  CHECK(pmic.IsInitialized());
  CHECK_EQ(pmic.GetI2cAddress(), 0x08);
  const size_t transfers = bus.log().size();
  CHECK(pmic.EnsureInitialized());
  CHECK_EQ(bus.log().size(), transfers); // second call is a no-op
  CHECK_EQ(pmic.GetErrorFlags(), 0);
}

TEST_CASE("a wrong DEVICE_ID fails initialisation with WrongDeviceId") {
  MockPf1550 bus;
  bus.reg(0x00) = 0x78; // a PF1500 (DEVICE_ID 000)
  Driver pmic(&bus);
  CHECK(!pmic.EnsureInitialized());
  CHECK(!pmic.IsInitialized());
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::WrongDeviceId)) != 0);
}

TEST_CASE("no device at the address fails initialisation with DeviceNotFound") {
  MockPf1550 bus(0x09);
  Driver pmic(&bus); // talks to 0x08
  CHECK(!pmic.EnsureInitialized());
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::DeviceNotFound)) != 0);
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::I2cRead)) != 0);
}

TEST_CASE("a bus that does not come up fails with NotInitialized") {
  MockPf1550 bus;
  bus.bus_ok = false;
  Driver pmic(&bus);
  CHECK(!pmic.EnsureInitialized());
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::NotInitialized)) != 0);
}

TEST_CASE("the carrier profile writes SW1 first, LDO2_CTRL last, and waits where the table says") {
  MockPf1550 bus;
  Driver pmic(&bus);
  REQUIRE(pmic.EnsureInitialized());
  bus.clearLog();
  REQUIRE(pmic.ApplyPortentaH7CarrierProfile());
  const auto w = bus.writes();
  REQUIRE(w.size() == pf1550::profiles::kPortentaH7Carrier.size());
  CHECK_EQ(w.front().reg, 0x32); // SW1_VOLT
  CHECK_EQ(w.front().value, 0x06);
  CHECK_EQ(w.back().reg, 0x50); // LDO2_CTRL
  CHECK_EQ(w.back().value, 0x0F);
  for (size_t i = 0; i < w.size(); ++i) {
    CHECK_EQ(w[i].reg, pf1550::profiles::kPortentaH7Carrier[i].reg);
    CHECK_EQ(w[i].value, pf1550::profiles::kPortentaH7Carrier[i].value);
  }
  uint32_t total = 0;
  for (uint32_t d : bus.delays) {
    total += d;
  }
  CHECK_EQ(total, 8000u); // 5 ms after SW1_CTRL, 1 ms after each of three charger / SW3 writes
}

TEST_CASE("a profile stops at the first write that fails") {
  MockPf1550 bus;
  bus.fail_write_reg = 0x4D; // LDO1_CTRL
  Driver pmic(&bus);
  REQUIRE(pmic.EnsureInitialized());
  bus.clearLog();
  CHECK(!pmic.ApplyPortentaH7CarrierProfile());
  CHECK_EQ(bus.writes().back().reg, 0x4D);
  CHECK_EQ(bus.writes().size(), 7u);
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::ProfileApply)) != 0);
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::I2cWrite)) != 0);
}

TEST_CASE("rail setters write the voltage register and reject an unknown rail") {
  MockPf1550 bus;
  Driver pmic(&bus);
  REQUIRE(pmic.SetSwVoltage(2, pf1550::SwVoltageCode::V3_3));
  CHECK_EQ(bus.reg(0x38), 0x07);
  REQUIRE(pmic.SetLdoVoltage(3, pf1550::LdoVoltageCode::V1_2));
  CHECK_EQ(bus.reg(0x52), 0x09);
  bus.clearLog();
  CHECK(!pmic.SetSwVoltage(4, pf1550::SwVoltageCode::V3_3));
  CHECK(!pmic.SetLdoVoltage(0, pf1550::LdoVoltageCode::V1_8));
  CHECK(bus.log().empty());
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::InvalidParam)) != 0);
}

TEST_CASE("the VBUS input limit takes only the supported milliamp values") {
  MockPf1550 bus;
  Driver pmic(&bus);
  REQUIRE(pmic.SetVbusCurrentLimitMa(1500));
  CHECK_EQ(bus.reg(0x94), 0xA0);
  REQUIRE(pmic.SetVbusCurrentLimitMa(500));
  CHECK_EQ(bus.reg(0x94), 0x70);
  bus.clearLog();
  CHECK(!pmic.SetVbusCurrentLimitMa(1234));
  CHECK(bus.log().empty());
}

TEST_CASE("power mode and USB rails drive the strap pins by name") {
  MockPf1550 bus;
  Driver pmic(&bus);
  REQUIRE(pmic.SetPowerMode(pf1550::PowerMode::Standby));
  CHECK(bus.pins[static_cast<size_t>(CtrlPin::Standby)] == GpioSignal::Active);
  REQUIRE(pmic.SetPowerMode(pf1550::PowerMode::Run));
  CHECK(bus.pins[static_cast<size_t>(CtrlPin::Standby)] == GpioSignal::Inactive);
  REQUIRE(pmic.SetUsbRails(true, false));
  CHECK(bus.pins[static_cast<size_t>(CtrlPin::UsbVbusEn)] == GpioSignal::Active);
  CHECK(bus.pins[static_cast<size_t>(CtrlPin::UsbOtgEn)] == GpioSignal::Inactive);
}

TEST_CASE("an OTP byte is read through the three keys and FMRADDR / FMRDATA") {
  MockPf1550 bus;
  bus.otp[0x1E] = 0x5A;
  Driver pmic(&bus);
  bus.clearLog();
  uint8_t value = 0;
  REQUIRE(pmic.ReadOtpByte(0x1E, value));
  CHECK_EQ(value, 0x5A);
  const auto w = bus.writes();
  REQUIRE(w.size() == 4u);
  CHECK_EQ(w[0].reg, 0x6F);
  CHECK_EQ(w[0].value, 0x15);
  CHECK_EQ(w[1].reg, 0x9F);
  CHECK_EQ(w[1].value, 0x50);
  CHECK_EQ(w[2].reg, 0xDF);
  CHECK_EQ(w[2].value, 0xAB);
  CHECK_EQ(w[3].reg, 0xC4);
  CHECK_EQ(w[3].value, 0x1E);
  REQUIRE(bus.delays.size() == 1u);
  CHECK_EQ(bus.delays[0], 2000u);
}

TEST_CASE("an OTP region read stops at the end of the OTP range") {
  MockPf1550 bus;
  Driver pmic(&bus);
  uint8_t buf[4] = {};
  CHECK(!pmic.ReadOtpRegion(buf, sizeof buf, 0x35)); // 0x35, 0x36, then 0x37 is past the end
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::InvalidParam)) != 0);
}

TEST_CASE("clearing latched faults writes 1s to every status register") {
  MockPf1550 bus;
  bus.reg(0x18) = 0x02; // LDO2 fault latched
  bus.reg(0x80) = 0x01; // charger interrupt latched
  Driver pmic(&bus);
  pf1550::FaultFlags faults;
  REQUIRE(pmic.ReadLatchedFaults(faults));
  CHECK(faults.Has(pf1550::FaultFlags::kLdo2Fault));
  CHECK(faults.Has(pf1550::FaultFlags::kChgFault));
  REQUIRE(pmic.ClearLatchedFaults());
  CHECK_EQ(bus.reg(0x18), 0);
  CHECK_EQ(bus.reg(0x80), 0);
  REQUIRE(pmic.ReadLatchedFaults(faults));
  CHECK_EQ(faults.value, 0u);
}

TEST_CASE("a diagnostic snapshot decodes state, rails and the VBUS limit") {
  MockPf1550 bus;
  Driver pmic(&bus);
  REQUIRE(pmic.EnsureInitialized());
  REQUIRE(pmic.ApplyPortentaH7CarrierProfile());
  pf1550::DiagnosticSnapshot snap;
  REQUIRE(pmic.ReadDiagnosticSnapshot(snap));
  CHECK(snap.read_ok);
  CHECK_EQ(snap.device_id, 0x7C);
  CHECK(snap.state == pf1550::PmicState::Run);
  CHECK_EQ(snap.vbus_in_limit_ma, 1500);
  const auto mv = [&](pf1550::RailId id) {
    return snap.rails[static_cast<size_t>(id)].set_voltage_mv;
  };
  CHECK_EQ(mv(pf1550::RailId::Sw1), 3000);
  CHECK_EQ(mv(pf1550::RailId::Sw2), 3300);
  CHECK_EQ(mv(pf1550::RailId::Sw3), 3300);
  CHECK_EQ(mv(pf1550::RailId::Ldo1), 1000);
  CHECK_EQ(mv(pf1550::RailId::Ldo2), 1800);
  CHECK_EQ(mv(pf1550::RailId::Ldo3), 1200);
  CHECK(snap.rails[static_cast<size_t>(pf1550::RailId::Sw2)].enabled_stby);

  pf1550::SelfTestResult result;
  CHECK(pmic.RunPowerSelfTest(result));
  CHECK(result.device_id_ok && result.state_run && result.all_rails_enabled);
}

TEST_CASE("code tables match the datasheet") {
  // Table 31, DVS disabled: eight set points, every code from 7 up is 3.30 V.
  CHECK_EQ(pf1550::SwCodeToMillivolts(0), 1100);
  CHECK_EQ(pf1550::SwCodeToMillivolts(5), 2500);
  CHECK_EQ(pf1550::SwCodeToMillivolts(6), 3000);
  CHECK_EQ(pf1550::SwCodeToMillivolts(40), 3300);
  // Table 31, DVS enabled: 0.6 V + 12.5 mV per code.
  CHECK_EQ(pf1550::SwDvsCodeToMillivolts(40), 1100);
  // Table 37: SW3 1.8 V to 3.3 V in 100 mV steps.
  CHECK_EQ(pf1550::Sw3CodeToMillivolts(0), 1800);
  CHECK_EQ(pf1550::Sw3CodeToMillivolts(15), 3300);
  // Table 41 (LDO1, LDO3) and Table 43 (LDO2).
  CHECK_EQ(pf1550::LdoCodeToMillivolts(15, false), 1500);
  CHECK_EQ(pf1550::LdoCodeToMillivolts(16, false), 1800);
  CHECK_EQ(pf1550::LdoCodeToMillivolts(31, false), 3300);
  CHECK_EQ(pf1550::LdoCodeToMillivolts(15, true), 3300);
  // Table 58: VBUS input current limit.
  const uint16_t table58[] = {10,  15,  20,  25,  30,  35,  40,  45,  50,   100, 150,
                              200, 300, 400, 500, 600, 700, 800, 900, 1000, 1500};
  for (uint8_t code = 0; code < sizeof table58 / sizeof table58[0]; ++code) {
    CHECK_EQ(pf1550::VbusLimitRegToMilliamps(static_cast<uint8_t>(code << 3)), table58[code]);
  }
  // Table 146: STATE_INFO.
  CHECK(pf1550::DecodeStateInfo(0x0C) == pf1550::PmicState::Run);
  CHECK(pf1550::DecodeStateInfo(0x2B) == pf1550::PmicState::RegsDisable);
}
