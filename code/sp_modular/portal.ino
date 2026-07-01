// Commissioning / setup portal.
//
// Opened when there are no saved WiFi credentials, or on a double reset. Lets
// the user edit all runtime settings (see settings.ino) via WiFiManager custom
// parameters, and serves a live sensor-reading page that refreshes every 10 s so
// they can confirm the connected sensors actually work.
//
// The portal runs non-blocking so we can enforce our own 5-minute timeout and
// let a "keep awake" toggle on the live page defeat it for debugging.

#include <WiFiManager.h>
#include <Wire.h>

// Provided by sp_modular.ino — reads every sensor once (no pump action).
SensorPacket readAllSensors();

static const uint32_t PORTAL_TIMEOUT_MS = 5UL * 60UL * 1000UL;  // 5 minutes

static WiFiManager wm;
static volatile bool keepAwake = false;

// Custom parameters (allocated once; the portal runs a single time per boot).
static WiFiManagerParameter *p_deviceName;
static WiFiManagerParameter *p_deviceUuid;
static WiFiManagerParameter *p_apiKey;
static WiFiManagerParameter *p_apiUrl;
static WiFiManagerParameter *p_sleepSec;
static WiFiManagerParameter *p_minMoistV;
static WiFiManagerParameter *p_maxMoistV;
static WiFiManagerParameter *p_battDiv;
static WiFiManagerParameter *p_moistThr;
static WiFiManagerParameter *p_pumpSec;
static WiFiManagerParameter *p_useBme;
static WiFiManagerParameter *p_useTsl;
static WiFiManagerParameter *p_usePump;

static WiFiManagerParameter *makeCheckbox(const char *id, const char *label, bool checked) {
  // WiFiManager checkbox idiom: value "T", pre-check via the "checked" attribute.
  // When submitted unchecked the browser omits the field, so getValue() is empty.
  // WFM_LABEL_AFTER renders the label to the right of the box so it lines up.
  const char *custom = checked ? "type=\"checkbox\" checked" : "type=\"checkbox\"";
  return new WiFiManagerParameter(id, label, "T", 2, custom, WFM_LABEL_AFTER);
}

static bool checkboxChecked(WiFiManagerParameter *p) {
  return strncmp(p->getValue(), "T", 1) == 0;
}

// Persist the edited parameters back into settings/NVS.
static void saveParamsCallback() {
  strlcpy(settings.deviceName, p_deviceName->getValue(), sizeof(settings.deviceName));
  strlcpy(settings.deviceUuid, p_deviceUuid->getValue(), sizeof(settings.deviceUuid));
  strlcpy(settings.apiKey,     p_apiKey->getValue(),     sizeof(settings.apiKey));
  strlcpy(settings.apiUrl,     p_apiUrl->getValue(),     sizeof(settings.apiUrl));

  uint32_t sleepSec = strtoul(p_sleepSec->getValue(), nullptr, 10);
  if (sleepSec > 0) settings.sleepSec = sleepSec;
  settings.minMoistV         = atof(p_minMoistV->getValue());
  settings.maxMoistV         = atof(p_maxMoistV->getValue());
  settings.battDivider       = atof(p_battDiv->getValue());
  settings.moistureThreshold = atof(p_moistThr->getValue());
  settings.pumpDurationSec   = strtoul(p_pumpSec->getValue(), nullptr, 10);

  settings.useBme  = checkboxChecked(p_useBme);
  settings.useTsl  = checkboxChecked(p_useTsl);
  settings.usePump = checkboxChecked(p_usePump);

  saveSettings();
  Serial.println("Portal: settings saved");
}

// --- Live sensor endpoints ---------------------------------------------------

static void appendFloat(String &j, const char *key, float v, uint8_t decimals) {
  j += "\"";
  j += key;
  j += "\":";
  j += isValidFloat(v) ? String((double)v, (unsigned int)decimals) : "null";
  j += ",";
}

static String buildSensorJson() {
  SensorPacket d = readAllSensors();

  String j = "{";
  appendFloat(j, "moisture_pct", d.moistPct, 1);
  appendFloat(j, "moisture_v",   d.moistV,   3);
  appendFloat(j, "battery_v",    d.battV,    2);
  j += "\"rssi\":" + String(d.wifiRssi) + ",";

  j += "\"bme_enabled\":" + String(settings.useBme ? "true" : "false") + ",";
  if (settings.useBme) {
    j += "\"bme_ok\":" + String(isValidFloat(d.temp) ? "true" : "false") + ",";
    appendFloat(j, "temperature", d.temp,  2);
    appendFloat(j, "humidity",    d.hum,   1);
    appendFloat(j, "pressure",    d.press, 1);
  }

  j += "\"tsl_enabled\":" + String(settings.useTsl ? "true" : "false") + ",";
  if (settings.useTsl) {
    j += "\"tsl_ok\":" + String(isValidFloat(d.lux) ? "true" : "false") + ",";
    appendFloat(j, "lux", d.lux, 2);
    j += "\"ir\":" + String(d.ir) + ",";
    j += "\"full\":" + String(d.full) + ",";
  }

  j += "\"keep_awake\":" + String(keepAwake ? "true" : "false");
  j += "}";
  return j;
}

