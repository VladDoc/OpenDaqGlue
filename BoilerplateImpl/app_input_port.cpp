#include "app_input_port.h"
#include <csignal>
#include <iomanip>
#include <iostream>
#include <opendaq/search_filter_factory.h>

#include "app_signal.h"
#include "app_property_object.h"

BEGIN_NAMESPACE_OPENDAQ

bool AppInputPort::processCommand(OpenDaqObject& output, const std::vector<std::string>& command, const InstancePtr& instance)
{
    InputPortPtr port = output.object.asPtr<IInputPort>();
    if (command.empty())
        return false;

    if (command[0] == "connect")
        return connect(port, command[1], instance);
    if (command[0] == "disconnect")
        return disconnect(port);
    if (command[0] == "print")
        return print(port, command[1]);
    if (command[0] == "help")
        return (help(), true);
    if (command[0] == "select") {
        auto ptr = select(port, command[1], 0);
        if(ptr) output = *ptr;
        return !!ptr;
    }
    return false;
}

int AppInputPort::connect(const InputPortPtr& port, const string_view signalId, const InstancePtr& instance)
{
    for (auto signal : instance.getSignalsRecursive())
    {
        if (signal.getGlobalId().toStdString() == signalId)
        {
            port.connect(signal);
            return EC_OK;
        }
    }

    std::cout << "Invalid signal ID." << std::endl;
    return EC_SIGNAL_ID_INVALID;
}

int AppInputPort::disconnect(const InputPortPtr& port)
{
    if (port.getSignal().assigned()) {
        port.disconnect();
        return EC_OK;
    }

    std::cout << "No signal connected." << std::endl;
    return EC_NOT_AVAILABLE;
}

int AppInputPort::print (const InputPortPtr& port, const string_view item)
{
    if (item == "name")
    {
        const auto name = port.getName();
        if (name.assigned() && name.getLength() > 0)
            std::cout << "Name: " + name << std::endl;
        else
            std::cout << "Not available." << std::endl;

        return EC_OK;
    }

    if (item == "requires-signal")
    {
        if (port.getRequiresSignal())
            std::cout << "Requires signal: True" << std::endl;
        else
            std::cout << "Requires signal: False" << std::endl;

        return EC_OK;
    }

    if (item == "signal-id")
    {
        const auto signal = port.getSignal();
        if (!signal.assigned())
        {
            std::cout << "<disconnected>" << std::endl;
            return EC_OK;
        }

        const auto id = signal.getGlobalId();
        std::cout << "Signal ID: " + id << std::endl;

        return EC_OK;
    }

    return EC_PROPERTY_DOESNT_EXIST;
}

OpenDaqObjectPtr AppInputPort::select(const InputPortPtr& port, const string_view item, uint64_t index)
{
    const auto portPtr = port.asPtr<IInputPort>();
    if (item == "signal")
    {
        if (portPtr.getSignal().assigned()) {
            return Make_OpenDaqObjectPtr<AppSignal>(portPtr.getSignal());
        }
        else
            std::cout << "No signal connected to input port." << std::endl;
        return nullptr;
    }

    return nullptr;
}

void AppInputPort::help()
{
    std::cout << std::setw(25) << std::left << "connect <signal-id>"
              << "Connects the signal with <signal-id> to the input port" << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "disconnect"
              << "Disconnects the connected signal, if a signal is connected to the input port." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "select signal"
              << "Select the connected signal if available." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "print <info>"
              << "Prints the value of the given <info> parameter. Available information:" << std::endl
              << std::setw(25) << ""
              << "[name, signal-id, requires-signal]" << std::endl
              << std::endl;
}

int AppInputPort::connect(const string_view signalId, const InstancePtr& instance)
{
    return connect(this->object, signalId, instance);
}
int AppInputPort::disconnect()
{
    return disconnect(this->object);
}

int AppInputPort::list(const InputPortPtr& ip, const string_view item)
{
    return AppPropertyObject::list(ip, item);
}

int AppInputPort::set(const InputPortPtr& ip, const string_view item, const string_view value)
{
    return AppPropertyObject::set(ip, item, value);
}

int AppInputPort::getCount(const InputPortPtr& ip, const string_view item)
{
    return AppPropertyObject::getCount(ip, item);
}


END_NAMESPACE_OPENDAQ
