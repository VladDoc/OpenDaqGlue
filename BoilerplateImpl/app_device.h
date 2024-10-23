#pragma once
#include <opendaq/device_ptr.h>

#include <vector>
#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

BEGIN_NAMESPACE_OPENDAQ

class AppDevice : public OpenDaqObjectStaticImpl<OpenDaqObject, AppDevice, DevicePtr>
{
public:
    static bool processCommand(OpenDaqObject& device, const std::vector<std::string>& command);

    virtual OpenDaqObjectPtr add(const string_view type, const string_view what);
    virtual int remove(const string_view type, uint64_t index);

    virtual int loadConfiguration(const char* json);
    virtual ::String saveConfiguration();

    virtual ::String GetAvailableDeviceConnectionString(uint64_t index);
    virtual ::String GetAvailableFunctionBlockID(uint64_t index);
private:
    static int  list(const DevicePtr& device, const string_view item);
    static OpenDaqObjectPtr select(const DevicePtr& device, const string_view item, uint64_t index);
    static OpenDaqObjectPtr add(const DevicePtr& device, const string_view type, const string_view what);
    static int remove(const DevicePtr& device, const string_view type, uint64_t index);
    static int print(const DevicePtr& device, const string_view item);

    static void help();

    static int set(const DevicePtr& device, const string_view, const string_view)
    {
        std::cout << "Unsupported Method AppDevice::set\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }

    static int getCount(const DevicePtr& device, const string_view item);

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppDevice, DevicePtr>;
};

END_NAMESPACE_OPENDAQ
