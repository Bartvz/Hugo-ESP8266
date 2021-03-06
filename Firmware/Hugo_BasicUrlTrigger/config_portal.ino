
void startConfigPortal() {
  startBlinking(CONFIG_BLINK_SPEED);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  IPAddress ap_ip(10, 10, 10, 1);
  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
  String ap_name = AP_NAME + macLastThreeSegments(mac);
  WiFi.softAP(ap_name.c_str());
  IPAddress ip = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(ip);
  server.on("/", handleRoot);
  server.begin();

  delay(3000);

  while (deviceMode == CONFIG_MODE) { // BLOCKING INFINITE LOOP
    if (readButtons() > 0 || millis() - configTimer > CONFIG_TIMEOUT) {
      stopBlinking();
      goToSleep();
      return;
    }
    server.handleClient();
  }

}

void toggleConfigMode() {
  if (digitalRead(button1_pin) == HIGH && digitalRead(button4_pin) == HIGH) {
    int i = 0;
    while (digitalRead(button1_pin) == HIGH && digitalRead(button4_pin) == HIGH && i < 200) {
      delay(10);

      if (i > 100) {
        deviceMode = CONFIG_MODE;
        configTimer = millis(); // start counter
        return;
      }
      i++;
    }
  }
}

void handleRoot() {
  if (server.args()) {

    char *keys[] = {
      "id",
      "pw",
      "ip",
      "gw",
      "sn",
      "b1",
      "b2",
      "b3",
      "b4",
      "b5",
      "b6",
      "b7"
    };
    for (int i = 0; i < 12; i++) {
      if (server.hasArg(keys[i])) {
        json[keys[i]] = server.arg(keys[i]);
      }
    }
    /*
    if (server.hasArg("ssid")) {
      json["ssid"] = server.arg("ssid");
    }
    if (server.hasArg("pass")) {
      json["pass"] = server.arg("pass");
    }
    if (server.hasArg("ip")) {
      json["ip"] = server.arg("ip");
    }
    if (server.hasArg("gw")) {
      json["gw"] = server.arg("gw");
    }
    if (server.hasArg("sn")) {
      json["sn"] = server.arg("sn");
    }
    if (server.hasArg("b1")) {
      json["b1"] = server.arg("b1");
    }
    if (server.hasArg("b2")) {
      json["b2"] = server.arg("b2");
    }
    if (server.hasArg("b3")) {
      json["b3"] = server.arg("b3");
    }
    if (server.hasArg("b4")) {
      json["b4"] = server.arg("b4");
    }
    if (server.hasArg("b5")) {
      json["b5"] = server.arg("b5");
    }
    if (server.hasArg("b6")) {
      json["b6"] = server.arg("b6");
    }
    if (server.hasArg("b7")) {
      json["b7"] = server.arg("b7");
    }
    */
    saveConfig();
  }

  const char* batteryColor = "#a53e3e"; // default RED
  int batteryPercent = batteryPercentage;
  if (batteryPercent >= 40) batteryColor = "#a57d3e";
  if (batteryPercent >= 60) batteryColor = "#9ea53e";
  if (batteryPercent >= 80) batteryColor = "#7ca53e";
  if (batteryPercent > 100) batteryColor = "#3e7ea5";

  String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Hugo Configuration</title><style>html,body{margin:0;padding:0;font-size:16px;background:#444;}body,*{box-sizing:border-box;font-family:-apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,sans-serif;}a{color:inherit;text-decoration:underline;}.wrapper{padding:30px 0;}.c{margin:auto;padding:40px;max-width:500px;color:#fff;background:#000;box-shadow:0 0 100px rgba(0,0,0,.5);border-radius:50px;}.r{margin-bottom:15px;}h1{margin:0 0 10px 0;font-family:Arial,sans-serif;font-weight:300;font-size:2rem;}h1 + p{margin-bottom:30px;}h2{margin:30px 0 0 0;font-family:Arial,sans-serif;font-weight:300;font-size:1.5rem;}p{font-size:.85rem;margin:0 0 20px 0;color:rgba(255,255,255,.7);}label{display:block;width:100%;margin-bottom:5px;}input[type=\"text\"],input[type=\"password\"]{display:inline-block;width:100%;height:42px;line-height:38px;padding:0 20px;color:#fff;border:2px solid #666;background:none;border-radius:5px;transition:.15s;box-shadow:none;outline:none;}input[type=\"text\"]:hover,input[type=\"password\"]:hover{border-color:#ababab;}input[type=\"text\"]:focus,input[type=\"password\"]:focus{border-color:#fff;}button{display:block;width:100%;padding:10px 20px;font-size:1rem;font-weight:700;text-transform:uppercase;background:#ff9c29;border:0;border-radius:5px;cursor:pointer;transition:.15s;outline:none;}button:hover{background:#ffba66;}.github{margin-top:15px;text-align:center;}.github a{color:#ff9c29;transition:.15s;}.github a:hover{color:#ffba66;}.bat p{margin:0 0 5px 0;text-align:center;text-transform:uppercase;font-size:.8rem;}.bat >div{position:relative;margin:0 auto 20px;width:300px;height:10px;background:#272727;border-radius:5px;}.bat >div >div{position:absolute;left:0;top:0;bottom:0;border-radius:5px;min-width:10px;}.mac{display:inline-block;margin-top:8px;padding:2px 5px;color:#fff;background:#444;border-radius:3px;}</style><style media=\"all and (max-width:520px)\">.wrapper{padding:20px 0;}.c{padding:25px 15px;border-radius:0;}</style></head><body><div class=\"wrapper\">";
  html += "<div class=\"bat\"><p>Battery level: " + ((batteryPercent > 100) ? "Charging" : (String)batteryPercent + "%") + "</p><div><div style=\"background: " + batteryColor + ";width: " + ((batteryPercent > 100) ? 100 : batteryPercent) + "%\"></div></div></div>";
  html += "<div class=\"c\"><form method=\"post\" action=\"/\"><h1>Hugo Configuration</h1><p>Press any of the Hugo's buttons to shut down config AP and resume normal function.</p><h2>Network settings</h2><p>Set your network credentials here.</p><div class=\"r\"><label for=\"id\">WiFi SSID</label><input type=\"text\" id=\"id\" name=\"id\" value=\"";
  html += json["id"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"pw\">WiFi Password</label><input type=\"password\" id=\"pw\" name=\"pw\" value=\"";
  html += json["pw"].as<const char*>();
  html += "\"></div><h2>Static IP settings (optional)</h2><p>In some cases this might speed up response time. All 3 need to be set and IP should be reserved in router's DHCP settings.";
  html += "<br>MAC address: <span class=\"mac\">";
  html += macToStr(mac);
  html += "</span></p>";
  html += "<div class=\"r\"><label for=\"ip\">IP Address (optional):</label><input type=\"text\" id=\"ip\" name=\"ip\" value=\"";
  html += json["ip"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"gw\">Gateway IP (optional):</label><input type=\"text\" id=\"gw\" name=\"gw\" value=\"";
  html += json["gw"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"sn\">Subnet mask (optional):</label><input type=\"text\" id=\"sn\" name=\"sn\" value=\"";
  html += json["sn"].as<const char*>();
  html += "\"></div>";
  html += "<h2>Buttons settings</h2><p>Assign target URL for each button.<br>Use <b>[blvl]</b> shortcode to add battery percentage.<br>Use <b>[mac]</b> to add mac address (as unique identifier).<br>For example: \"http://example.com/?trigger=button1&battery_percentage=[blvl]&mac=[mac]\"</p>";
  html += "<div class=\"r\"><label for=\"b1\">Button 1 url</label><input type=\"text\" id=\"b1\" name=\"b1\" value=\"";
  html += json["b1"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"b2\">Button 2 url</label><input type=\"text\" id=\"b2\" name=\"b2\" value=\"";
  html += json["b2"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"b3\">Button 3 url</label><input type=\"text\" id=\"b3\" name=\"b3\" value=\"";
  html += json["b3"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"b4\">Button 4 url</label><input type=\"text\" id=\"b4\" name=\"b4\" value=\"";
  html += json["b4"].as<const char*>();
  html += "\"></div>";
  html += "<h2>Button combinations</h2><p>Push two neighbouring buttons together and have them act as a virtual button.</p>";
  html += "<div class=\"r\"><label for=\"b5\">B1+B2 url</label><input type=\"text\" id=\"b5\" name=\"b5\" value=\"";
  html += json["b5"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"b6\">B2+B3 url</label><input type=\"text\" id=\"b6\" name=\"b6\" value=\"";
  html += json["b6"].as<const char*>();
  html += "\"></div><div class=\"r\"><label for=\"b7\">B3+B4 url</label><input type=\"text\" id=\"b7\" name=\"b7\" value=\"";
  html += json["b7"].as<const char*>();
  html += "\"></div>";
  html += "<div class=\"r\"><button type=\"submit\">Save and reboot</button></div></form></div>";
  html += "<div class=\"github\"><p>";
  html += SKETCH;
  html += " ";
  html += FW_VERSION;
  html += ", check out <a href=\"https://git.io/Jezc2\" target=\"_blank\"><strong>Hugo</strong> on GitHub</a></p></div>";
  html += "</div></body></html>";
  server.send(200, "text/html", html);

  if (server.args()) {
    delay(1000);
    ESP.reset();
    delay(100);
  }
}
