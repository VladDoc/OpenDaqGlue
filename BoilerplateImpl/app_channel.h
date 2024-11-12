#pragma once
#include <opendaq/channel_ptr.h>

#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

BEGIN_NAMESPACE_OPENDAQ

class AppChannel : public OpenDaqObjectStaticImpl<OpenDaqObject, AppChannel, ChannelPtr>
{
public:
    static bool processCommand(OpenDaqObject& channel, const std::vector<std::string>& command);

    virtual ::String get(const string_view item) override
    {
        return AppChannel::get(this->object, item);
    }
private:
    static int              print   (const ChannelPtr& channel, const string_view item);
    static void             help    ();

    static int              list    (const ChannelPtr& channel, const string_view item);
    static OpenDaqObjectPtr select  (const ChannelPtr& channel, const string_view item, uint64_t index);
    static int              set     (const ChannelPtr& channel, const string_view item, const string_view value);
    static ::String         get     (const ChannelPtr& channel, const string_view item);

    static int              getCount(const ChannelPtr& channel, const string_view item);

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppChannel, ChannelPtr>;
};

END_NAMESPACE_OPENDAQ
