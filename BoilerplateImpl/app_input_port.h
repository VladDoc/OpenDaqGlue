#pragma once
#include <opendaq/input_port_ptr.h>
#include <opendaq/instance_ptr.h>

#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

BEGIN_NAMESPACE_OPENDAQ

class AppInputPort : public OpenDaqObjectStaticImpl<OpenDaqObject, AppInputPort, InputPortPtr>
{
public:
    static bool processCommand(OpenDaqObject& port, const std::vector<std::string>& command, const InstancePtr& instance);

    AppInputPort() = default;
    AppInputPort(const InputPortPtr& port)
    {
        this->object = port;
    }

    virtual int connect(const string_view signalId, const InstancePtr& instance);
    virtual int disconnect();

private:
    static int connect(const InputPortPtr& port, const string_view signalId, const InstancePtr& instance);
    static int disconnect(const InputPortPtr& port);

    static int              print (const InputPortPtr& port, const string_view item);
    static OpenDaqObjectPtr select(const InputPortPtr& port, const string_view item, uint64_t index);
    static void             help();

    static int set(const BaseObjectPtr&, const string_view, const string_view)
    {
        std::cout << "Unsupported Method AppInputPort::set\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }
    static int   list(const BaseObjectPtr&, const string_view)
    {
        return EC_METHOD_NOT_IMPLEMENTED;
    }
    static int   getCount(const BaseObjectPtr&, const string_view)
    {
        std::cout << "Unsupported Method AppInputPort::getCount\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppInputPort, InputPortPtr>;
};

END_NAMESPACE_OPENDAQ
