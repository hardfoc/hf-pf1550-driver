/**
 * @file test_conformance.cpp
 * @brief Regression tests for behaviour checked against the NXP PF1550 data sheet (Rev. 7, section 12):
 * interrupt status bit positions, the SW voltage tables and their read-only cases, the VBUS input limit
 * table, the VSNVS snapshot entry and the self-test without VBUS.
 */
#include "mock_pf1550.hpp"
#include "test_harness.hpp"

using pf1550::FaultFlags;
using pf1550_test::MockPf1550;
using Driver = pf1550::PF1550<MockPf1550>;

namespace {
[[maybe_unused]] uint16_t flag(Driver::Error e) {
  return static_cast<uint16_t>(e);
}
FaultFlags latched(MockPf1550& bus) {
  Driver pmic(&bus);
  FaultFlags f{};
  (void)pmic.ReadLatchedFaults(f);
  return f;
}
} // namespace

// Table 108: MISC_INT_STAT0 = PWRUP_I, PWRDN_I, PWRON_I, LOW_SYS_WARN_I, SYS_OVLO_I from bit 0.
TEST_CASE("a RUN to STANDBY transition (PWRDN_I) is not a VSYS fault") {
  MockPf1550 bus;
  bus.reg(0x28) = 1u << 1; // PWRDN_I
  const FaultFlags f = latched(bus);
  CHECK(!f.Has(FaultFlags::kVsysLow));
  CHECK(pf1550::WorstSeverityPortentaH7(f) == pf1550::FaultSeverity::kInfo);
}

TEST_CASE("MISC_INT_STAT0 bits decode to PWRON, LOW_SYS_WARN and SYS_OVLO") {
  MockPf1550 bus;
  bus.reg(0x28) = 1u << 2;
  CHECK_EQ(latched(bus).value, FaultFlags::kPwrOn);
  bus.reg(0x28) = 1u << 3;
  CHECK_EQ(latched(bus).value, FaultFlags::kVsysLow);
  bus.reg(0x28) = 1u << 4;
  CHECK_EQ(latched(bus).value, FaultFlags::kVsysOv);
  bus.reg(0x28) = 1u << 0; // PWRUP_I: a transition, not a fault
  CHECK_EQ(latched(bus).value, 0u);
}

// Tables 90 and 93: SW1/SW2/SW3 at bits 0, 1, 2 of SW_INT_STAT0 (low side) and SW_INT_STAT1 (high side).
TEST_CASE("SW current-limit bits are 0, 1 and 2 for SW1, SW2 and SW3") {
  MockPf1550 bus;
  bus.reg(0x08) = 1u << 1;
  CHECK_EQ(latched(bus).value, FaultFlags::kSw2Ls);
  bus.reg(0x08) = 1u << 2;
  CHECK_EQ(latched(bus).value, FaultFlags::kSw3Ls);
  bus.reg(0x08) = 0;
  bus.reg(0x0B) = 1u << 1;
  CHECK_EQ(latched(bus).value, FaultFlags::kSw2Hs);
  bus.reg(0x0B) = 1u << 2;
  CHECK_EQ(latched(bus).value, FaultFlags::kSw3Hs);
}

TEST_CASE("SW sense bits mark the right rail live in the snapshot") {
  MockPf1550 bus;
  bus.reg(0x0A) = 1u << 2; // SW_INT_SENSE0: SW3 low side
  Driver pmic(&bus);
  pf1550::DiagnosticSnapshot snap{};
  REQUIRE(pmic.ReadDiagnosticSnapshot(snap));
  CHECK(snap.rails[static_cast<size_t>(pf1550::RailId::Sw3)].sense_live);
  CHECK(!snap.rails[static_cast<size_t>(pf1550::RailId::Sw2)].sense_live);
}

// Table 102: TEMP_INT_STAT0 = THERM110I (bit 0), unused (bit 1), THERM125I (bit 2).
TEST_CASE("TEMP_INT_STAT0 bit 2 is the 125 C crossing and bit 1 is unused") {
  MockPf1550 bus;
  bus.reg(0x20) = 1u << 2;
  CHECK_EQ(latched(bus).value, FaultFlags::kTempShdn);
  bus.reg(0x20) = 1u << 1;
  CHECK_EQ(latched(bus).value, 0u);
  bus.reg(0x20) = 1u << 0;
  CHECK_EQ(latched(bus).value, FaultFlags::kTempWarn);
}

// Table 58: VBUS_LIN_ILIM codes 21 to 31 are reserved.
TEST_CASE("reserved VBUS input-limit codes decode to 0 mA") {
  for (uint8_t code = 21; code < 32; ++code) {
    CHECK_EQ(pf1550::VbusLimitRegToMilliamps(static_cast<uint8_t>(code << 3)), 0);
  }
  CHECK_EQ(pf1550::VbusLimitRegToMilliamps(20u << 3), 1500);
}

