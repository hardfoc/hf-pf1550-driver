/**
 * @file api_tasks.cpp
 * @brief The API guide's task snippets, compiled by the host build (never run).
 */
#include "my_i2c.hpp"

#include <cstdio>

using Driver = pf1550::PF1550<MyI2c>;

bool init(Driver& pmic) {
  // --8<-- [start:init]
  if (!pmic.EnsureInitialized()) {
    const uint16_t err = pmic.GetErrorFlags();
    if (err & static_cast<uint16_t>(Driver::Error::WrongDeviceId)) {
      std::printf("not a PF1550 at 0x%02X\n", pmic.GetI2cAddress());
    } else {
      std::printf("no answer at 0x%02X (errors 0x%04X)\n", pmic.GetI2cAddress(), err);
    }
    return false;
  }
  // --8<-- [end:init]
  return true;
}

bool profile(Driver& pmic) {
  // --8<-- [start:profile]
  pmic.SetPowerMode(pf1550::PowerMode::Run);     // STANDBY strap low
  pmic.SetUsbRails(true, true);                  // USB_VBUS_EN, USB_OTG_EN high
  if (!pmic.ApplyPortentaH7CarrierProfile()) {   // 23 writes, SW1 first, LDO2_CTRL last
    return false;                                // stopped at the first write that failed
  }
  // --8<-- [end:profile]
  return true;
}

bool rails(Driver& pmic) {
  // --8<-- [start:rails]
  bool ok = true;
  ok &= pmic.SetLdoVoltage(2, pf1550::LdoVoltageCode::V1_8);  // LDO2_VOLT = 0x00
  ok &= pmic.SetSwVoltage(2, pf1550::SwVoltageCode::V3_3);    // SW2_VOLT = 0x07
  ok &= pmic.SetVbusCurrentLimitMa(500);                      // VBUS_INLIM_CNFG = 0x70
  ok &= pmic.WriteRegister(pf1550::Register::Sw2Ctrl,
                           pf1550::SwCtrlBits::kEn | pf1550::SwCtrlBits::kStbyEn);
  // --8<-- [end:rails]
  return ok;
}

bool status(Driver& pmic) {
  // --8<-- [start:status]
  pf1550::PmicState state{};
  pf1550::ChargerState charger{};
  uint8_t category = 0;
  if (!pmic.ReadPmicState(state) || !pmic.ReadChargerState(charger) ||
      !pmic.ReadInterruptCategory(category)) {
    return false;
  }
  std::printf("%s, charger %s, INT_CATEGORY 0x%02X\n", pf1550::PmicStateName(state),
              pf1550::ChargerStateName(charger), category);
  // --8<-- [end:status]
  return true;
}

bool diagnostics(Driver& pmic) {
  // --8<-- [start:diagnostics]
  pf1550::DiagnosticSnapshot snap{};
  if (!pmic.ReadDiagnosticSnapshot(snap)) {      // about 30 reads: identity, rails, faults
    return false;                                // fields that were read are still filled in
  }
  for (size_t i = 0; i < static_cast<size_t>(pf1550::RailId::Vsnvs); ++i) {
    const auto& r = snap.rails[i];
    std::printf("%-4s %4u mV run=%d stby=%d fault=%d\n", pf1550::RailName(static_cast<pf1550::RailId>(i)),
                r.set_voltage_mv, r.enabled_run, r.enabled_stby, r.stat_latched);
  }
  // Between full reads, refresh only what changes on its own (about 14 reads).
  return pmic.RefreshStatusSnapshot(snap);
  // --8<-- [end:diagnostics]
}

bool faults(Driver& pmic) {
  // --8<-- [start:faults]
  uint8_t category = 0;
  if (!pmic.ReadInterruptCategory(category)) {
    return false;
  }
  if (category != 0) {                           // something latched since the last clear
    pf1550::FaultFlags faults{};
    if (!pmic.ReadLatchedFaults(faults)) {
      return false;
    }
    if (faults.Has(pf1550::FaultFlags::kLdo2Fault)) {
      std::printf("LDO2 current limit\n");
    }
    // Handle the cause first, then re-arm: write 1 to every status bit.
    pmic.ClearLatchedFaults();
  }
  // --8<-- [end:faults]
  return true;
}

bool otp(Driver& pmic) {
  // --8<-- [start:otp]
  uint8_t otp[pf1550::OtpRegion::kEndExclusive - pf1550::OtpRegion::kStart] = {};
  if (!pmic.ReadOtpRegion(otp, sizeof otp)) {    // 0x1C..0x36, about 2 ms per byte
    return false;
  }
  uint8_t flavor = 0;
  pmic.ReadRegister(pf1550::Register::OtpFlavor, flavor);  // 0x00: OTP not burned (A0EP)
  // --8<-- [end:otp]
  return true;
}

bool selftest(Driver& pmic) {
  // --8<-- [start:selftest]
  pf1550::SelfTestResult result{};
  const bool pass = pmic.RunPowerSelfTest(result);
  std::printf("self-test %s, worst %s\n", pass ? "PASS" : "FAIL",
              pf1550::FaultSeverityName(result.worst_severity));
  // --8<-- [end:selftest]
  return pass;
}
