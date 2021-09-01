
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

/* 
cari di library manager, 
"Firebase Arduino Client Library for ESP826 and ESP32" by Mobizt version 2.3.7
*/
#include <Firebase_ESP_Client.h>
#include "time.h"
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = (7 * 60 * 60);
const int daylightOffset_sec = 0;
//Provide the token generation process info.
#include "addons/TokenHelper.h"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Workshop Elka"
#define WIFI_PASSWORD "gapakekabel"

/* 2. Define the API Key */
#define API_KEY "AIzaSyBUzQFzg_PG8e2rvijLxvnfyyGXiGvnP3M"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "pkm-um-tensys"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "esp8266@device.id"
#define USER_PASSWORD "82668266"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
String user_ID = "TvSl0HuLK3Zhc6CI0vWR"; //user ID
String path = "";
unsigned long dataMillis = 0;
int count = 0;
void setup()
{
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    Firebasebegin();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
    // contoh dari nilai sensor
    float sistolik = 100;
    float diastolik = 90;
    float denyutNadi = 120;
    firebaseDataSend(sistolik, diastolik, denyutNadi);
}

void Firebasebegin()
{
    /* Assign the api key (required) */

    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);

    if (Firebase.ready())
    {
        Serial.println('firebase sudah siap');
    }
}

void firebaseDataSend(float sistolik, float diastolik, float denyutNadi)
{
    String content;
    FirebaseJson js;
    unsigned long epoch = getTime();
    String path = "users/" + user_ID +"/riwayat/"+ String(epoch,HEX);
    js.set("fields/sistolik/doubleValue", sistolik);
    js.set("fields/diastolik/doubleValue", diastolik);
    js.set("fields/denyutNadi/doubleValue", denyutNadi);
    js.set("fields/timestamp/timestampValue", printLocalTime());
    js.toString(content);
    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, path.c_str(), content.c_str()))
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    else
        Serial.println(fbdo.errorReason());
}

String printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "";
    }
    String readableDate = String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon) + "-" + String(timeinfo.tm_mday) + "T" + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec)+String("+07:00");
    return readableDate;
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}
