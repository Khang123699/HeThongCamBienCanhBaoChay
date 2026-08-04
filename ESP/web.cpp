#include <WiFi.h>
#include <WebServer.h>
#include "web.h"
#include "data.h"

WebServer server(80);

const char* ap_ssid     = "HeThongCanhBaoChay";
const char* ap_password = "12345678";

String currentSSID = "NOT_CONNECTED";
String currentIP   = "0.0.0.0";

/* ================= DASHBOARD ================= */
String getDashboardHTML()
{
    return R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dashboard</title>

<style>
body{
    margin:0;
    font-family:Arial;
    background:linear-gradient(135deg,#0f172a,#1e293b);
    color:white;
    padding:20px;
}
.container{max-width:1000px;margin:auto;}
.topbar{
    display:flex;
    justify-content:space-between;
    flex-wrap:wrap;
    gap:10px;
}
.badge{
    background:#334155;
    padding:10px;
    border-radius:8px;
}
button{
    background:#ef4444;
    border:none;
    padding:10px;
    border-radius:8px;
    color:white;
    cursor:pointer;
}
.grid{
    margin-top:20px;
    display:grid;
    grid-template-columns:repeat(auto-fit,minmax(200px,1fr));
    gap:15px;
}
.card{
    background:#1e293b;
    padding:20px;
    border-radius:12px;
    text-align:center;
}
.alert{
    margin-top:20px;
    padding:15px;
    border-radius:12px;
    font-weight:bold;
    text-align:center;
}
@keyframes blink {
  0% {opacity:1;}
  50% {opacity:0.4;}
  100% {opacity:1;}
}
</style>
</head>

<body>

<div class="container">

<div class="topbar">
    <div class="badge">WiFi: <span id="wifiInfo"></span></div>
    <div class="badge">IP: <span id="ipInfo"></span></div>
    <button onclick="changeWifi()">Đổi WiFi</button>
</div>

<div class="grid">
    <div class="card">STATE<br><b id="state"></b></div>
    <div class="card">MQ2<br><b id="mq2"></b></div>
    <div class="card">TEMP<br><b id="temp"></b></div>
    <div class="card">HUM<br><b id="hum"></b></div>
    <div class="card">FIRE<br><b id="fire"></b></div>
</div>

<div id="alertBox" class="alert">
 Đang chờ dữ liệu...
</div>

</div>

<script>
function loadData(){
    fetch("/data")
    .then(r=>r.json())
    .then(d=>{
        document.getElementById("state").innerText=d.state;
        document.getElementById("mq2").innerText=d.mq2;
        document.getElementById("temp").innerText=d.temp;
        document.getElementById("hum").innerText=d.hum;
        document.getElementById("fire").innerText=d.fire;
        document.getElementById("wifiInfo").innerText=d.ssid;
        document.getElementById("ipInfo").innerText=d.ip;

        updateAlert(d.state);
    });
}

function updateAlert(state){
    const box = document.getElementById("alertBox");

    if(state.includes("FIRE") || state.includes("DANGER") || state.includes("LEVEL3")){
        box.style.background = "#7f1d1d";
        box.style.color = "#fecaca";
        box.style.animation = "blink 1s infinite";
        box.innerText = "🚨 NGUY HIỂM: Phát hiện cháy hoặc khí gas cao!";
    }
    else if(state.includes("WARNING") || state.includes("LEVEL1") || state.includes("LEVEL2")){
        box.style.background = "#78350f";
        box.style.color = "#fde68a";
        box.style.animation = "none";
        box.innerText = "⚠️ CẢNH BÁO: Môi trường bất thường!";
    }
    else{
        box.style.background = "#14532d";
        box.style.color = "#bbf7d0";
        box.style.animation = "none";
        box.innerText = "✅ AN TOÀN: Hệ thống hoạt động bình thường.";
    }
}

function changeWifi(){
    fetch("/resetwifi")
    .then(()=>window.location.href="/");
}

setInterval(loadData, 500);
loadData();
</script>

</body>
</html>
)rawliteral";
}

/* ================= CONFIG WIFI ================= */
String getRootHTML()
{
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Cấu hình WiFi</title>

<style>
body{
    margin:0;
    font-family:Arial;
    background:linear-gradient(135deg,#0f172a,#1e293b);
    color:white;
    display:flex;
    align-items:center;
    justify-content:center;
    height:100vh;
}
.card{
    width:100%;
    max-width:400px;
    background:#1e293b;
    padding:20px;
    border-radius:15px;
}
h2{text-align:center;}
select,input{
    width:100%;
    padding:10px;
    margin-top:10px;
    border-radius:8px;
    border:none;
}
button{
    width:100%;
    padding:12px;
    margin-top:15px;
    background:#22c55e;
    color:white;
    border:none;
    border-radius:8px;
}
</style>
</head>

<body>

<div class="card">
<h2>Kết nối WiFi</h2>

<form action="/connect" method="POST">
<select name="ssid">
)rawliteral";

    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; i++)
    {
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);

        String signal = "";
        if (rssi > -60) signal = "";
        else if (rssi > -80) signal = "";

        html += "<option value=\"" + ssid + "\">";
        html += signal + " " + ssid;
        html += "</option>";
    }

    html += R"rawliteral(
</select>

<input type="password" name="password" placeholder="Mật khẩu">

<button type="submit">Kết nối</button>
</form>
</div>

</body>
</html>
)rawliteral";

    return html;
}

/* ================= WIFI ================= */
bool connectToWiFi(String ssid, String pass)
{
    WiFi.disconnect(true, true);
    delay(500);

    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
    {
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        currentSSID = ssid;
        currentIP   = WiFi.localIP().toString();
        return true;
    }

    return false;
}

/* ================= HANDLERS ================= */
void handleRoot()
{
    if (WiFi.status() == WL_CONNECTED)
        server.send(200, "text/html", getDashboardHTML());
    else
        server.send(200, "text/html", getRootHTML());
}

void handleConnect()
{
    String ssid = server.arg("ssid");
    String pass = server.arg("password");

    if (connectToWiFi(ssid, pass))
    {
        server.sendHeader("Location", "/");
        server.send(302);
    }
    else
    {
        server.send(200, "text/html", "Loi ket noi");
    }
}

void handleResetWiFi()
{
    WiFi.disconnect(true, true);
    currentSSID = "NOT_CONNECTED";
    currentIP   = "0.0.0.0";
    server.send(200, "text/plain", "OK");
}

void handleData()
{
    String json = "{";
    json += "\"state\":\"" + trangThai + "\",";
    json += "\"mq2\":" + String(mq2) + ",";
    json += "\"temp\":" + String(temp) + ",";
    json += "\"hum\":" + String(hum) + ",";
    json += "\"fire\":" + String(fire) + ",";
    json += "\"ssid\":\"" + currentSSID + "\",";
    json += "\"ip\":\"" + currentIP + "\"";
    json += "}";

    server.send(200, "application/json", json);
}

/* ================= INIT ================= */
void Web_Init()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_password);

    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.on("/data", handleData);
    server.on("/resetwifi", handleResetWiFi);

    server.begin();
}

void Web_HandleClient()
{
    server.handleClient();
}