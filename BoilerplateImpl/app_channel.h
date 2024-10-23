#pragma once
#include <opendaq/channel_ptr.h>

#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

#include "app_function_block.h"

BEGIN_NAMESPACE_OPENDAQ

class AppChannel : public AppFunctionBlock
{
public:
    static bool processCommand(OpenDaqObject& channel, const std::vector<std::string>& command);

    virtual int print(const string_view item) override
    {
        return AppChannel::print(this->object, item);
    }
    virtual void help(int) override
    {
        AppChannel::help();
    }

private:
    static int print(const ChannelPtr& channel, const string_view item);
    static void help();

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppChannel, ChannelPtr>;
};

END_NAMESPACE_OPENDAQ
