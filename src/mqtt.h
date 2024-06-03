#pragma once

#include <string>
#include <variant>

#include "MQTTClient.h"

#include <WiFiClient.h>


class Mqtt {
public:
    Mqtt(std::string  host, std::string clientId, std::string baseTopic, WiFiClient& net);
    virtual ~Mqtt() = default;

    // No variants :(
    void publish(const std::string& topic, const std::string& message);
    void publish(const std::string& topic, double message);
    void publish(const std::string& topic, int message);

    bool loop();

protected:
    void messageReceived(String &topic, String &payload);
    bool reconnect();

private:
    std::string host;
    std::string clientId;
    std::string baseTopic;

    MQTTClient client;

};


