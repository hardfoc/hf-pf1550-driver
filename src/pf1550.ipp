/**
 * @file pf1550.ipp
 * @brief PF1550 driver template implementation
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

namespace pf1550 {

template <typename BusType>
PF1550<BusType>::PF1550(BusType* bus, uint8_t address) noexcept
    : bus_(bus), address_(address), initialized_(false), error_flags_(0) {}

template <typename BusType>
bool PF1550<BusType>::writeReg8(uint8_t reg, uint8_t value) noexcept {
  if (bus_ == nullptr) {
    setError(Error::InvalidParam);
    return false;
  }
  const uint8_t data = value;
  if (!bus_->Write(address_, reg, &data, 1)) {
    setError(Error::I2cWrite);
    return false;
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::readReg8(uint8_t reg, uint8_t& value) noexcept {
  if (bus_ == nullptr) {
    setError(Error::InvalidParam);
    return false;
  }
  if (!bus_->Read(address_, reg, &value, 1)) {
    setError(Error::I2cRead);
    return false;
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::EnsureInitialized() noexcept {
  if (initialized_) {
    return true;
  }
  if (bus_ == nullptr || !bus_->EnsureInitialized()) {
    setError(Error::NotInitialized);
    return false;
  }
  if (!VerifyDevice()) {
    return false;
  }
  initialized_ = true;
  ClearErrorFlags(static_cast<uint16_t>(Error::NotInitialized));
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ReadDeviceId(uint8_t& id) noexcept {
  if (!readReg8(static_cast<uint8_t>(Register::DeviceId), id)) {
    return false;
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::VerifyDevice() noexcept {
  uint8_t id = 0;
  if (!ReadDeviceId(id)) {
    setError(Error::DeviceNotFound);
    return false;
  }
  if (id != kExpectedDeviceId) {
    setError(Error::WrongDeviceId);
    return false;
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ReadRegister(Register reg, uint8_t& value) noexcept {
  return readReg8(static_cast<uint8_t>(reg), value);
}

template <typename BusType>
bool PF1550<BusType>::WriteRegister(Register reg, uint8_t value) noexcept {
  return writeReg8(static_cast<uint8_t>(reg), value);
}

template <typename BusType>
bool PF1550<BusType>::ApplyProfile(std::span<const profiles::RegisterWrite> profile) noexcept {
  if (bus_ == nullptr || !bus_->EnsureInitialized()) {
    setError(Error::NotInitialized);
    return false;
  }

  for (const auto& entry : profile) {
    if (!writeReg8(entry.reg, entry.value)) {
      setError(Error::ProfileApply);
      return false;
    }
    if (entry.delay_us > 0) {
      bus_->DelayUs(entry.delay_us);
    }
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ApplyPortentaH7DefaultProfile() noexcept {
  return ApplyProfile(profiles::kPortentaH7Default);
}

template <typename BusType>
bool PF1550<BusType>::ApplyPortentaH7CarrierProfile() noexcept {
  return ApplyProfile(profiles::kPortentaH7Carrier);
}

template <typename BusType>
bool PF1550<BusType>::SetPowerMode(PowerMode mode) noexcept {
  if (bus_ == nullptr) {
    setError(Error::InvalidParam);
    return false;
  }
  switch (mode) {
  case PowerMode::Run:
    bus_->GpioSetInactive(CtrlPin::Standby);
    break;
  case PowerMode::Standby:
    bus_->GpioSetActive(CtrlPin::Standby);
    break;
  default:
    setError(Error::InvalidParam);
    return false;
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::SetUsbRails(bool vbus_en, bool otg_en) noexcept {
  if (bus_ == nullptr) {
    setError(Error::InvalidParam);
    return false;
  }
  bus_->GpioSet(CtrlPin::UsbVbusEn, vbus_en ? GpioSignal::Active : GpioSignal::Inactive);
  bus_->GpioSet(CtrlPin::UsbOtgEn, otg_en ? GpioSignal::Active : GpioSignal::Inactive);
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ReadPmicStatus(uint8_t& status) noexcept {
  return readReg8(static_cast<uint8_t>(Register::StateInfo), status);
}

template <typename BusType>
bool PF1550<BusType>::ReadPmicState(PmicState& state) noexcept {
  uint8_t raw = 0;
  if (!ReadPmicStatus(raw)) {
    state = PmicState::Unknown;
    return false;
  }
  state = DecodeStateInfo(raw);
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ReadChargerState(ChargerState& state, uint8_t* raw_reg) noexcept {
  uint8_t raw = 0;
  if (!readReg8(static_cast<uint8_t>(Register::ChgSense), raw)) {
    state = ChargerState::Unknown;
    return false;
  }
  if (raw_reg != nullptr) {
    *raw_reg = raw;
  }
  state = DecodeChargerSense(raw);
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ReadOtpByte(uint8_t otp_addr, uint8_t& value) noexcept {
  if (!writeReg8(OtpUnlockKeys::kKey1Reg, OtpUnlockKeys::kKey1Val) ||
      !writeReg8(OtpUnlockKeys::kKey2Reg, OtpUnlockKeys::kKey2Val) ||
      !writeReg8(OtpUnlockKeys::kKey3Reg, OtpUnlockKeys::kKey3Val)) {
    return false;
  }

  if (!writeReg8(static_cast<uint8_t>(Register::FmrAddr), otp_addr)) {
    return false;
  }
  bus_->DelayUs(2000);
  return readReg8(static_cast<uint8_t>(Register::FmrData), value);
}

template <typename BusType>
bool PF1550<BusType>::ReadOtpRegion(uint8_t* buffer, size_t len, uint8_t start) noexcept {
  if (buffer == nullptr || len == 0) {
    setError(Error::InvalidParam);
    return false;
  }
  for (size_t i = 0; i < len; ++i) {
    const uint8_t addr = static_cast<uint8_t>(start + i);
    if (addr >= OtpRegion::kEndExclusive) {
      setError(Error::InvalidParam);
      return false;
    }
    if (!ReadOtpByte(addr, buffer[i])) {
      return false;
    }
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::SetSwVoltage(uint8_t sw_index, SwVoltageCode run_code) noexcept {
  Register volt_reg = Register::Sw1Volt;
  switch (sw_index) {
  case 1:
    volt_reg = Register::Sw1Volt;
    break;
  case 2:
    volt_reg = Register::Sw2Volt;
    break;
  case 3:
    volt_reg = Register::Sw3Volt;
    break;
  default:
    setError(Error::InvalidParam);
    return false;
  }
  return writeReg8(static_cast<uint8_t>(volt_reg), static_cast<uint8_t>(run_code));
}

template <typename BusType>
bool PF1550<BusType>::SetLdoVoltage(uint8_t ldo_index, LdoVoltageCode code) noexcept {
  Register volt_reg = Register::Ldo1Volt;
  switch (ldo_index) {
  case 1:
    volt_reg = Register::Ldo1Volt;
    break;
  case 2:
    volt_reg = Register::Ldo2Volt;
    break;
  case 3:
    volt_reg = Register::Ldo3Volt;
    break;
  default:
    setError(Error::InvalidParam);
    return false;
  }
  return writeReg8(static_cast<uint8_t>(volt_reg), static_cast<uint8_t>(code));
}

template <typename BusType>
bool PF1550<BusType>::SetVbusCurrentLimitMa(uint16_t limit_ma) noexcept {
  const uint8_t value = VbusLimitMilliampsToReg(limit_ma);
  if (value == 0xFFU) {
    setError(Error::InvalidParam);
    return false;
  }
  return writeReg8(static_cast<uint8_t>(Register::VbusInCurrentLimit), value);
}

// ---------------------------------------------------------------------------
// Interrupt management
// ---------------------------------------------------------------------------

template <typename BusType>
bool PF1550<BusType>::ReadInterruptCategory(uint8_t& category) noexcept {
  return readReg8(static_cast<uint8_t>(Register::IntCategory), category);
}

template <typename BusType>
bool PF1550<BusType>::ReadLatchedFaults(FaultFlags& faults) noexcept {
  faults = FaultFlags{};
  uint8_t sw0 = 0, sw1 = 0, ldo0 = 0, temp0 = 0, misc0 = 0, chg = 0, vbus = 0;
  bool any_fail = false;

  any_fail |= !readReg8(static_cast<uint8_t>(Register::SwIntStat0), sw0);
  any_fail |= !readReg8(static_cast<uint8_t>(Register::SwIntStat1), sw1);
  any_fail |= !readReg8(static_cast<uint8_t>(Register::LdoIntStat0), ldo0);
  any_fail |= !readReg8(static_cast<uint8_t>(Register::TempIntStat0), temp0);
  any_fail |= !readReg8(static_cast<uint8_t>(Register::MiscIntStat0), misc0);
  any_fail |= !readReg8(static_cast<uint8_t>(Register::ChgInt), chg);
  any_fail |= !readReg8(static_cast<uint8_t>(Register::VbusSns), vbus);

  if (sw0 & SwIntStat0Bits::kSw1Ls) faults.Set(FaultFlags::kSw1Ls);
  if (sw0 & SwIntStat0Bits::kSw2Ls) faults.Set(FaultFlags::kSw2Ls);
  if (sw0 & SwIntStat0Bits::kSw3Ls) faults.Set(FaultFlags::kSw3Ls);
  if (sw1 & SwIntStat1Bits::kSw1Hs) faults.Set(FaultFlags::kSw1Hs);
  if (sw1 & SwIntStat1Bits::kSw2Hs) faults.Set(FaultFlags::kSw2Hs);
  if (sw1 & SwIntStat1Bits::kSw3Hs) faults.Set(FaultFlags::kSw3Hs);
  if (ldo0 & LdoIntStat0Bits::kLdo1Fault) faults.Set(FaultFlags::kLdo1Fault);
  if (ldo0 & LdoIntStat0Bits::kLdo2Fault) faults.Set(FaultFlags::kLdo2Fault);
  if (ldo0 & LdoIntStat0Bits::kLdo3Fault) faults.Set(FaultFlags::kLdo3Fault);
  if (temp0 & TempIntStat0Bits::kTherm110) faults.Set(FaultFlags::kTempWarn);
  if (temp0 & TempIntStat0Bits::kTherm125) faults.Set(FaultFlags::kTempShdn);
  if (misc0 & MiscIntStat0Bits::kPwrOn) faults.Set(FaultFlags::kPwrOn);
  if (misc0 & MiscIntStat0Bits::kLowSysWarn) faults.Set(FaultFlags::kVsysLow);
  if (misc0 & MiscIntStat0Bits::kSysOvlo) faults.Set(FaultFlags::kVsysOv);
  // PWRUP_I / PWRDN_I report finished mode transitions, not faults: read MISC_INT_STAT0 for them.
  if (chg != 0) faults.Set(FaultFlags::kChgFault);
  // VBUS_VALID is bit 5 of VBUS_SNS — invalid means !bit5.
  if ((vbus & (1u << 5)) == 0) faults.Set(FaultFlags::kVbusInval);

  if (any_fail) {
    faults.Set(FaultFlags::kI2cReadFail);
    setError(Error::DiagnosticRead);
    return false;
  }
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ClearLatchedFaults() noexcept {
  const uint8_t kClearAll = 0xFFU;
  bool ok = true;
  ok &= writeReg8(static_cast<uint8_t>(Register::SwIntStat0), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::SwIntStat1), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::SwIntStat2), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::LdoIntStat0), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::TempIntStat0), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::OnKeyIntStat0), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::MiscIntStat0), kClearAll);
  ok &= writeReg8(static_cast<uint8_t>(Register::ChgInt), kClearAll);
  return ok;
}

template <typename BusType>
bool PF1550<BusType>::SetInterruptMaskAll(bool masked) noexcept {
  const uint8_t mask_value = masked ? 0xFFU : 0x00U;
  bool ok = true;
  ok &= writeReg8(static_cast<uint8_t>(Register::SwIntMask0), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::SwIntMask1), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::SwIntMask2), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::LdoIntMask0), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::TempIntMask0), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::OnKeyIntMask0), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::MiscIntMask0), mask_value);
  ok &= writeReg8(static_cast<uint8_t>(Register::ChgIntMask), mask_value);
  return ok;
}

// ---------------------------------------------------------------------------
// Diagnostic snapshot + self-test
// ---------------------------------------------------------------------------

template <typename BusType>
bool PF1550<BusType>::fillRailSnapshot(DiagnosticSnapshot& snap) noexcept {
  struct RailMap {
    RailId   id;
    Register volt_reg;
    Register ctrl_reg;
    bool     is_ldo2;  // true → LDO2 group B encoding
    bool     is_sw;
  };
  static constexpr RailMap kMap[] = {
      {RailId::Sw1,  Register::Sw1Volt,  Register::Sw1Ctrl,  false, true},
      {RailId::Sw2,  Register::Sw2Volt,  Register::Sw2Ctrl,  false, true},
      {RailId::Sw3,  Register::Sw3Volt,  Register::Sw3Ctrl,  false, true},
      {RailId::Ldo1, Register::Ldo1Volt, Register::Ldo1Ctrl, false, false},
      {RailId::Ldo2, Register::Ldo2Volt, Register::Ldo2Ctrl, true,  false},
      {RailId::Ldo3, Register::Ldo3Volt, Register::Ldo3Ctrl, false, false},
  };
  bool ok = true;
  for (const auto& m : kMap) {
    auto& d = snap.rails[static_cast<size_t>(m.id)];
    if (!readReg8(static_cast<uint8_t>(m.volt_reg), d.volt_reg)) { ok = false; continue; }
    if (!readReg8(static_cast<uint8_t>(m.ctrl_reg), d.ctrl_reg)) { ok = false; continue; }
    if (m.id == RailId::Sw3) {
      d.set_voltage_mv = Sw3CodeToMillivolts(d.volt_reg);  /* Table 37 */
    } else if (m.is_sw) {
      const bool dvs = sw_dvs_enabled_[m.id == RailId::Sw1 ? 0U : 1U];
      d.set_voltage_mv = dvs ? SwDvsCodeToMillivolts(d.volt_reg) : SwCodeToMillivolts(d.volt_reg);
    } else {
      d.set_voltage_mv = LdoCodeToMillivolts(d.volt_reg & 0x1FU, m.is_ldo2);
    }
    d.enabled_run  = (d.ctrl_reg & SwCtrlBits::kEn) != 0;
    d.enabled_stby = (d.ctrl_reg & SwCtrlBits::kStbyEn) != 0;
  }
  return ok;
}

