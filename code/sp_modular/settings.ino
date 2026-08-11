// Runtime settings persisted in NVS/Preferences.
//
// On first boot (or after resetSettingsToDefaults() + saveSettings()), the
// DEFAULT_* values from config.h are written into flash. Thereafter the setup
// portal edits these and calls saveSettings(). config.h no longer needs editing
// per device — which is what lets a single web-flashed binary be reconfigured.

#include <Preferences.h>

// Global settings instance (declared extern in config.h).
Settings settings;

static Preferences prefs;

static const char *NVS_NAMESPACE = "sp";
// Bump when the Settings layout changes so stale flash is re-initialised from defaults.
static const uint32_t SETTINGS_VERSION = 3;

void resetSettingsToDefaults() {
  // Derive a unique per-device id from the factory-programmed MAC in efuse.
  // Stable across reboots/reflashes/factory-reset and unique per board, so
  // students' setup APs and dashboard entries never collide. The prefix keeps
  // the id recognisable on a shared public instance; the API only accepts
  // [A-Za-z0-9_-], which both parts satisfy.
  uint32_t uid = (uint32_t)ESP.getEfuseMac();   // lower 32 bits of the 48-bit MAC
  char uidStr[9];
  snprintf(uidStr, sizeof(uidStr), "%08x", uid);

  snprintf(settings.deviceName, sizeof(settings.deviceName), "%s-%s", DEFAULT_DEVICE_NAME, uidStr);
  snprintf(settings.deviceId, sizeof(settings.deviceId), "%s-%s", DEFAULT_DEVICE_ID_PREFIX, uidStr);
  strlcpy(settings.project,    DEFAULT_PROJECT,     sizeof(settings.project));
  strlcpy(settings.apiKey,     DEFAULT_API_KEY,     sizeof(settings.apiKey));
  strlcpy(settings.apiUrl,     DEFAULT_API_URL,     sizeof(settings.apiUrl));
  settings.sleepSec          = DEFAULT_TIME_TO_SLEEP_SEC;
  settings.minMoistV         = DEFAULT_MIN_MOIST_V;
  settings.maxMoistV         = DEFAULT_MAX_MOIST_V;
  settings.battDivider       = DEFAULT_BATT_DIVIDER;
  settings.moistureThreshold = DEFAULT_MOISTURE_THRESHOLD;
  settings.pumpDurationSec   = DEFAULT_PUMP_DURATION_SEC;
  settings.useBme            = DEFAULT_USE_BME280;
  settings.useTsl            = DEFAULT_USE_TSL2591;
  settings.usePump           = DEFAULT_USE_PUMP;
}

void loadSettings() {
  prefs.begin(NVS_NAMESPACE, /*readOnly=*/true);
  uint32_t version = prefs.getUInt("version", 0);

  if (version != SETTINGS_VERSION) {
    // First boot or layout changed — seed flash with defaults.
    prefs.end();
    Serial.println("Settings: initialising NVS from defaults");
    resetSettingsToDefaults();
    saveSettings();
    return;
  }

  prefs.getString("deviceName", settings.deviceName, sizeof(settings.deviceName));
  prefs.getString("deviceId",   settings.deviceId,   sizeof(settings.deviceId));
  prefs.getString("project",    settings.project,    sizeof(settings.project));
  prefs.getString("apiKey",     settings.apiKey,     sizeof(settings.apiKey));
  prefs.getString("apiUrl",     settings.apiUrl,     sizeof(settings.apiUrl));
  settings.sleepSec          = prefs.getUInt("sleepSec",   DEFAULT_TIME_TO_SLEEP_SEC);
  settings.minMoistV         = prefs.getFloat("minMoistV", DEFAULT_MIN_MOIST_V);
  settings.maxMoistV         = prefs.getFloat("maxMoistV", DEFAULT_MAX_MOIST_V);
  settings.battDivider       = prefs.getFloat("battDiv",   DEFAULT_BATT_DIVIDER);
  settings.moistureThreshold = prefs.getFloat("moistThr",  DEFAULT_MOISTURE_THRESHOLD);
  settings.pumpDurationSec   = prefs.getUInt("pumpSec",    DEFAULT_PUMP_DURATION_SEC);
  settings.useBme            = prefs.getBool("useBme",     DEFAULT_USE_BME280);
  settings.useTsl            = prefs.getBool("useTsl",     DEFAULT_USE_TSL2591);
  settings.usePump           = prefs.getBool("usePump",   DEFAULT_USE_PUMP);
  prefs.end();

  Serial.println("Settings: loaded from NVS");
}

void saveSettings() {
  prefs.begin(NVS_NAMESPACE, /*readOnly=*/false);
  prefs.putString("deviceName", settings.deviceName);
  prefs.putString("deviceId",   settings.deviceId);
  prefs.putString("project",    settings.project);
  prefs.putString("apiKey",     settings.apiKey);
  prefs.putString("apiUrl",     settings.apiUrl);
  prefs.putUInt("sleepSec",   settings.sleepSec);
  prefs.putFloat("minMoistV", settings.minMoistV);
  prefs.putFloat("maxMoistV", settings.maxMoistV);
  prefs.putFloat("battDiv",   settings.battDivider);
  prefs.putFloat("moistThr",  settings.moistureThreshold);
  prefs.putUInt("pumpSec",    settings.pumpDurationSec);
  prefs.putBool("useBme",     settings.useBme);
  prefs.putBool("useTsl",     settings.useTsl);
  prefs.putBool("usePump",    settings.usePump);
  prefs.putUInt("version",    SETTINGS_VERSION);
  prefs.end();

  Serial.println("Settings: saved to NVS");
}
