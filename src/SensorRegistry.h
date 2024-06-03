
#include <map>
#include <functional>
#include <memory>
#include <utility>

#include "sensor.h"

template<class KeyType, class RegisterType, class CtorFunction>
class Registry {

public:
    Registry() = default;

    bool hasKey(const KeyType key) const;
    std::tuple<bool, std::unique_ptr<RegisterType>> createItem(const KeyType key);
    bool registerItem(const KeyType key, CtorFunction ctor);
    int size() const {return registry.size();}

private:
    std::map<KeyType, CtorFunction> registry;

};

using SensorId = uint8_t;
using SensorCtor = std::function<std::unique_ptr<Sensor>()>;
using SensorRegistry  = Registry<SensorId, Sensor, SensorCtor>;

using SleepBehaviorId = uint8_t;
using SleepBehaviorCtor = std::function<std::unique_ptr<SleepBehavior>()>;
using SleepBehaviorRegistry  = Registry<SleepBehaviorId, SleepBehavior, SleepBehaviorCtor>;

template<class KeyType, class RegisterType, class CtorFunction>
bool Registry<KeyType, RegisterType, CtorFunction>::hasKey(const KeyType  key) const
{
    return this->registry.find(key) != this->registry.end();
}

template<class KeyType, class RegisterType, class CtorFunction>
std::tuple<bool, std::unique_ptr<RegisterType>>
Registry<KeyType, RegisterType, CtorFunction>::createItem(const KeyType key)
{
    if ( auto e = this->registry.find(key); e != this->registry.end()){
        return {true, std::move(e->second())};
    }

    return  { false, std::move(std::unique_ptr<RegisterType>(nullptr))};
}

template<class KeyType, class RegisterType, class CtorFunction>
bool Registry<KeyType, RegisterType, CtorFunction>::registerItem(
    const KeyType key, CtorFunction ctor)
{
    this->registry[key] = ctor;
    return true;
}

// class SensorRegisterHelper {

// public:
//     SensorRegisterHelper() {};

//     static bool registerSensor(SensorId sensorId, SensorCtor ctor);

// };

// extern std::unique_ptr<SensorRegistry> registry;