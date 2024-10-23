#include "app_channel.h"
#include "app_function_block.h"
#include <iostream>
#include <iomanip>

BEGIN_NAMESPACE_OPENDAQ

bool AppChannel::processCommand(OpenDaqObject& channel, const std::vector<std::string>& command)
{
    auto channelPtr = channel.object.asPtr<IChannel>();
    if (command.empty())
        return false;

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

END_NAMESPACE_OPENDAQ
