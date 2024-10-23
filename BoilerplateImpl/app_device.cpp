#include "app_device.h"
#include <iomanip>
#include <iostream>

#include "app_function_block.h"
#include "app_property_object.h"
#include "app_channel.h"
#include "app_signal.h"

BEGIN_NAMESPACE_OPENDAQ

bool AppDevice::processCommand(OpenDaqObject& deviceobj, const std::vector<std::string>& command)
{
    auto device = deviceobj.object.asPtr<IDevice>();
    if (command.empty())
        return false;

    if (command[0] == "list" && command.size() == 2)
        return list(device, command[1]);
    if (command[0] == "add" && command.size() == 3)  {
        auto ptr = add(device, command[1], command[2]);

        if(ptr) deviceobj = *ptr;
        return !!ptr;
    }
    if (command[0] == "remove" && command.size() == 3)
        return remove(device, command[1], stoi(command[1]));
    if (command[0] == "select" && command.size() == 3) {
        auto ptr = select(device, command[1], stoi(command[2]));

        if(ptr) deviceobj = *ptr;
        return !!ptr;
    }
    if (command[0] == "print" && command.size() == 2)
        return print(device, command[1]);
    if (command[0] == "help")
        return (help(), true);

    return AppPropertyObject::processCommand(deviceobj, command);
}

int AppDevice::list(const DevicePtr& device, const string_view item)
{
    if (item == "devices")
    {
        int cnt = 0;
        for (const auto& subDevice : device.getDevices())
        {
            std::string name = subDevice.getInfo().getName().assigned() ? subDevice.getInfo().getName() : "";
            std::string serialNumber = subDevice.getInfo().getSerialNumber().assigned() ? subDevice.getInfo().getSerialNumber() : "";
            std::string connectionString = subDevice.getInfo().getConnectionString().assigned() ? subDevice.getInfo().getConnectionString() : "";
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Serial number: " << serialNumber
                      << ", Connection string: " << connectionString
                      << std::endl;
            cnt++;
        }
        return EC_OK;
    }

    if (item == "channels")
    {
        int cnt = 0;
        for (const auto& channel : device.getChannels())
        {
            std::string name = channel.getFunctionBlockType().getName().assigned() ? channel.getFunctionBlockType().getName() : "";
            std::string id = channel.getFunctionBlockType().getId().assigned() ? channel.getFunctionBlockType().getId() : "";
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Unique ID: " << id << std::endl;
            cnt++;
        }
        return EC_OK;
    }
    
    if (item == "function-blocks")
    {
        int cnt = 0;
        for (const auto& fb : device.getFunctionBlocks())
        {
            std::string name = fb.getFunctionBlockType().getName().assigned() ? fb.getFunctionBlockType().getName() : "";
            std::string id = fb.getFunctionBlockType().getId().assigned() ? fb.getFunctionBlockType().getId() : "";
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Unique ID: " << id << std::endl;
            cnt++;
        }
        return EC_OK;
    }
    
    if (item == "signals")
    {
        int cnt = 0;
        for (const auto& signal : device.getSignals())
        {
            std::string name = signal.getDescriptor().getName().assigned() ? signal.getDescriptor().getName() : "";
            std::string id = signal.getGlobalId();
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Unique ID: " << id << std::endl;
            cnt++;
        }
        return EC_OK;
    }
    if (item == "available-devices")
    {
        int cnt = 0;
        for (const auto& discoveredDevice : device.getAvailableDevices())
        {
            std::string name = discoveredDevice.getName().assigned() ? discoveredDevice.getName() : "";
            std::string serialNumber = discoveredDevice.getSerialNumber().assigned() ? discoveredDevice.getSerialNumber() : "";
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Serial number: " << serialNumber
                      << ", Connection string: " << discoveredDevice.getConnectionString()
                      << std::endl;
            cnt++;
        }
        return EC_OK;
    }

    if (item == "available-function-blocks")
    {
        int cnt = 0;
        for (const auto& [_, fb] : device.getAvailableFunctionBlockTypes())
        {
            std::string name = fb.getName().assigned() ? fb.getName() : "";
            std::string id = fb.getId().assigned() ? fb.getId() : "";
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Unique ID: " << id << std::endl;
            cnt++;
        }
        return EC_OK;
    }

    return AppPropertyObject::list(device, item);
}

