#include "SensorRegistry.h"

bool SensorRegistry::hasSensor(const SensorId sensorId) const
{
    return this->registry.find(sensorId) != this->registry.end();
}

std::tuple<bool, std::unique_ptr<Sensor>> SensorRegistry::createSensor(const SensorId SensorId)
{
    if ( auto e = this->registry.find(SensorId); e != this->registry.end()){
        return { true, e->second() };
    } 

    return  { false, std::unique_ptr<Sensor>(nullptr) };
}

bool SensorRegistry::registerSensor(const SensorId sensorId, SensorCtor ctor)
{

    this->registry[sensorId] = ctor;
    return true;
}


bool SensorRegisterHelper::registerSensor(SensorId sensorId, SensorCtor ctor)
{
    registry->registerSensor(sensorId, ctor);
    return true;
}
