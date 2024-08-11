//
// Created by Matthi on 03.06.24.
//

#include "mqtt.h"

#include <utility>

Mqtt::Mqtt(
        std::string host,
        std::string username,
        std::string password,
        std::string clientId,
        std::string baseTopic,
        WiFiClient& net)
    : host {std::move( host )}
    , username {std::move(username)}
    , password {std::move(password)}
    , clientId {std::move( clientId )}
    , baseTopic {std::move( baseTopic )}
    , client()
{
    client.begin(this->host.c_str(), net);
    client.onMessage([this](String &topic, String &payload){
        this->messageReceived(topic, payload);
    });
}

void Mqtt::publish(const std::string &topic, const std::string &message)
{
    client.publish((baseTopic + "/" + topic).c_str(), message.c_str());
}

void Mqtt::publish(const std::string &topic, double message)
{
    publish(topic, std::to_string(message));
}

void Mqtt::publish(const std::string &topic, int message)
{
    publish(topic, std::to_string(message));
}

void Mqtt::messageReceived(String &topic, String &payload)
{
    // Not needed for now
}

bool Mqtt::reconnect() {
    int reconnectAttempts = 0;

    std::string logMessage{"Reconnect as " + username + " : " + password};
    Serial.println(logMessage.c_str());

    while (!client.connect(clientId.c_str(), username.c_str(), password.c_str()))
    {
        delay(500);
        if (reconnectAttempts++ > 10)
        {
            return false;
        }
    }
    return true;
}

bool Mqtt::loop()
{
    if (!client.connected())
    {
        bool result = reconnect();
        if (!result)
        {
            return false;
        }
    }
    return client.loop();
}