static const char LIVE_PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Live Sensors</title>
<style>
 body{font-family:system-ui,sans-serif;margin:0;background:#0f1115;color:#e8eaed}
 header{padding:16px 20px;background:#1a1d24;font-size:1.2rem;font-weight:600}
 .grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(150px,1fr));gap:12px;padding:16px 20px}
 .card{background:#1a1d24;border:1px solid #2a2e37;border-radius:10px;padding:14px}
 .label{font-size:.72rem;text-transform:uppercase;letter-spacing:.05em;color:#9aa0aa}
 .value{font-size:1.5rem;font-weight:600;margin-top:6px}
 .unit{font-size:.85rem;color:#9aa0aa;margin-left:3px}
 .bad{color:#ff6b6b}
 .foot{padding:12px 20px;color:#9aa0aa;font-size:.85rem;display:flex;align-items:center;gap:10px;flex-wrap:wrap}
 a{color:#7aa2ff}
</style></head><body>
<header>🌱 Live Sensor Readings</header>
<div class="grid" id="grid"></div>
<div class="foot">
 <label><input type="checkbox" id="ka"> Keep portal awake (debug — disables 5&nbsp;min timeout)</label>
 <span id="status">refreshing every 10 s…</span>
 <a href="/">&larr; Back to setup</a>
</div>
<script>
const cards=[
 ["moisture_pct","Moisture","%",1],["moisture_v","Moisture","V",3],
 ["battery_v","Battery","V",2],["rssi","WiFi RSSI","dBm",0],
 ["temperature","Temperature","°C",2],["humidity","Humidity","%",1],
 ["pressure","Pressure","hPa",1],["lux","Light","lux",2],
 ["ir","IR","",0],["full","Full","",0]];
function render(d){
 let h="";
 for(const [k,lab,unit,dec] of cards){
  if(!(k in d))continue;
  let v=d[k];
  let disp=(v===null||v===undefined)?"—":(typeof v==="number"?v.toFixed(dec):v);
  let bad=(v===null||v===undefined)?" bad":"";
  h+=`<div class="card"><div class="label">${lab}</div><div class="value${bad}">${disp}<span class="unit">${unit}</span></div></div>`;
 }
 if(d.bme_enabled&&!d.bme_ok)h+=`<div class="card"><div class="label">BME280</div><div class="value bad">not detected</div></div>`;
 if(d.tsl_enabled&&!d.tsl_ok)h+=`<div class="card"><div class="label">TSL2591</div><div class="value bad">not detected</div></div>`;
 document.getElementById("grid").innerHTML=h;
 document.getElementById("ka").checked=!!d.keep_awake;
}
async function tick(){
 try{const r=await fetch("/sensors.json",{cache:"no-store"});render(await r.json());
  document.getElementById("status").textContent="updated "+new Date().toLocaleTimeString();
 }catch(e){document.getElementById("status").textContent="read error";}
}
document.getElementById("ka").addEventListener("change",async e=>{
 await fetch("/keepawake?v="+(e.target.checked?1:0));
});
tick();setInterval(tick,10000);
</script></body></html>
)HTML";

// Registered after WiFiManager starts its web server.
static void bindCustomRoutes() {
  wm.server->on("/sensors.json", []() {
    wm.server->sendHeader("Cache-Control", "no-store");
    wm.server->send(200, "application/json", buildSensorJson());
  });
  wm.server->on("/keepawake", []() {
    keepAwake = wm.server->arg("v") == "1";
    Serial.println(keepAwake ? "Portal: keep-awake ON" : "Portal: keep-awake OFF");
    wm.server->send(200, "application/json",
                    String("{\"keep_awake\":") + (keepAwake ? "true" : "false") + "}");
  });
  wm.server->on("/livesensors", []() {
    wm.server->send_P(200, "text/html", LIVE_PAGE);
  });
  wm.server->on("/factoryreset", HTTP_POST, []() {
    Serial.println("Portal: factory reset requested");
    resetSettingsToDefaults();
    saveSettings();
    wm.resetSettings();     // also clear saved WiFi credentials
    wm.server->send(200, "text/html",
                    "<!doctype html><meta charset=utf-8>"
                    "<body style='font-family:system-ui;padding:24px'>"
                    "<h3>Factory reset done</h3>"
                    "<p>Settings restored to defaults and WiFi cleared. Rebooting…</p>");
    delay(800);
    ESP.restart();
  });
}

static void buildParameters() {
  static char sleepBuf[12], minBuf[12], maxBuf[12], battBuf[12], thrBuf[12], pumpBuf[12];
  snprintf(sleepBuf, sizeof(sleepBuf), "%u", settings.sleepSec);
  snprintf(minBuf,   sizeof(minBuf),   "%.2f", settings.minMoistV);
  snprintf(maxBuf,   sizeof(maxBuf),   "%.2f", settings.maxMoistV);
  snprintf(battBuf,  sizeof(battBuf),  "%.2f", settings.battDivider);
  snprintf(thrBuf,   sizeof(thrBuf),   "%.1f", settings.moistureThreshold);
  snprintf(pumpBuf,  sizeof(pumpBuf),  "%u", settings.pumpDurationSec);

  p_deviceName = new WiFiManagerParameter("dname", "Device name", settings.deviceName, sizeof(settings.deviceName) - 1);
  p_deviceUuid = new WiFiManagerParameter("duuid", "Device UUID", settings.deviceUuid, sizeof(settings.deviceUuid) - 1);
  p_apiKey     = new WiFiManagerParameter("akey",  "API key",     settings.apiKey,     sizeof(settings.apiKey) - 1);
  p_apiUrl     = new WiFiManagerParameter("aurl",  "API URL",     settings.apiUrl,     sizeof(settings.apiUrl) - 1);
  p_sleepSec   = new WiFiManagerParameter("sleep", "Sleep interval (s)", sleepBuf, 11);
  p_minMoistV  = new WiFiManagerParameter("minv",  "Moisture wet voltage (V)", minBuf, 11);
  p_maxMoistV  = new WiFiManagerParameter("maxv",  "Moisture dry voltage (V)", maxBuf, 11);
  p_battDiv    = new WiFiManagerParameter("bdiv",  "Battery divider ratio", battBuf, 11);
  p_moistThr   = new WiFiManagerParameter("mthr",  "Pump moisture threshold (%)", thrBuf, 11);
  p_pumpSec    = new WiFiManagerParameter("psec",  "Pump duration (s)", pumpBuf, 11);
  p_useBme     = makeCheckbox("ubme",  "Enable BME280 (temp/humidity/pressure)", settings.useBme);
  p_useTsl     = makeCheckbox("utsl",  "Enable TSL2591 (light)", settings.useTsl);
  p_usePump    = makeCheckbox("upump", "Enable watering pump", settings.usePump);

  wm.addParameter(p_deviceName);
  wm.addParameter(p_deviceUuid);
  wm.addParameter(p_apiKey);
  wm.addParameter(p_apiUrl);
  wm.addParameter(p_sleepSec);
  wm.addParameter(p_minMoistV);
  wm.addParameter(p_maxMoistV);
  wm.addParameter(p_battDiv);
  wm.addParameter(p_moistThr);
  wm.addParameter(p_pumpSec);
  wm.addParameter(p_useBme);
  wm.addParameter(p_useTsl);
  wm.addParameter(p_usePump);
}

void runCommissioningPortal() {
  buildParameters();

  wm.setSaveParamsCallback(saveParamsCallback);
  wm.setWebServerCallback(bindCustomRoutes);
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(0);          // we enforce the timeout ourselves
  wm.setBreakAfterConfig(true);          // return control after WiFi is saved
  // Don't let WiFiManager tear the portal down itself when it connects: it frees
  // `server` and, since we manage the shutdown below, a second shutdown would
  // deref the now-null server and crash. We own the portal lifecycle.
  wm.setDisableConfigPortal(false);

  // Add a "Live Sensors" button to the menu (the "custom" slot renders our HTML).
  std::vector<const char *> menu = {"wifi", "param", "custom", "sep", "info", "restart", "exit"};
  wm.setMenu(menu);
  wm.setCustomMenuHTML(
      "<form action='/livesensors' method='get'><button class='D'>Live Sensor Readings</button></form>"
      "<form action='/factoryreset' method='post' "
      "onsubmit='return confirm(\"Erase all settings AND saved WiFi, restore defaults?\")'>"
      "<button style='background:#a12222'>Factory Reset (settings + WiFi)</button></form>");

  String apName = String(settings.deviceName) + "-Setup";
  Serial.println("Portal: starting AP '" + apName + "'");
  wm.startConfigPortal(apName.c_str());

  uint32_t start = millis();
  while (true) {
    wm.process();

    if (WiFi.status() == WL_CONNECTED && wm.getWiFiIsSaved()) {
      Serial.println("Portal: WiFi configured — connected");
      break;
    }
    if (!keepAwake && (millis() - start > PORTAL_TIMEOUT_MS)) {
      Serial.println("Portal: 5-minute timeout reached");
      break;
    }
    delay(10);
  }

  // Only shut down if still active — guards against a double shutdown (which
  // would deref a freed server) in case WiFiManager already closed the portal.
  if (wm.getConfigPortalActive()) {
    wm.stopConfigPortal();
  }
}