template <typename BusType>
bool PF1550<BusType>::fillFaultSnapshot(DiagnosticSnapshot& snap) noexcept {
  if (!ReadLatchedFaults(snap.faults)) {
    return false;
  }
  // Per-rail latched + live mapping.
  uint8_t sw_sense0 = 0, sw_sense1 = 0, ldo_sense = 0;
  (void)readReg8(static_cast<uint8_t>(Register::SwIntSense0), sw_sense0);
  (void)readReg8(static_cast<uint8_t>(Register::SwIntSense1), sw_sense1);
  (void)readReg8(static_cast<uint8_t>(Register::LdoIntSense0), ldo_sense);

  auto mark = [&](RailId id, bool latched, bool live) {
    auto& d = snap.rails[static_cast<size_t>(id)];
    d.stat_latched = latched;
    d.sense_live   = live;
  };
  mark(RailId::Sw1,
       snap.faults.Has(FaultFlags::kSw1Ls | FaultFlags::kSw1Hs),
       ((sw_sense0 & SwIntStat0Bits::kSw1Ls) || (sw_sense1 & SwIntStat1Bits::kSw1Hs)) != 0);
  mark(RailId::Sw2,
       snap.faults.Has(FaultFlags::kSw2Ls | FaultFlags::kSw2Hs),
       ((sw_sense0 & SwIntStat0Bits::kSw2Ls) || (sw_sense1 & SwIntStat1Bits::kSw2Hs)) != 0);
  mark(RailId::Sw3,
       snap.faults.Has(FaultFlags::kSw3Ls | FaultFlags::kSw3Hs),
       ((sw_sense0 & SwIntStat0Bits::kSw3Ls) || (sw_sense1 & SwIntStat1Bits::kSw3Hs)) != 0);
  mark(RailId::Ldo1, snap.faults.Has(FaultFlags::kLdo1Fault),
       (ldo_sense & LdoIntStat0Bits::kLdo1Fault) != 0);
  mark(RailId::Ldo2, snap.faults.Has(FaultFlags::kLdo2Fault),
       (ldo_sense & LdoIntStat0Bits::kLdo2Fault) != 0);
  mark(RailId::Ldo3, snap.faults.Has(FaultFlags::kLdo3Fault),
       (ldo_sense & LdoIntStat0Bits::kLdo3Fault) != 0);
  return true;
}

