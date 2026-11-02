/**
 * @file configuration.cpp
 * @brief The Configuration page's snippets, compiled by the host build (never run).
 */
#include "my_i2c.hpp"

#include <array>

// --8<-- [start:custom-profile]
#include "pf1550_profiles.hpp"

constexpr std::array<pf1550::profiles::RegisterWrite, 4> kMyBoardProfile = {{
    {0x4F, 0x0F, 0},     // LDO2_VOLT: 3.3 V (Table 43)
    {0x52, 0x10, 0},     // LDO3_VOLT: 1.8 V (Table 41)
    {0x53, 0x0F, 0},     // LDO3_CTRL: on in RUN, STANDBY and SLEEP, low power
    {0x50, 0x0F, 1000},  // LDO2_CTRL last, then wait 1 ms
}};

bool apply_my_board(pf1550::PF1550<MyI2c>& pmic) {
  return pmic.ApplyProfile(kMyBoardProfile);
}
// --8<-- [end:custom-profile]

bool declare_dvs(pf1550::PF1550<MyI2c>& pmic) {
  // --8<-- [start:dvs]
  // SW1 on this part runs from the DVS table (0.6 V + 12.5 mV per code): decode it that way.
  pmic.SetSwDvsEnabled(1, true);
  pf1550::DiagnosticSnapshot snap{};
  return pmic.ReadDiagnosticSnapshot(snap);
  // --8<-- [end:dvs]
}
