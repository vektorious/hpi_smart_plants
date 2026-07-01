// Double-reset detection.
//
// Detects two manual resets in quick succession, used to force the setup
// portal open even when WiFi credentials are already saved.
//
// The "armed" flag lives in NVS (flash) rather than RTC memory: a press of the
// board's reset button pulls EN low, and RTC RAM is not reliably retained
// across an EN reset on all ESP32-C6 boards, whereas NVS always survives.
//
// Arming window: the flag is set on the first manual reset and cleared again
// when the device enters deep sleep (disarmDoubleReset(), called from
// goToSleep()). So a second reset counts as a "double reset" only if it lands
// within one boot cycle of the first — a natural double-press window. Scheduled
// timer wakeups return early and never touch NVS, so normal hourly operation
// causes no flash wear.

#include <Preferences.h>

static Preferences drdPrefs;
static const char *DRD_NAMESPACE = "drd";

bool detectDoubleReset() {
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    // Scheduled wake — not a manual reset. Don't arm, don't write flash.
    return false;
  }

  drdPrefs.begin(DRD_NAMESPACE, /*readOnly=*/false);
  bool armed = drdPrefs.getBool("armed", false);
  bool doubleReset = false;

  if (armed) {
    doubleReset = true;
    drdPrefs.putBool("armed", false);   // consume it
    Serial.println("Double reset detected — opening setup portal");
  } else {
    drdPrefs.putBool("armed", true);    // arm; disarmed at deep sleep
  }
  drdPrefs.end();

  return doubleReset;
}

void disarmDoubleReset() {
  drdPrefs.begin(DRD_NAMESPACE, /*readOnly=*/false);
  if (drdPrefs.getBool("armed", false)) {
    drdPrefs.putBool("armed", false);
  }
  drdPrefs.end();
}
