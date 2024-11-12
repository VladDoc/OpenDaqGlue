#include "app_descriptor.h"

#include <iostream>
#include <iomanip>

#include "app_property_object.h"
#include "app_signal.h"
#include "stdout_redirect.h"

BEGIN_NAMESPACE_OPENDAQ

bool AppDescriptor::processCommand(OpenDaqObject& desc, const std::vector<std::string>& command)
{
    auto descriptorPtr = desc.object.asPtr<IDataDescriptor>();
    if (command.empty())
        return false;

    if (command[0] == "list")
        return list(descriptorPtr, command[1]);
    if (command[0] == "set")
        return set(descriptorPtr, command[1], command[2]);
    if (command[0] == "print")
        return print(descriptorPtr, command[1]);
    if (command[0] == "help")
        return (help(), true);

    return AppPropertyObject::processCommand(desc, command);
}

int AppDescriptor::print(const DataDescriptorPtr& desc, const string_view item)
{
    return (AppSignal::printDataDescriptor(desc, 4, 0), EC_OK);
}

void AppDescriptor::help()
{
    // TODO: do descriptor help
    AppPropertyObject::help();
}

int AppDescriptor::list(const DataDescriptorPtr& desc, const string_view item)
{
    if(item == "properties" || item == "all" || item == "json")
        return (AppSignal::printDataDescriptor(desc, 4, 0), EC_OK);
    else
        return EC_PROPERTY_DOESNT_EXIST;
}

OpenDaqObjectPtr AppDescriptor::select(const DataDescriptorPtr& desc, const string_view item, uint64_t index)
{
    std::cout << "Descriptor does not support sub items" << std::endl;
    return nullptr;
}

int AppDescriptor::set(const DataDescriptorPtr& desc, const string_view item, const string_view value)
{
    // TODO: do set
    return EC_OK;
}

int AppDescriptor::getCount(const DataDescriptorPtr& desc, const string_view item)
{
    std::cout << "Descriptor does not support sub items" << std::endl;
    return EC_METHOD_NOT_IMPLEMENTED;
}

::String AppDescriptor::SaveConfiguration()
{
    bool piped = IsPipedToString();
    ::String oldbuf = GetBufferString();
    EraseBuffer();
    PipeToString();

    AppSignal::printDataDescriptor(this->object, 4, 1);

    auto str = GetBufferString();
    if(!piped) DefaultStdOut();
    SetStringBuffer(oldbuf);
    return
    "{\n" +
        str +
    "\n}";
}

END_NAMESPACE_OPENDAQ