template <typename BusType>
bool PF1550<BusType>::ReadDiagnosticSnapshot(DiagnosticSnapshot& out) noexcept {
  out = DiagnosticSnapshot{};
  bool ok = true;
  ok &= readReg8(static_cast<uint8_t>(Register::DeviceId),    out.device_id);
  ok &= readReg8(static_cast<uint8_t>(Register::OtpFlavor),   out.otp_flavor);
  ok &= readReg8(static_cast<uint8_t>(Register::SiliconRev),  out.silicon_rev);
  ok &= readReg8(static_cast<uint8_t>(Register::IntCategory), out.int_category);
  ok &= readReg8(static_cast<uint8_t>(Register::StateInfo),   out.state_info_reg);
  out.state = DecodeStateInfo(out.state_info_reg);

  uint8_t chg_raw = 0;
  if (readReg8(static_cast<uint8_t>(Register::ChgSense), chg_raw)) {
    out.chg_sense_reg = chg_raw;
    out.charger = DecodeChargerSense(chg_raw);
  } else {
    ok = false;
  }
  ok &= readReg8(static_cast<uint8_t>(Register::VbusSns), out.vbus_sns_reg);

  uint8_t vbus_lim_reg = 0;
  if (readReg8(static_cast<uint8_t>(Register::VbusInCurrentLimit), vbus_lim_reg)) {
    out.vbus_in_limit_ma = VbusLimitRegToMilliamps(vbus_lim_reg);
  }

  ok &= fillRailSnapshot(out);
  ok &= fillFaultSnapshot(out);

  out.read_ok = ok;
  if (!ok) {
    setError(Error::DiagnosticRead);
  }
  return ok;
}

