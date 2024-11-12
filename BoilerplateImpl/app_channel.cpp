#include "app_channel.h"
#include "app_function_block.h"
#include <iostream>
#include <iomanip>

#include "app_property_object.h"

BEGIN_NAMESPACE_OPENDAQ

bool AppChannel::processCommand(OpenDaqObject& channel, const std::vector<std::string>& command)
{
    auto channelPtr = channel.object.asPtr<IChannel>();
    if (command.empty())
        return false;

    if (command[0] == "list")
        return list(channelPtr, command[1]);
    if (command[0] == "set")
        return set(channelPtr, command[1], command[2]);
    if (command[0] == "print")
        return print(channelPtr, command[1]);
    if (command[0] == "help")
        return (help(), true);

    return AppFunctionBlock::processCommand(channel, command);
}

int AppChannel::print(const ChannelPtr& channel, const string_view item)
{
    if (item == "tags")
    {
        std::cout << "[";
        const auto tags = channel.getTags().getList();
        for (auto it = tags.begin(); it != tags.end(); ++it)
        {
            std::cout << *it;
            if (std::next(it) != tags.begin())
                std::cout << ",";
        }
        std::cout << "]" << std::endl;

        return EC_OK;
    }

    return AppFunctionBlock::print(channel, item);
}

void AppChannel::help()
{
    AppFunctionBlock::help();
    
    std::cout << std::setw(25) << std::left << "print tags"
              << "Prints the channel's tags." << std::endl
              << std::endl;
}

int AppChannel::list(const ChannelPtr& channel, const string_view item)
{
    return AppFunctionBlock::list(channel, item);
}

OpenDaqObjectPtr AppChannel::select(const ChannelPtr& channel, const string_view item, uint64_t index)
{
    return AppFunctionBlock::select(channel, item, index);
}

int AppChannel::set(const ChannelPtr& channel, const string_view item, const string_view value)
{
    return AppPropertyObject::set(channel, item, value);
}

int AppChannel::getCount(const ChannelPtr& channel, const string_view item)
{
    return AppFunctionBlock::getCount(channel, item);
}

::String AppChannel::get(const ChannelPtr& channel, const string_view item)
{
    return AppFunctionBlock::get(channel, item);
}

END_NAMESPACE_OPENDAQ
