#include "app_function_block.h"
#include "app_property_object.h"

#include "app_signal.h"
#include "app_input_port.h"
#include <iomanip>

BEGIN_NAMESPACE_OPENDAQ

bool AppFunctionBlock::processCommand(OpenDaqObject& fb, const std::vector<std::string>& command)
{
    auto fbPtr = fb.object.asPtr<IFunctionBlock>();
    if (command.empty())
        return false;

    if (command[0] == "list")
        return list(fbPtr, command[1]);
    if (command[0] == "select") {
        auto index = command.size() == 2 ? 0 : stoi(command[2]);
        auto ptr = select(fbPtr, command[1], index);

        if(ptr) fb = *ptr;
        return !!ptr;
    }
    if (command[0] == "print")
        return print(fbPtr, command[1]);
    if (command[0] == "help")
        return (help(), true);

    return AppPropertyObject::processCommand(fb, command);
}

int AppFunctionBlock::list(const FunctionBlockPtr& fb, const string_view item)
{
    if (item == "signals")
    {
        int cnt = 0;
        for (auto signal : fb.getSignals())
        {
            auto descriptor = signal.getDescriptor();
            if (!descriptor.assigned())
                continue;

            std::string name = signal.getDescriptor().getName().assigned() ? signal.getDescriptor().getName() : "";
            std::string id = signal.getGlobalId();
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Unique ID: " << id << std::endl;
            cnt++;
        }
        return EC_OK;
    }

    if (item == "input-ports")
    {
        int cnt = 0;
        for (auto port : fb.getInputPorts())
        {
            std::string name = port.getName().assigned() ? port.getName() : "";
            std::string signalId = port.getSignal().assigned() ? port.getSignal().getGlobalId() : "<disconnected>";
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Signal ID: " << signalId << std::endl;
            cnt++;
        }
        return EC_OK;
    }

    return AppPropertyObject::list(fb, item);
}

OpenDaqObjectPtr AppFunctionBlock::select(const FunctionBlockPtr& fb, const string_view item, uint64_t index)
{
    const auto fbPtr = fb.asPtr<IFunctionBlock>();

    if (item == "status-signal")
    {
        if (fbPtr.getStatusSignal().assigned()) {
            return Make_OpenDaqObjectPtr<AppSignal>(fbPtr.getStatusSignal());
        }
        else
            std::cout << "Status signal not available." << std::endl;
    }

    if (item == "signal")
    {
        if (fbPtr.getSignals().getCount() > index) {
            return Make_OpenDaqObjectPtr<AppSignal>(fbPtr.getSignals()[index]);
        }
        else
            std::cout << "Index out of bounds." << std::endl;
    }

    if (item == "input-port")
    {
        if (fbPtr.getInputPorts().getCount() > index) {
            return Make_OpenDaqObjectPtr<AppInputPort>(fbPtr.getSignals()[index]);
        }
        else
            std::cout << "Index out of bounds." << std::endl;
    }

    return nullptr;
}

int AppFunctionBlock::print(const FunctionBlockPtr& fb, const string_view item)
{
    if (item == "name")
    {
        const auto name = fb.getFunctionBlockType().getName();
        if (name.assigned() && name.getLength() > 0)
            std::cout << "Name: " + name << std::endl;
        else
            std::cout << "Not available." << std::endl;

        return EC_OK;
    }

    if (item == "id")
    {
        const auto id = fb.getFunctionBlockType().getId();
        if (id.assigned() && id.getLength() > 0)
            std::cout << "ID: " + id << std::endl;
        else
            std::cout << "Not available." << std::endl;

        return EC_OK;
    }

    if (item == "description")
    {
        const auto desc = fb.getFunctionBlockType().getDescription();
        if (desc.assigned() && desc.getLength() > 0)
            std::cout << "Description: " + desc << std::endl;
        else
            std::cout << "Not available." << std::endl;

        return EC_OK;
    }

    return EC_PROPERTY_DOESNT_EXIST;
}

void AppFunctionBlock::help()
{
    AppPropertyObject::help();
    
    std::cout << std::setw(25) << std::left << "list <type>"
              << "Lists added all child objects of chosen <type>. Available types:" << std::endl
              << std::setw(25) << ""
              << "[signals, input-ports]." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "select <type> <index>"
              << "Select a child of a chosen <type> at a given <index>." << std::endl
              << std::setw(25) << ""
              << "Available types: [signal, input-port, status-signal]." << std::endl
              << std::setw(25) << ""
              << "The index is ignored if the status-signal is chosen for the <type> parameter" << std::endl
              << std::setw(25) << ""
              << "and can be omitted." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "print <info>"
              << "Prints the value of the given <info> parameter. Available information:" << std::endl
              << std::setw(25) << ""
              << "[name, id, description]" << std::endl
              << std::endl;
}

int AppFunctionBlock::getCount(const FunctionBlockPtr& fb, const string_view item)
{
    if (item == "signals")
        return fb.getSignals().getCount();
    if (item == "input-ports")
        return fb.getInputPorts().getCount();

    return AppPropertyObject::getCount(fb, item);
}

END_NAMESPACE_OPENDAQ