template <typename BusType>
bool PF1550<BusType>::RefreshStatusSnapshot(DiagnosticSnapshot& snap) noexcept {
  bool ok = true;
  ok &= readReg8(static_cast<uint8_t>(Register::IntCategory), snap.int_category);
  ok &= readReg8(static_cast<uint8_t>(Register::StateInfo),   snap.state_info_reg);
  snap.state = DecodeStateInfo(snap.state_info_reg);
  uint8_t chg_raw = 0;
  if (readReg8(static_cast<uint8_t>(Register::ChgSense), chg_raw)) {
    snap.chg_sense_reg = chg_raw;
    snap.charger = DecodeChargerSense(chg_raw);
  } else {
    ok = false;
  }
  ok &= readReg8(static_cast<uint8_t>(Register::VbusSns), snap.vbus_sns_reg);
  ok &= fillFaultSnapshot(snap);
  snap.read_ok = ok;
  if (!ok) {
    setError(Error::DiagnosticRead);
  }
  return ok;
}

template <typename BusType>
bool PF1550<BusType>::RunPowerSelfTest(SelfTestResult& out) noexcept {
  out = SelfTestResult{};
  out.ran = true;

  if (!ReadDiagnosticSnapshot(out.snapshot)) {
    out.worst_severity = FaultSeverity::kMcuKill;  // could not even read PMIC
    setError(Error::SelfTestFailed);
    return false;
  }

  out.device_id_ok = (out.snapshot.device_id == kExpectedDeviceId);
  out.state_run = (out.snapshot.state == PmicState::Run);

  out.all_rails_enabled = true;
  for (size_t i = 0; i < static_cast<size_t>(RailId::COUNT); ++i) {
    // Skip VSNVS (no `EN` bit in same place).
    if (static_cast<RailId>(i) == RailId::Vsnvs) continue;
    if (!out.snapshot.rails[i].enabled_run) {
      out.all_rails_enabled = false;
      break;
    }
  }

  out.faults = out.snapshot.faults;
  out.worst_severity = WorstSeverityPortentaH7(out.faults);

  auto sev_val = [](FaultSeverity s) noexcept {
    return static_cast<uint8_t>(s);
  };
  auto raise = [&](FaultSeverity target) noexcept {
    if (sev_val(out.worst_severity) < sev_val(target)) {
      out.worst_severity = target;
    }
  };

  if (!out.device_id_ok)      raise(FaultSeverity::kMcuKill);
  if (!out.state_run)         raise(FaultSeverity::kCritical);
  if (!out.all_rails_enabled) raise(FaultSeverity::kCritical);

  if (out.worst_severity != FaultSeverity::kInfo) {
    setError(Error::SelfTestFailed);
  }
  return out.worst_severity == FaultSeverity::kInfo;
}

} // namespace pf1550
