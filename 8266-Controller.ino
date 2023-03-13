#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <Wire.h> //定义iic的引脚
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCL_PIN 4
#define SDA_PIN 5
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define STA 1 // 连接热点
#define AP 0  // 作为热点
const char *ssid = "TEST-WIFI";
const char *password = "12345678"; // 名字密码
const char *sta_ssid = "blue";
const char *sta_password = "12345678";

int index1;
int index2;
int index3;
int index4;
int index5;
int index6;
int index7;

String joy_x;
String joy_y;
String but_a;
String but_b;
String but_x;
String but_y;
String but_l;
String but_r;

uint8_t send_buff[20];

WiFiUDP Udp; // 创建UDP对象

int open_flag = 0; // ap开启成功失败标志位

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

void draw_text(char *str);

void setup()
{
    Serial.begin(9600);

    pinMode(2, OUTPUT); // 指示灯
    digitalWrite(2, LOW);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    /*-----初始化------*/
    display.display();
    delay(2000); // Pause for 2 seconds

    // Clear the buffer
    display.clearDisplay();

#if AP

    WiFi.mode(WIFI_AP);

    WiFi.softAPConfig(local_IP, gateway, subnet);
    Serial.printf("设置接入点中 ... ");
    WiFi.softAP(ssid, password, 3, 1);       // 启动AP网络
    open_flag = WiFi.softAP(ssid, password); // 监控状态变量result
    if (open_flag)
    {
        Serial.println("开启成功");
        digitalWrite(2, LOW);
    }
    else
    {
        Serial.println("开启失败");
    }
#endif
/**---------------------TCP Server端初始化---------------------------**/
#if STA
    WiFi.mode(WIFI_STA);
    /*------------------连接网络-------------------*/
    WiFi.begin(sta_ssid, sta_password);

    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 30);
    display.print("Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
        display.print(".");
        Serial.print(".");
        display.display();
        delay(500);
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());

    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 30);            // Start at top-left corner
    display.println(WiFi.localIP());
    display.display();

#endif

    Udp.begin(3000); // 服务器启动监听端口号
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
}

void loop()
{
    // display.display();
    delay(20);
    // Serial.println(WiFi.localIP());
    //     Serial.println(WiFi.softAPgetStationNum()); //打印客户端连接

    int packetSize = Udp.parsePacket(); // 获取当前队首数据包长度
    if (packetSize)                     // 如果有数据可用
    {
        digitalWrite(2, LOW);
        char buf[packetSize + 1];
        String rx;
        Udp.read(buf, packetSize); // 读取当前包数据
        rx = buf;

        index1 = rx.indexOf(':', 0);
        index2 = rx.indexOf(':', index1 + 1);
        index3 = rx.indexOf(':', index2 + 1);
        index4 = rx.indexOf(':', index3 + 1);
        index5 = rx.indexOf(':', index4 + 1);
        index6 = rx.indexOf(':', index5 + 1);
        index7 = rx.indexOf(':', index6 + 1);

        joy_x = rx.substring(0, index1);
        joy_y = rx.substring(index1 + 1, index2);
        but_a = rx.substring(index2 + 1, index3);
        but_b = rx.substring(index3 + 1, index4);
        but_x = rx.substring(index4 + 1, index5);
        but_y = rx.substring(index5 + 1, index6);
        but_l = rx.substring(index6 + 1, index7);
        but_r = rx.substring(index7 + 1, packetSize + 1);

        send_buff[0] = 0xA5;
        send_buff[1] = joy_x.toInt();
        send_buff[2] = joy_y.toInt();
        send_buff[3] = (but_a.toInt() | (but_b.toInt() << 1) | but_x.toInt() << 2 | but_y.toInt() << 3 | but_l.toInt() << 4 | but_r.toInt() << 5);
        send_buff[4] = 0x55;

        Serial.write(send_buff, 5);

        // Serial.println(joy_x.toInt());
        // Serial.println(joy_y.toInt());
        // Serial.println(send_buff[3]);

        // Serial.print("Received: ");
        // Serial.println(rx);
        // Serial.print("From IP: ");
        // Serial.println(Udp.remoteIP());
        // Serial.print("From Port: ");
        // Serial.println(Udp.remotePort());

        // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //准备发送数据
        // Udp.print("Received: ");                           //复制数据到发送缓存
        // Udp.write((const uint8_t *)buf, packetSize);       //复制数据到发送缓存
        // Udp.endPacket();                                   //发送数据
    }
}

void draw_text(char *str)
{
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println(str);
    display.display(); // Show initial text
    delay(100);
}