// Table 31: DVS enabled is 0.6 V + 12.5 mV per code and the registers are writable; DVS disabled makes
// SWx_VOLT read-only. Table 37: SW3 is read-only.
TEST_CASE("SetSwVoltage encodes the voltage for a DVS rail and refuses read-only registers") {
  MockPf1550 bus;
  Driver pmic(&bus);
  pmic.SetSwDvsEnabled(1, true);
  REQUIRE(pmic.SetSwVoltage(1, pf1550::SwVoltageCode::V1_1));
  CHECK_EQ(bus.reg(0x32), 40); // 0.6 V + 40 x 12.5 mV = 1.100 V
  bus.clearLog();
  CHECK(!pmic.SetSwVoltage(1, pf1550::SwVoltageCode::V3_3)); // above the DVS range
  CHECK(!pmic.SetSwVoltage(2, pf1550::SwVoltageCode::V3_3)); // DVS disabled: read-only
  CHECK(!pmic.SetSwVoltage(3, pf1550::SwVoltageCode::V3_1)); // SW3: read-only
  CHECK(bus.writes().empty());
  CHECK((pmic.GetErrorFlags() & flag(Driver::Error::InvalidParam)) != 0);
}

TEST_CASE("SetSwVoltageMv takes a DVS grid voltage and nothing else") {
  MockPf1550 bus;
  Driver pmic(&bus);
  pmic.SetSwDvsEnabled(2, true);
  REQUIRE(pmic.SetSwVoltageMv(2, 1000));
  CHECK_EQ(bus.reg(0x38), 0x20);
  REQUIRE(pmic.SetSwVoltageMv(2, 1388)); // 1.3875 V, the top code, rounded to mV
  CHECK_EQ(bus.reg(0x38), 0x3F);
  bus.clearLog();
  CHECK(!pmic.SetSwVoltageMv(2, 1006));
  CHECK(!pmic.SetSwVoltageMv(1, 1000)); // SW1 still DVS disabled
  CHECK(!pmic.SetSwVoltageMv(3, 3300));
  CHECK(bus.writes().empty());
  CHECK_EQ(pf1550::SwDvsMillivoltsToCode(600), 0);
  CHECK_EQ(pf1550::SwDvsMillivoltsToCode(1100), 40);
  CHECK_EQ(pf1550::SwDvsMillivoltsToCode(599), 0xFF);
}

TEST_CASE("the Portenta profiles do not write the read-only SW voltage registers") {
  for (const auto* profile : {&pf1550::profiles::kPortentaH7Default, &pf1550::profiles::kPortentaH7Carrier}) {
    for (const auto& w : *profile) {
      const bool sw_volt = (w.reg >= 0x32 && w.reg <= 0x34) || (w.reg >= 0x38 && w.reg <= 0x3A) ||
                           (w.reg >= 0x3E && w.reg <= 0x40);
      CHECK(!sw_volt);
    }
    CHECK_EQ(profile->back().reg, 0x50); // LDO2_CTRL stays last
  }
}

// Table 127: VSNVS_CTRL (0x48); VSNVS is a fixed 3.0 V always-on output.
TEST_CASE("the snapshot fills the VSNVS entry") {
  MockPf1550 bus;
  bus.reg(0x48) = 0x20; // LIBGDIS
  Driver pmic(&bus);
  pf1550::DiagnosticSnapshot snap{};
  REQUIRE(pmic.ReadDiagnosticSnapshot(snap));
  const auto& v = snap.rails[static_cast<size_t>(pf1550::RailId::Vsnvs)];
  CHECK_EQ(v.ctrl_reg, 0x20);
  CHECK_EQ(v.set_voltage_mv, 3000);
  CHECK(v.enabled_run && v.enabled_stby);
}

// Table 153: VBUS_VALID clear means no valid input, the normal state when running from the battery.
TEST_CASE("the self-test passes on battery with no VBUS") {
  MockPf1550 bus;
  for (uint8_t r : {0x35, 0x3B, 0x41, 0x4D, 0x50, 0x53}) {
    bus.reg(r) = 0x01; // every rail enabled in RUN
  }
  bus.reg(0x86) = 0; // VBUS not valid
  Driver pmic(&bus);
  pf1550::SelfTestResult result{};
  CHECK(pmic.RunPowerSelfTest(result));
  CHECK(result.faults.Has(FaultFlags::kVbusInval));
  CHECK(result.worst_severity == pf1550::FaultSeverity::kInfo);
}
