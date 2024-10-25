#include "app_signal.h"
#include <opendaq/sample_type_traits.h>
#include <iomanip>
#include <iostream>
#include <charconv>
#include <chrono>
#include <thread>

#include "opendaq/opendaq.h"

BEGIN_NAMESPACE_OPENDAQ

bool AppSignal::processCommand(OpenDaqObject& output, const std::vector<std::string>& command)
{
    auto signal = output.object.asPtr<ISignal>();
    if (command.empty())
        return false;

    if (command[0] == "print") {
    	if (command.size() == 2)
    		return print(signal, command[1]);
    }
    if (command[0] == "help") {
        help();
        return true;
    }
    if (command[0] == "set") {
    	if (command.size() == 3)
        	return set(signal, command[1], command[2]);
	}
    if (command[0] == "select") {
        auto index = command.size() == 2 ? 0 : stoi(command[2]);
        auto ptr = select(signal, command[1], index);

        if(ptr) output = *ptr;
        return !!ptr;
    }
    if (command[0] == "list") {
    	if (command.size() == 2)
    		return list(signal, command[1]);
	}
    return false;
}

int AppSignal::print(const SignalPtr& signal, const std::string_view item)
{
    if (item == "name")
    {
        if (!signal.getDescriptor().assigned())
            return EC_NOT_AVAILABLE;

        const auto name = signal.getDescriptor().getName();
        if (name.assigned() && name.getLength() > 0) {
            std::cout << "Name: " + name << std::endl;
            return EC_OK;
        }

        std::cout << "Not available." << std::endl;
        return EC_NOT_AVAILABLE;
    }

    if (item == "id")
    {
        const auto id = signal.getGlobalId();
        std::cout << "ID: " + id << std::endl;

        return EC_OK;
    }

    if (item == "descriptor")
    {
        if (!signal.getDescriptor().assigned())
            return EC_NOT_AVAILABLE;

        printDataDescriptor(signal.getDescriptor(), 4, 0);
        std::cout << std::endl;
        return EC_OK;
    }

    if (item == "public")
    {
        if (signal.getPublic())
            std::cout << "Public: true" << std::endl;
        else
            std::cout << "Public: false" << std::endl;
        return EC_OK;
    }

    if (item == "active")
    {
        if (signal.getActive())
            std::cout << "Active: true" << std::endl;
        else
            std::cout << "Active: false" << std::endl;
        return true;
    }

    if (item == "domain-signal")
    {
        if (!signal.getDomainSignal().assigned())
        {
            std::cout << "Domain signal not available." << std::endl;
            return EC_NOT_AVAILABLE;
        }
        const auto sig = signal.getDomainSignal();
        std::string name = sig.getDescriptor().getName().assigned() ? sig.getDescriptor().getName() : "";
        std::string id = sig.getGlobalId();
        std::cout << "Name : " << name << ", Unique ID : " << id << std::endl;
        return EC_OK;
    }
    return EC_PROPERTY_DOESNT_EXIST;
}

int AppSignal::list(const SignalPtr& signal, const std::string_view item)
{
    if (item == "related")
    {
        int cnt = 0;
        for (auto sig : signal.getRelatedSignals())
        {
            std::string name = sig.getDescriptor().getName().assigned() ? sig.getDescriptor().getName() : "";
            std::string id = sig.getGlobalId();
            std::cout << "[" << std::to_string(cnt) << "] Name: " << name << ", Unique ID: " << id << std::endl;
            cnt++;
        }
        return EC_OK;
    }

    return EC_PROPERTY_DOESNT_EXIST;
}

int AppSignal::set(const SignalPtr& signal, const std::string_view item, const std::string_view value)
{
    bool boolean;
    if (value == "true")
        boolean = true;
    else if (value == "false")
        boolean = false;
    else
        return false;

    if (item == "public")
    {
        signal.setPublic(boolean);
        return EC_OK;
    }

    if (item == "active")
    {
        signal.setActive(boolean);
        return EC_OK;
    }

    return EC_PROPERTY_DOESNT_EXIST;
}

OpenDaqObjectPtr AppSignal::select(const SignalPtr& signal, const std::string_view item, uint64_t index)
{
    if (item == "domain-signal")
    {
        if (signal.getDomainSignal().assigned())
            return Make_OpenDaqObjectPtr<AppSignal>(signal.getDomainSignal());
        else
            std::cout << "No domain signal." << std::endl;
    }

    if (item == "related")
    {
        if (signal.getRelatedSignals().getCount() > index)
            return Make_OpenDaqObjectPtr<AppSignal>(signal.getRelatedSignals()[index]);
        else
            std::cout << "Index out of bounds." << std::endl;
    }

    return nullptr;
}

