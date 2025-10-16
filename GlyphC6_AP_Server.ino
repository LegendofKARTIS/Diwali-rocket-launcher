#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Rocket Launcher";
const char* password = "12345678";

const int RocketPin = 7;
bool EngineState = false;

AsyncWebServer server(80);

// ------------------- HTML -------------------
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Rocket Launch pad Controller</title>
<style>
:root {--bg-color:#000000;--primary:#FFD700;--secondary:#CCCCCC;--text-color:#FFFFFF;}
body {margin:0;padding:0;font-family:'Segoe UI',sans-serif;background:var(--bg-color);color:var(--text-color);display:flex;flex-direction:column;align-items:center;justify-content:flex-start;min-height:100vh;overflow-x:hidden;}
h1{text-align:center;margin-top:40px;font-size:2.5rem;color:var(--primary);text-shadow:0 0 2px var(--primary),0 0 20px var(--primary);letter-spacing:2px;}
h2{text-align:center;margin-bottom:30px;color:var(--secondary);font-weight:bold;text-shadow:0 0 2px var(--secondary);transition:all 0.3s ease-in-out;}
button{width:160px;height:160px;border:3px solid var(--primary);border-radius:50%;font-size:1.3rem;font-weight:bold;color:var(--text-color);background:#111111;cursor:pointer;box-shadow:0 0 15px var(--primary), inset 0 0 10px #222222;transition:all 0.3s ease;}
button:hover{box-shadow:0 0 25px var(--primary), inset 0 0 15px #333333;transform:scale(1.05);}
button.active{background:var(--primary);color:var(--bg-color);box-shadow:0 0 35px var(--primary), inset 0 0 15px #333333;}
.engineMessage{font-family:'Gill Sans','Gill Sans MT',Calibri,'Trebuchet MS',sans-serif;margin-top:20px;font-size:1.5rem;text-align:center;transition:opacity 0.5s ease-in-out;}
footer{position:absolute;bottom:20px;color:var(--secondary);font-size:0.9rem;text-align:center;width:100%;text-shadow:0 0 5px var(--secondary);}
.line{width:80%;height:2px;background:linear-gradient(90deg,transparent,var(--primary),transparent);margin:10px 0;animation:glowMove 2s linear infinite;}
@keyframes glowMove{0%{background-position:0% 0%;}100%{background-position:100% 0%;}}
</style>
</head>
<body>
<h1>ESP32 Smart Rocket Launcher</h1>
<h2>Engine Status: <span id="statusText">OFF</span></h2>
<p id="engineMessage" class="engineMessage" style="color: var(--primary); font-size: 1.2rem; margin-top: 10px;">Ready to Engage...</p>
<div class="line"></div>
<button id="ledBtn" onclick="toggleLED()">OFF</button>
<div class="line"></div>
<footer>Launch sequence will begin once you press the Button.</footer>

<script>
document.addEventListener("DOMContentLoaded", function() {
    let EngineState = false;
    let timerSequence = [];

    function fadeMessage(newText, delay){
        const message = document.getElementById('engineMessage');
        timerSequence.push(setTimeout(()=>{
            message.style.opacity = 0;
            setTimeout(()=>{
                message.textContent = newText;
                message.style.opacity = 1;
            }, 500);
        }, delay));
    }

    function updateStatusText(){
        const btn = document.getElementById('ledBtn');
        const status = document.getElementById('statusText');
        timerSequence.forEach(clearTimeout);
        timerSequence = [];

        if(EngineState){
            btn.classList.add('active');
            btn.textContent='ON';
            status.textContent='ON';
            fadeMessage("Thrusters Engaged",0);
            fadeMessage("Launching",2000);
            fadeMessage("Launching.",3000);
            fadeMessage("Launching..",4000);
            fadeMessage("Launching...",5000);
            fadeMessage("Launched.",6000);
        } else {
            btn.classList.remove('active');
            btn.textContent='OFF';
            status.textContent='OFF';
            fadeMessage("Ready to Engage...",0);
        }
    }

    // --- Key fix: use fetch to call ESP32 toggle endpoint ---
    function toggleLED(){
        fetch("/toggle")
        .then(response => response.text())
        .then(data => {
            EngineState = (data === "on");
            updateStatusText();
        })
        .catch(err => console.error("Error:", err));
    }

    // Assign button click AFTER DOM loaded
    document.getElementById('ledBtn').addEventListener('click', toggleLED);

    // Initialize UI
    updateStatusText();
});

</script>
</body>
</html>
)rawliteral";
// ------------------- End HTML -------------------

void setup() {
  Serial.begin(115200);
  pinMode(RocketPin, OUTPUT);
  digitalWrite(RocketPin, LOW);

  // Start AP
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", htmlPage);
  });

  // Toggle LED endpoint
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    EngineState = !EngineState;
    digitalWrite(RocketPin, EngineState);
    request->send(200, "text/plain", EngineState ? "on" : "off");
  });

  // Status endpoint
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", EngineState ? "on" : "off");
  });

  server.begin();
}

void loop() {
  // Nothing here, AsyncWebServer handles everything
}
