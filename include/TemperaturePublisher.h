#ifndef _INCLUDE_TEMPERATUREPUBLISHER_
#define _INCLUDE_TEMPERATUREPUBLISHER_

#include <string>

class TemperaturePublisher
{
private:
public:
    TemperaturePublisher(const std::string &address, uint16_t port);

};

#endif