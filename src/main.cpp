#include <Arduino.h>
#include <SPI.h>

#include "Ethernet.h"


#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceID3.h"


AudioGeneratorMP3* mp3;
AudioFileSourceSD* file;
AudioOutputI2S* out;
AudioFileSourceID3* id3;


//SPI Define SDCard && Ethernet
#define SCK         11
#define MOSI        3
#define MISO        12


//SDCard
#define CS          2


//Ethernet
#define SS          13


//I2S Audio
#define I2S_BCLK    10
#define I2S_LRC     45
#define I2S_DOUT    9
#define PA_CAL      46

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(173,194,33,104); // Google


//Function_Test
//Audio
void audioInitialize();
void audioLoop();

//SDCard
void sdCardInitialize();

EthernetClient client;


/**
 * Setup Program
 **/
void setup()
{
    Serial.begin(115200);

    pinMode(SS, OUTPUT);

    pinMode(CS, OUTPUT);


    SPI.begin(SCK, MISO, MOSI);
    //SPI.begin(SCK, MISO, MOSI,CS);
    //sdCardInitialize();

    digitalWrite(CS,HIGH);

    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // no point in carrying on, so do nothing forevermore:
        for(;;)
            ;
    }
    // give the Ethernet shield a second to initialize:
    delay(1000);
    Serial.println("connecting...");

    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
        Serial.println("connected");
        // Make a HTTP request:
        client.println("GET /search?q=arduino HTTP/1.0");
        client.println();
    }
    else {
        // kf you didn't get a connection to the server:
        Serial.println("connection failed");
    }


    // Initialize SD card with custom pins
    digitalWrite(SS,HIGH);
    if (!SD.begin(CS))
    {
        Serial.println("Failed to initialize SD card!");
    }
    else
    {
        Serial.println("SD card initialized.");
    }

}


/**
 * Run Program
 **/
void loop()
{
    // if there are incoming bytes available
    // from the server, read them and print them:
    if (client.available()) {
        char c = client.read();
        Serial.print(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();

        // do nothing forevermore:
        for(;;)
            ;
    }
}


/**
 * implement Function
 **/
void audioLoop()
{
    if (mp3->isRunning())
    {
        if (!mp3->loop())
        {
            mp3->stop();
            Serial.println("Playback finished ->.");
            Serial.println(mp3->loop());
        }
    }
    else
    {
        Serial.println("MP3 generator is not running.");
        delay(1000);
    }
}

void audioInitialize()
{
    pinMode(PA_CAL, OUTPUT);
    digitalWrite(PA_CAL, HIGH);


    // Open MP3 file
    file = new AudioFileSourceSD("/iceonbaby.mp3");
    if (!file)
    {
        Serial.println("Failed to open MP3 file!");
    }


    // Set up audio output (I2S) with custom pins
    out = new AudioOutputI2S();
    out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    out->SetGain(0.5); // Volume control (0.0 - 1.0)

    // Create MP3 generator
    mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);

    Serial.println("Playback started.");
}

void sdCardInitialize()
{
    // Initialize SD card with custom pins
    if (!SD.begin(CS))
    {
        Serial.println("Failed to initialize SD card!");
    }
    else
    {
        Serial.println("SD card initialized.");
    }
}
