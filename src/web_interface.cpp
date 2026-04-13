#include <WebServer.h>
#include "web_interface.h"
#include "definitions.h"
#include "deauth.h"

WebServer server(80);
int num_networks = 0;

String getEncryptionType(wifi_auth_mode_t encryptionType);

void handle_root() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Deauther - v2</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Roboto:wght@400;500;700&display=swap');
        
        body {
            font-family: 'Roboto', Arial, sans-serif;
            background: linear-gradient(135deg, #1e3c72, #2a5298);
            color: #fff;
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 1000px;
            margin: 0 auto;
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 16px;
            padding: 25px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }
        h1 {
            text-align: center;
            margin-bottom: 10px;
            font-size: 2.2em;
        }
        .status {
            text-align: center;
            font-size: 1.3em;
            margin: 15px 0;
            padding: 12px;
            background: rgba(0,0,0,0.3);
            border-radius: 10px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            background: rgba(255,255,255,0.15);
            border-radius: 10px;
            overflow: hidden;
        }
        th, td {
            padding: 14px;
            text-align: left;
            border-bottom: 1px solid rgba(255,255,255,0.2);
        }
        th {
            background: #0d47a1;
            color: white;
        }
        tr:hover {
            background: rgba(255,255,255,0.1);
        }
        .form-container {
            background: rgba(255,255,255,0.12);
            padding: 20px;
            border-radius: 12px;
            margin: 20px 0;
        }
        input[type="text"], input[type="number"], select {
            width: 100%;
            padding: 12px;
            margin: 8px 0;
            border: none;
            border-radius: 8px;
            background: rgba(255,255,255,0.9);
            color: #333;
            font-size: 16px;
        }
        button, input[type="submit"] {
            background: #00c853;
            color: white;
            border: none;
            padding: 14px 20px;
            font-size: 16px;
            border-radius: 8px;
            cursor: pointer;
            transition: 0.3s;
            width: 100%;
            margin-top: 10px;
        }
        button:hover, input[type="submit"]:hover {
            background: #00b140;
            transform: translateY(-2px);
        }
        .btn-danger {
            background: #f44336;
        }
        .btn-danger:hover {
            background: #d32f2f;
        }
        .btn-scan {
            background: #2196F3;
        }
        .info {
            font-size: 0.95em;
            opacity: 0.9;
        }
        .footer {
            text-align: center;
            margin-top: 30px;
            font-size: 0.9em;
            opacity: 0.7;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Deauther</h1>
        <div class="status">
            <strong>Trạng thái:</strong> 
            <span id="status">Sẵn sàng</span> | 
            Stations bị deauth: <strong id="eliminated">0</strong>
        </div>

        <div class="form-container">
            <h2>🔍 Quét mạng WiFi</h2>
            <button onclick="rescan()" class="btn-scan">Quét lại mạng</button>
        </div>

        <h2>Danh sách mạng WiFi</h2>
        <table id="networkTable">
            <tr>
                <th>#</th>
                <th>SSID</th>
                <th>BSSID</th>
                <th>Channel</th>
                <th>RSSI</th>
                <th>Mã hóa</th>
                <th>Hành động</th>
            </tr>
        </table>

        <div class="form-container">
            <h2>🎯 Tấn công Deauth đơn lẻ</h2>
            <input type="number" id="net_num" placeholder="Số mạng (Network Number)" min="0">
            <input type="number" id="reason" placeholder="Reason Code (mặc định 7)" value="7">
            <button onclick="startSingleDeauth()">Bắt đầu tấn công mạng này</button>
        </div>

        <div class="form-container">
            <h2>🌐 Tấn công tất cả (All Stations)</h2>
            <input type="number" id="all_reason" placeholder="Reason Code" value="7">
            <button onclick="startAllDeauth()" class="btn-danger">Bắt đầu Deauth ALL</button>
        </div>

        <div class="form-container">
            <button onclick="stopDeauth()" class="btn-danger">⛔ Dừng tấn công</button>
        </div>

        <div class="footer">
            <p>ESP32 Deauther - Chỉ dùng để test mạng của bạn • 2026</p>
        </div>
    </div>

    <script>
        function updateTable(networks) {
            let table = document.getElementById("networkTable");
            // Xóa các hàng cũ trừ header
            while (table.rows.length > 1) table.deleteRow(1);

            for (let net of networks) {
                let row = table.insertRow();
                row.innerHTML = `
                    <td>${net.num}</td>
                    <td>${net.ssid}</td>
                    <td>${net.bssid}</td>
                    <td>${net.channel}</td>
                    <td>${net.rssi} dBm</td>
                    <td>${net.enc}</td>
                    <td><button onclick="quickDeauth(${net.num})" style="background:#ff9800; padding:8px 12px; font-size:14px;">Attack</button></td>
                `;
            }
        }

        function rescan() {
            fetch('/rescan').then(() => location.reload());
        }

        function startSingleDeauth() {
            let num = document.getElementById("net_num").value;
            let reason = document.getElementById("reason").value || 7;
            if (num === "") {
                alert("Vui lòng nhập số mạng!");
                return;
            }
            fetch(`/deauth?net_num=${num}&reason=${reason}`)
                .then(() => alert("Đã bắt đầu tấn công mạng #" + num));
        }

        function quickDeauth(num) {
            let reason = 7;
            fetch(`/deauth?net_num=${num}&reason=${reason}`)
                .then(() => alert("Đang tấn công mạng #" + num));
        }

        function startAllDeauth() {
            let reason = document.getElementById("all_reason").value || 7;
            if (confirm("Bạn chắc chắn muốn deauth TẤT CẢ stations?")) {
                fetch(`/deauth_all?reason=${reason}`)
                    .then(() => {
                        document.getElementById("status").innerHTML = "Đang tấn công ALL - Channel hopping";
                        alert("Deauth ALL đã bắt đầu!");
                    });
            }
        }

        function stopDeauth() {
            fetch('/stop').then(() => {
                document.getElementById("status").innerHTML = "Đã dừng";
                alert("Đã dừng tấn công!");
                location.reload();
            });
        }

        // Update eliminated stations realtime mỗi 2 giây
        setInterval(() => {
            fetch('/status')
                .then(response => response.text())
                .then(data => {
                    document.getElementById("eliminated").innerHTML = data;
                })
                .catch(() => {});
        }, 2000);

        // Load danh sách mạng khi mở trang
        window.onload = function() {
            // Ở đây có thể fetch JSON nếu muốn, nhưng giữ đơn giản thì reload là ổn
        };
    </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handle_status() {
  server.send(200, "text/plain", String(eliminated_stations));
}

void handle_deauth() {
  int wifi_number = server.arg("net_num").toInt();
  uint16_t reason = server.arg("reason").toInt() ? server.arg("reason").toInt() : 7;

  if (wifi_number >= 0 && wifi_number < num_networks) {
    start_deauth(wifi_number, DEAUTH_TYPE_SINGLE, reason);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Invalid network number");
  }
}

void handle_deauth_all() {
  uint16_t reason = server.arg("reason").toInt() ? server.arg("reason").toInt() : 7;
  start_deauth(0, DEAUTH_TYPE_ALL, reason);
  server.send(200, "text/plain", "Deauth ALL started");
}

void handle_rescan() {
  num_networks = WiFi.scanNetworks();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handle_stop() {
  stop_deauth();
  server.sendHeader("Location", "/");
  server.send(303);
}

void start_web_interface() {
  server.on("/", HTTP_GET, handle_root);
  server.on("/status", HTTP_GET, handle_status);
  server.on("/deauth", HTTP_GET, handle_deauth);
  server.on("/deauth_all", HTTP_GET, handle_deauth_all);
  server.on("/rescan", HTTP_GET, handle_rescan);
  server.on("/stop", HTTP_GET, handle_stop);

  server.begin();
  DEBUG_PRINTLN("Web interface started at 192.168.4.1");
}

void web_interface_handle_client() {
  server.handleClient();
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Enterprise";
    default: return "Unknown";
  }
}