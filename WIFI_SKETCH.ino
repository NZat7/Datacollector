#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_SSID";       // Replace with your network name
const char* password = "YOUR_PASSWORD"; // Replace with your network password

ESP8266WebServer server(80);

// HTML, CSS, and JS web application stored in flash memory
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>DACO CONTROL</title>
    <style>
        body {
            background-color: #ffffff;
            color: #000000;
            font-family: "Courier New", Courier, monospace;
            text-align: center;
            margin-top: 50px;
        }
        canvas {
            border: 1px solid #000000;
            background-color: #ffffff;
            margin-top: 20px;
        }
        .box {
            margin: 15px;
        }
        input[type="text"] {
            font-family: "Courier New", Courier, monospace;
            padding: 5px;
            font-size: 16px;
        }
        input[type="submit"] {
            font-family: "Courier New", Courier, monospace;
            padding: 6px 12px;
            font-size: 16px;
        }
    </style>
</head>
<body>

    <h2>DACO CONTROL</h2>
    
    <form action="/run" method="POST">
        <div class="box">
            Sequence: 
            <input type="text" id="seqInput" name="seq" oninput="updatePath()" autocomplete="off">
            <input type="submit" value="Send">
        </div>
    </form>
    <div class="box" style="font-size: 14px;">
        <strong>Commands:</strong> 0 = Forward, 1 = Turn Right, 2 = Turn Left, 3 = Backward, 4 = Collect Soil Data
    </div>

    <div class="box">
        <!-- The visual box where vectors are drawn -->
        <canvas id="pathCanvas" width="600" height="600"></canvas>
    </div>

    <script>
        // Legend for commands:
        // 0 = Forwards
        // 1 = Turn Right
        // 2 = Turn Left
        // 3 = Backwards
        // 4 = Collect Soil Data

        function updatePath() {
            const canvas = document.getElementById('pathCanvas');
            const ctx = canvas.getContext('2d');
            const inputVal = document.getElementById('seqInput').value;

            // Clear canvas
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            // Starting position & orientation
            let x = canvas.width / 2;
            let y = canvas.height / 2;
            let angle = 0; 
            const stepLength = 30;

            // Arrays to track path and soil data collection points
            const points = [{ x, y }];
            const collectionPoints = [];

            // 1. Process movement sequence
            for (let i = 0; i < inputVal.length; i++) {
                let char = inputVal[i];

                if (char === '1') {
                    angle += 90;
                } else if (char === '2') {
                    angle -= 90;
                } else if (char === '4') {
                    // Record point for collection marker
                    collectionPoints.push({ x, y });
                } else if (char === '0' || char === '3') {
                    let rad = (angle - 90) * (Math.PI / 180);
                    let dx = Math.cos(rad) * stepLength;
                    let dy = Math.sin(rad) * stepLength;

                    if (char === '0') {
                        x += dx;
                        y += dy;
                    } else if (char === '3') {
                        x -= dx;
                        y -= dy;
                    }
                    points.push({ x, y });
                }
            }

            // 2. Draw black starting marker
            ctx.fillStyle = 'black';
            ctx.fillRect(points[0].x - 3, points[0].y - 3, 6, 6);

            // 3. Draw black path lines
            ctx.strokeStyle = 'black';
            ctx.lineWidth = 3;
            ctx.beginPath();
            ctx.moveTo(points[0].x, points[0].y);
            for (let i = 1; i < points.length; i++) {
                ctx.lineTo(points[i].x, points[i].y);
            }
            ctx.stroke();

            // 4. Draw red collection dots ON TOP of the black lines
            ctx.fillStyle = 'red';
            for (let pt of collectionPoints) {
                ctx.fillRect(pt.x - 4, pt.y - 4, 8, 8);
            }

            // 5. Draw final position marker
            ctx.fillStyle = 'black';
            ctx.fillRect(x - 4, y - 4, 8, 8);
        }
    </script>

</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleRun() {
  if (server.hasArg("seq")) {
    String sequence = server.arg("seq");
    Serial.println(sequence);
    
    // Redirect back to root after submitting command
    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on("/", handleRoot);
  server.on("/run", HTTP_POST, handleRun);
  server.begin();
}

void loop() {
  server.handleClient();
}
