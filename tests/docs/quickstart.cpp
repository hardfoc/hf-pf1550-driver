/**
 * @file quickstart.cpp
 * @brief The Quickstart page's example, compiled by the host build (never run).
 */
#include "my_i2c.hpp"

#include <cstdio>

// --8<-- [start:quickstart]
#include "pf1550.hpp"

int bring_up_pmic(MyI2c& bus) {
  pf1550::PF1550<MyI2c> pmic(&bus);              // 7-bit address 0x08

  if (!pmic.EnsureInitialized()) {               // 1. bus up, DEVICE_ID == 0x7C
    return -1;                                   // wiring, pull-ups or address
  }

  pf1550::PmicState state{};                     // 2. where is the state machine?
  if (!pmic.ReadPmicState(state) || state != pf1550::PmicState::Run) {
    return -2;
  }

  pf1550::DiagnosticSnapshot snap{};             // 3. every rail's set point and enable
  if (!pmic.ReadDiagnosticSnapshot(snap)) {
    return -3;
  }
  for (const auto& rail : snap.rails) {
    std::printf("%u mV %s\n", rail.set_voltage_mv, rail.enabled_run ? "on" : "off");
  }
  return 0;
}
// --8<-- [end:quickstart]
