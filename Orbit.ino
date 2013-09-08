#include <Arduino.h>
#include <Orbit.h>

void configure()
{
    pinMode(3,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(13,OUTPUT);
}

void runXtimes()
{
    int vel = orbit_popinteger();
    int times = orbit_popinteger();
    digitalWrite(3,HIGH);
    for(int x = 0; x < times;++x)
    {
        analogWrite(5,LOW);
        analogWrite(6,vel);
        digitalWrite(13,HIGH);
        delay(2000);
        analogWrite(6,LOW);
        analogWrite(5,vel);
        digitalWrite(13,LOW);
        delay(2000);
    }
    analogWrite(5,LOW);
    analogWrite(6,LOW);
}
void setup()
{
    Serial.begin(9600);
}

void loop()
{

    char script[256];
    int index=0;
    memset(&script,'\0',256);
    while(Serial.available())
    {
        script[index++] = Serial.read();
        delay(10);
    }
    if(index>0)
    {
        Serial.write(script);
        orbit(script);
        orbit_registerfunction("runXtimes",runXtimes);
        orbit_registerfunction("configure",configure);
        orbit_run();
        index=0;
    }
}
