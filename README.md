# ESP32 Deauther

![PlatformIO](https://img.shields.io/badge/PlatformIO-Framework-blue.svg)
![License](https://img.shields.io/badge/License-GNU%20GPL%20v3-green.svg)
![ESP32](https://img.shields.io/badge/ESP32-2.4GHz_Deauth-orange.svg)

Công cụ **Deauth WiFi** mạnh mẽ dành cho ESP32.  
Cho phép quét và ngắt kết nối (deauthenticate) các thiết bị đang kết nối vào mạng WiFi **2.4GHz**.

> **⚠️ DISCLAIMER**  
> Project này chỉ phục vụ mục đích **học tập, nghiên cứu và kiểm tra bảo mật mạng của chính bạn**.  
> Mọi hành vi sử dụng để gây hại, phá sóng người khác hoặc hoạt động bất hợp pháp đều **bị nghiêm cấm**.  
> Tôi **không chịu bất kỳ trách nhiệm nào** phát sinh từ việc lạm dụng tool này.

---

## Tính năng

- Giao diện Web hiện đại, responsive (dùng tốt trên điện thoại)
- Quét mạng WiFi xung quanh nhanh chóng
- Tấn công Deauth một mạng cụ thể
- Tấn công Deauth tất cả stations (channel hopping)
- Hiển thị **realtime** số stations bị deauth
- Hỗ trợ nhiều board ESP32 (DOIT, S3, C3, C5...)
- LED nháy khi gửi gói deauth thành công

**Lưu ý quan trọng**: Chỉ hoạt động hiệu quả trên băng tần **2.4GHz**. Không hỗ trợ 5GHz trên ESP32 thông thường.

---

## Hướng dẫn cài đặt

### Cách 1: PlatformIO (Khuyến nghị)

1. Cài **Visual Studio Code** + extension **PlatformIO**
2. Clone repository:
   ```bash
   git clone https://github.com/khanh2k7/ESP32_DEAUTHER.git

## Cách sử dụng

### 1. Kết nối với ESP32 Deauther

Sau khi nạp code, ESP32 sẽ tạo một mạng WiFi mặc định:

- **Tên WiFi (SSID)**: `ESP32-Deauther`
- **Mật khẩu**: `esp32wroom32`

Kết nối vào mạng này bằng điện thoại hoặc máy tính.

### 2. Truy cập giao diện điều khiển

Mở trình duyệt và truy cập địa chỉ:

→ **http://192.168.4.1**

### 3. Các chức năng chính

- **Quét lại mạng**: Nhấn nút "Quét lại mạng" để hiển thị danh sách WiFi xung quanh.
- **Tấn công một mạng**: Nhập số thứ tự của mạng + Reason Code (thường dùng **7**) → bấm "Bắt đầu tấn công mạng này".
- **Tấn công tất cả**: Nhấn nút **Deauth ALL**.
- **Dừng tấn công**: Nhấn nút **Dừng tất cả tấn công**.

LED trên board sẽ nháy mỗi khi gửi gói deauth thành công.

---

## Thay đổi tên WiFi và mật khẩu (Rất quan trọng)

Nếu bạn muốn đổi tên WiFi và mật khẩu khác, hãy làm theo các bước sau:

1. Mở file **`include/definitions.h`**
2. Sửa hai dòng sau cho phù hợp:

```cpp
#define AP_SSID "ESP32-Deauther"      // ← Thay tên WiFi ở đây
#define AP_PASS "esp32wroom32"        // ← Thay mật khẩu ở đây