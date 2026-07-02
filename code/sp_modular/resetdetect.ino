// Double-reset detection.
//
// Detects two manual resets in quick succession, used to force the setup
// portal open even when WiFi credentials are already saved.
//
// The "armed" flag lives in NVS (flash) rather than RTC memory: a press of the
// board's reset button pulls EN low, and RTC RAM is not reliably retained
// across an EN reset on all ESP32-C6 boards, whereas NVS always survives.
//
// Detection window: on the first manual reset we arm the flag and then block for
// DRD_WINDOW_MS. If the user presses reset again during that window, the next
// boot sees the flag still armed and reports a double reset. If not, we disarm
// and continue to normal operation. The window is fixed (not "until deep sleep")
// so it doesn't depend on how fast the measurement + POST happens. Scheduled
// timer wakeups return early and never touch NVS, so normal hourly operation
// causes no delay and no flash wear.

#include <Preferences.h>

static Preferences drdPrefs;
static const char *DRD_NAMESPACE = "drd";
static const uint32_t DRD_WINDOW_MS = 3000;   // press again within ~3 s to open the portal

static void drdSetArmed(bool armed) {
  drdPrefs.begin(DRD_NAMESPACE, /*readOnly=*/false);
  drdPrefs.putBool("armed", armed);
  drdPrefs.end();
}

bool detectDoubleReset() {
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    // Scheduled wake — not a manual reset. Don't arm, don't wait, don't write flash.
    return false;
  }

  drdPrefs.begin(DRD_NAMESPACE, /*readOnly=*/true);
  bool armed = drdPrefs.getBool("armed", false);
  drdPrefs.end();

  if (armed) {
    drdSetArmed(false);   // consume it
    Serial.println("Double reset detected — opening setup portal");
    return true;
  }

  // First manual reset: arm and hold the window open for a second press.
  drdSetArmed(true);
  Serial.println("Press reset again within 3 s to open the setup portal...");
  delay(DRD_WINDOW_MS);
  drdSetArmed(false);     // window elapsed with no second press
  return false;
}