void AppSignal::help()
{
    std::cout << std::setw(25) << std::left << "print <info>"
              << "Prints the value of the given <info> parameter. Available information:" << std::endl
              << std::setw(25) << ""
              << "[name, id, description, tags, descriptor, active, public, domain-signal]" << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "set <property> <state>"
              << "Sets the value of <property> to <state>. Valid values for <property> are" << std::endl
              << std::setw(25) << ""
              << "[active, public]. Valid values for <state> are [true, false]" << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "select domain-signal"
              << "Selects the domain signal if available." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "select related <index>"
              << "Selects the related signal at the given <index>." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "list related"
              << "Lists signals related to the currently selected signal." << std::endl
              << std::endl;
}

void AppSignal::printDataDescriptor(const DataDescriptorPtr& descriptor, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << ""
              << "Data descriptor : {" << std::endl;

    const std::string name = descriptor.getName().assigned() ? descriptor.getName() : "";
    if (!name.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Name : \"" << name << "\"," << std::endl;
    }


    if (descriptor.getDimensions().assigned())
    {
        printDimensions(descriptor.getDimensions(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "Sample type : " << convertSampleTypeToString(descriptor.getSampleType()) << "," << std::endl;

    if (descriptor.getUnit().assigned())
    {
        printUnit(descriptor.getUnit(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    if (descriptor.getValueRange().assigned())
    {
        const auto range = descriptor.getValueRange();
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Value range : [" << std::to_string(range.getLowValue().getFloatValue()) << ", "
                  << std::to_string(range.getHighValue().getFloatValue()) << "]," << std::endl;
    }

    if (descriptor.getRule().assigned())
    {
        printDataRule(descriptor.getRule(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }


    const std::string absRef = descriptor.getOrigin().assigned() ? descriptor.getOrigin() : "";
    if (!absRef.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Absolute reference : \"" << absRef << "\"," << std::endl;
    }

    if (descriptor.getTickResolution().assigned())
    {
        const auto resolution = descriptor.getTickResolution();
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Resolution : " << std::to_string(resolution.getNumerator()) << " / "
                  << std::to_string(resolution.getDenominator()) << "," << std::endl;
    }

    if (descriptor.getPostScaling().assigned())
    {
        printScaling(descriptor.getPostScaling(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    const auto list = descriptor.getStructFields();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        printDataDescriptor(*it, indent, indentLevel + 1);

        if (std::next(it) != list.begin())
            std::cout << ",";
        std::cout << std::endl;
    }


    if (descriptor.getMetadata().assigned())
    {
        printMetadata(descriptor.getMetadata(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printDimensions(const ListPtr<IDimension>& dimensions, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << "" << "Dimensions : [" << std::endl;

    for (auto it = dimensions.begin(); it != dimensions.end(); ++it)
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << "" << "{";

        std::string name = (*it).getName().assigned() ? (*it).getName() : "";
        if (!name.empty())
        {
            std::cout << std::setw(indent * (indentLevel + 2)) << ""
                      << "Name : \"" << name << "\",";
        }

        if ((*it).getUnit().assigned())
        {
            printUnit((*it).getUnit(), indent, indentLevel + 2);
            std::cout << "," << std::endl;
        }

        if ((*it).getRule().assigned())
        {
            printDimensionRule((*it).getRule(), indent, indentLevel + 2);
            std::cout << std::endl;
        }

        std::cout << std::setw(indent * (indentLevel + 1)) << "" << "}";
        if (std::next(it) != dimensions.begin())
            std::cout << ",";
        std::cout << std::endl;
    }

    std::cout << std::setw(indent * indentLevel) << "" << "]";
}

void AppSignal::printDataRule(const DataRulePtr& rule, std::streamsize indent, int indentLevel)
{
    auto params = rule.getParameters();
    if (!params.assigned())
        return;

    std::cout << std::setw(indent * indentLevel) << ""
              << "Dimension rule : {" << std::endl;

    switch (rule.getType())
    {
        case DataRuleType::Other:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Other," << std::endl;
            break;
        case DataRuleType::Linear:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Linear," << std::endl;
            break;
        case DataRuleType::Constant:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Constant," << std::endl;
            break;
        case DataRuleType::Explicit:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Explicit," << std::endl;
            break;
    }
    const auto list = params.getKeys();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        std::cout << std::setw((indentLevel + 1) * indent) << "" << *it << " : " << params.get(*it).toString();
        if (std::next(it) != list.begin())
            std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printDimensionRule(const DimensionRulePtr& rule, std::streamsize indent, int indentLevel)
{
    const auto params = rule.getParameters();
    if (!params.assigned())
        return;

    std::cout << std::setw(indent * indentLevel) << "" << "Dimension rule : {" << std::endl;

    switch (rule.getType())
    {
        case DimensionRuleType::Other:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Other," << std::endl;
            break;
        case DimensionRuleType::Linear:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Linear," << std::endl;
            break;
        case DimensionRuleType::Logarithmic:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Logarithmic," << std::endl;
            break;
        case DimensionRuleType::List:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : List," << std::endl;
            break;
    }
    const auto list = params.getKeys();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        std::cout << std::setw((indentLevel + 1) * indent) << "" << *it << " : " << params.get(*it).toString();
        if (std::next(it) != list.begin())
            std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printUnit(const UnitPtr& unit, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << ""
              << "Unit : {" << std::endl;

    const std::string id = std::to_string(unit.getId());
    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "ID : \"" << id << "\"," << std::endl;

    const std::string symbol = unit.getSymbol().assigned() ? unit.getSymbol() : "";
    if (!symbol.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Symbol : \"" << symbol << "\"," << std::endl;
    }

    const std::string name = unit.getName().assigned() ? unit.getName() : "";
    if (!name.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Name : \"" << name << "\"," << std::endl;
    }

    const std::string quantity = unit.getQuantity().assigned() ? unit.getQuantity() : "";
    if (!quantity.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "Quantity : \"" << quantity << "\"" << std::endl;
    }

    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printScaling(const ScalingPtr& scaling, std::streamsize indent, int indentLevel)
{
    const auto params = scaling.getParameters();
    if (!params.assigned())
        return;

    std::cout << std::setw(indent * indentLevel) << ""
              << "Post scaling : {" << std::endl;

    switch (scaling.getType())
    {
        case ScalingType::Other:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Other," << std::endl;
            break;
        case ScalingType::Linear:
            std::cout << std::setw((indentLevel + 1) * indent) << "" << "Type : Linear," << std::endl;
            break;
    }

    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "Input data type : " << convertSampleTypeToString(scaling.getInputSampleType()) << "," << std::endl;

    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "Output data type : " << convertScaledSampleTypeToString(scaling.getOutputSampleType()) << "," << std::endl;

    const auto list = params.getKeys();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        std::cout << std::setw((indentLevel + 1) * indent) << "" << *it << " : " << params.get(*it).toString();
        if (std::next(it) != list.begin())
            std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printTags(const TagsPtr& tags, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << "" << "Tags : [";
    const auto list = tags.getList();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        std::cout << *it;
        if (std::next(it) != list.begin())
            std::cout << ",";
    }
    std::cout << "]";
}

void AppSignal::printMetadata(const DictPtr<IString, IString>& metadata, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << "" << "Metadata : {" << std::endl;

    const auto list = metadata.getKeys();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        std::cout << std::setw((indentLevel + 1) * indent) << *it << " : " << metadata.get(*it);
        if (std::next(it) != list.begin())
            std::cout << ",";
        std::cout << std::endl;
    }

    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

DataPacketPtr createPacketForSignal(const SignalPtr& signal, SizeT numSamples, Int offset)
{
    // Create a data packet where the values are generated via the +1 rule starting at 0
    auto domainPacket = daq::DataPacket(
        signal.getDomainSignal().getDescriptor(),
        numSamples,
        offset  // offset from 0 to start the sample generation at
    );

    return daq::DataPacketWithDomain(
        domainPacket,
        signal.getDescriptor(),
        numSamples
    );
}

void AppSignal::SendDataPacket(double* data, size_t count)
{
    auto signal = this->object.asPtr<ISignalConfig>();

    auto packet = createPacketForSignal(signal, count, 0);
    auto packet_data = static_cast<double*>(packet.getData());

    memcpy(packet_data, data, count);
    signal.sendPacket(packet);
}

// Sends one full turn of a sine wave
void AppSignal::SendTestDataPacket(size_t count, double sine_range)
{
    auto signal = this->object.asPtr<ISignalConfig>();

    auto packet = createPacketForSignal(signal, count, 0);
    auto data = static_cast<double*>(packet.getData());

    auto pi_fraction = (2 * M_PI) / count;

    for(auto i = 0u; i < count; ++i)
        data[i] = sin(pi_fraction * i) * sine_range;

    signal.sendPacket(packet);
}

int AppSignal::Read(uint64_t NumOfSamples, int timeout)
{
    auto signal = this->object.asPtr<ISignal>();

    //SendTestData(signal, NumOfSamples);

    if(reader == nullptr)
        reader = StreamReader(signal, ReadTimeoutType::All);

    auto timeReader = TimeReader(reader);

    samples.Resize(NumOfSamples);

    auto status = ReaderStatus();

    timeReader.readWithDomain(
        samples.readings.data(),
        samples.timestamps.data(),
        &samples.readCount,
        timeout,
        &status
    );

    daq::ReadStatus whu_happen = status.getReadStatus();

    //std::cout << (int)whu_happen << std::endl;

    return samples.readCount;
}


END_NAMESPACE_OPENDAQ
