
#include <map>
#include <functional>
#include <memory>

#include "sensor.h"


using SensorId = uint8_t;
using SensorCtor = std::function<std::unique_ptr<Sensor>()>;

class SensorRegistry {

public:
    SensorRegistry() = default;

    bool hasSensor(const SensorId sensorId) const;
    std::tuple<bool, std::unique_ptr<Sensor>> createSensor(const SensorId);

    bool registerSensor(const SensorId, SensorCtor);

private:
    std::map<SensorId, SensorCtor> registry;

};


class SensorRegisterHelper {
    
public:
    SensorRegisterHelper() {};

    static bool registerSensor(SensorId sensorId, SensorCtor ctor);

};

extern std::unique_ptr<SensorRegistry> registry;