OpenDaqObjectPtr AppDevice::select(const DevicePtr& device, const string_view item, uint64_t index)
{
    if (item == "device")
    {
        if (device.getDevices().getCount() > index)
            return Make_OpenDaqObjectPtr<AppDevice>(device.getDevices()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
        return nullptr;
    }

    if (item == "function-block")
    {
        if (device.getFunctionBlocks().getCount() > index)
            return Make_OpenDaqObjectPtr<AppFunctionBlock>(device.getFunctionBlocks()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
        return nullptr;
    }

    if (item == "channel")
    {
        if (device.getChannels().getCount() > index)
            return Make_OpenDaqObjectPtr<AppChannel>(device.getChannels()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
        return nullptr;
    }

    if (item == "signal")
    {
        if (device.getSignals().getCount() > index)
            return Make_OpenDaqObjectPtr<AppSignal>(device.getSignals()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
        return nullptr;
    }

    return nullptr;;
}

OpenDaqObjectPtr AppDevice::add(const DevicePtr& device, const string_view type, const string_view what)
{
    auto str = String(what.data(), what.size());
    if (type == "device")
    {
        try
        {
            return Make_OpenDaqObjectPtr<AppDevice>(device.addDevice(str));
        }
        catch (...)
        {
            std::cout << "Unable to add device with the given connection string" << std::endl;
        }
    }

    if (type == "function-block")
    {
        try
        {
            return Make_OpenDaqObjectPtr<AppFunctionBlock>(device.addFunctionBlock(str));
        }
        catch (...)
        {
            std::cout << "Unable to add function block with the given ID" << std::endl;
        }
    }

    return nullptr;
}

int AppDevice::remove(const DevicePtr& device, const string_view item, uint64_t index)
{
    if (item == "device")
    {
        if (device.getDevices().getCount() > index)
            device.removeDevice(device.getDevices()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
        return EC_ARRAY_OUT_OF_BOUNDS;
    }
    
    if (item == "function-block")
    {
        if (device.getFunctionBlocks().getCount() > index)
            device.removeFunctionBlock(device.getFunctionBlocks()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
        return EC_ARRAY_OUT_OF_BOUNDS;
    }

    return EC_PROPERTY_DOESNT_EXIST;
}

int AppDevice::print(const DevicePtr& device, const string_view item)
{
    auto stringPtr = daq::String(item.data(), item.size());
    const auto info = device.getInfo();

    if (item == "all")
    {
        for (const auto prop : info.getAllProperties())
        {
            const auto name = prop.getName();
            const auto value = info.getPropertyValue(name);
            std::cout << name << ": " << value << std::endl;
        }

        return EC_OK;
    }

    if (info.hasProperty(stringPtr))
    {
        std::cout << stringPtr << ": " << info.getPropertyValue(stringPtr) << std::endl;
        return EC_OK;
    }

    std::cout << "Not available." << std::endl;
    return EC_PROPERTY_DOESNT_EXIST;
}

void AppDevice::help()
{
    AppPropertyObject::help();
    
    std::cout << std::setw(25) << std::left << "list <type>"
              << "Lists added all child objects of chosen <type>. Available types:" << std::endl
              << std::setw(25) << ""
              << "[devices, channels, function-blocks, signals]." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "select <type> <index>"
              << "Select a child of a chosen <type> at a given <index>." << std::endl
              << std::setw(25) << ""
              << "Available types: [device, function-block]." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "list available-<type>"
              << "Lists all objects of chosen <type> that can be added via `add`." << std::endl
              << std::setw(25) << ""
              << "Available types: [devices, function-blocks]." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "add <type> <address/id>"
              << "Adds an object of <type> at the given address, or with the given ID. " << std::endl
              << std::setw(25) << ""
              << "Available types: [device, function-block]. To add function blocks," << std::endl
              << std::setw(25) << ""
              << "enter their ID. To add devices, enter their connection string." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "remove <type> <index>" 
              << "Remove an object of given <type> at the given <index>." << std::endl
              << std::setw(25) << ""
              << "Available types: [device, function-block]. The index can be obtained" << std::endl
              << std::setw(25) << ""
              << "by listing added devices or function blocks via `list`." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "print <info>"
              << "Prints the value of the given <info> parameter. Available information:" << std::endl
              << std::setw(25) << ""
              << "[name, location, model, serial-number, locking-context, platform," << std::endl
              << std::setw(25) << ""
              << "firmware-version, connection-string, metadata]." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "getCount <item>"
              << "Gets total count of an <item> objects. Available information:" << std::endl
              << std::setw(25) << ""
              << "[devices, function-blocks, channels, signals," << std::endl
              << std::setw(25) << ""
              << "available-devices, available-function-blocks]." << std::endl
              << std::endl;
}

OpenDaqObjectPtr AppDevice::add(const string_view type, const string_view what)
{
    return add(this->object, type, what);
}

int AppDevice::remove(const string_view type, uint64_t index)
{
    return remove(this->object, type, index);
}

int AppDevice::loadConfiguration(const char* json)
{
    auto Idevice = this->object.asPtr<IDevice>();
    Idevice->loadConfiguration(String(json));
    return EC_OK;
}

::String AppDevice::saveConfiguration()
{
    auto Idevice = this->object.asPtr<IDevice>();
    auto str = daq::String(nullptr);
    Idevice->saveConfiguration(&str);
    return str.toStdString();
}

int AppDevice::getCount(const DevicePtr& device, const string_view item)
{
    if (item == "devices")
        return device.getDevices().getCount();

    if (item == "function-blocks")
        return device.getFunctionBlocks().getCount();

    if (item == "channels")
        return device.getChannels().getCount();

    if (item == "signals")
        return device.getSignals().getCount();;

    if (item == "available-devices")
        return device.getAvailableDevices().getCount();

    if (item == "available-function-blocks")
        return device.getAvailableFunctionBlockTypes().getCount();

    return AppPropertyObject::getCount(device, item);
}

::String AppDevice::GetAvailableDeviceConnectionString(uint64_t index)
{
    auto Idevice = this->object.asPtr<IDevice>();

    if(Idevice.getAvailableDevices().getCount() > index)
        return Idevice.getAvailableDevices()[index].getConnectionString().toStdString();

    return nullptr;
}

::String AppDevice::GetAvailableFunctionBlockID(uint64_t index)
{
    auto Idevice = this->object.asPtr<IDevice>();

    if(Idevice.getAvailableFunctionBlockTypes().getCount() > index)
        return Idevice.getAvailableFunctionBlockTypes().getKeyList()[index].toStdString();

    return nullptr;
}


END_NAMESPACE_OPENDAQ
