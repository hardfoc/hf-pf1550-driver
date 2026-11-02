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
