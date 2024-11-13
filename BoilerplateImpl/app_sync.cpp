#include "app_sync.h"

#include <iostream>
#include <iomanip>

#include "app_property_object.h"
#include "app_signal.h"
#include "stdout_redirect.h"

BEGIN_NAMESPACE_OPENDAQ

bool AppSync::processCommand(OpenDaqObject& self, const std::vector<std::string>& command)
{
    auto sync = self.object.asPtr<ISyncComponent>();
    if (command.empty())
        return false;

    if (command[0] == "list")
        return list(sync, command[1]);
    if (command[0] == "set")
        return set(sync, command[1], command[2]);
    if (command[0] == "print")
        return print(sync, command[1]);
    if (command[0] == "help")
        return (help(), true);

    return AppPropertyObject::processCommand(self, command);
}

int AppSync::print(const SyncComponentPtr& sync, const string_view item)
{
    return AppPropertyObject::print(sync, item);
}

void AppSync::help()
{
    // TODO: do sync component help
    AppPropertyObject::help();
}

int AppSync::list(const SyncComponentPtr& sync, const string_view item)
{
    return AppPropertyObject::list(sync, item);
}

OpenDaqObjectPtr AppSync::select(const SyncComponentPtr& sync, const string_view item, uint64_t index)
{
    std::cout << "SyncComponent does not support sub items" << std::endl;
    return nullptr;
}

int AppSync::set(const SyncComponentPtr& sync, const string_view item, const string_view value)
{
    return AppPropertyObject::set(sync, item, value);
}

int AppSync::getCount(const SyncComponentPtr& sync, const string_view item)
{
    std::cout << "SyncComponent does not support sub items" << std::endl;
    return EC_METHOD_NOT_IMPLEMENTED;
}

::String AppSync::get(const SyncComponentPtr& sync, const string_view item)
{
    return AppPropertyObject::get(sync, item);
}

END_NAMESPACE_OPENDAQ
