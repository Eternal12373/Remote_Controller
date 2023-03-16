#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <Wire.h> //定义iic的引脚
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LunarLobster 0
#define KineticKudu 0
#define JammyJellyfish 1
#define ImpishIndri 0

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCL_PIN 4
#define SDA_PIN 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char *ssid = "TEST-WIFI";
const char *password = "12345678"; // 名字密码

#if LunarLobster
const char *sta_ssid = "LunarLobster";
const char *sta_password = "12345678";
#endif
#if KineticKudu
const char *sta_ssid = "KineticKudu";
const char *sta_password = "12345678";
#endif
#if JammyJellyfish
const char *sta_ssid = "JammyJellyfish";
const char *sta_password = "12345678";
#endif
#if ImpishIndri
const char *sta_ssid = "ImpishIndri";
const char *sta_password = "12345678";
#endif

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

WiFiUDP Udp;

int open_flag = 0;

void setup()
{
    Serial.begin(9600);

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.display();
    delay(2000);
    display.clearDisplay();
    /**---------------------TCP Server Init---------------------------**/
    WiFi.mode(WIFI_STA);
    /*------------------connect to the internet-------------------*/
    WiFi.begin(sta_ssid, sta_password);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(sta_password);
    display.println(sta_ssid);
    display.print("Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
        display.display();
        delay(500);
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());

    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 30);
    display.println(WiFi.localIP());
    display.display();

    Udp.begin(3000);
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
}

void loop()
{
    static int err_cnt = 0;
    delay(5);
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        err_cnt = 0;
        digitalWrite(2, HIGH);
        char buf[packetSize + 1];
        String rx;
        Udp.read(buf, packetSize);
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
    }
    else
    {
        err_cnt++;
        if (err_cnt > 200)
            digitalWrite(2, LOW);
    }
}
