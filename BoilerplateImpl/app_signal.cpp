#include "app_signal.h"
#include <opendaq/sample_type_traits.h>
#include <iomanip>
#include <iostream>
#include <charconv>
#include <chrono>
#include <thread>

#include "opendaq/opendaq.h"

#include "app_property_object.h"
#include "app_descriptor.h"

#include <nlohmann/json.hpp>


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
    return AppPropertyObject::print(signal, item);
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

    return AppPropertyObject::list(signal, item);
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

    return AppPropertyObject::set(signal, item, value);
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

    if (item == "descriptor")
    {
        if(signal.getDescriptor().assigned())
            return Make_OpenDaqObjectPtr<AppDescriptor>(signal.getDescriptor());
        else
            std::cout << "No descriptor." << std::endl;
    }

    return nullptr;
}

void AppSignal::help()
{
    return AppPropertyObject::help();

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
              << "\"dataDescriptor\": {" << std::endl;

    const std::string name = descriptor.getName().assigned() ? descriptor.getName() : "";
    if (!name.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "\"name\": \"" << name << "\"," << std::endl;
    }


    if (descriptor.getDimensions().assigned())
    {
        printDimensions(descriptor.getDimensions(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "\"sampleType\": " << (int)descriptor.getSampleType() << "," << std::endl;

    if (descriptor.getUnit().assigned())
    {
        printUnit(descriptor.getUnit(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    if (descriptor.getValueRange().assigned())
    {
        const auto range = descriptor.getValueRange();
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "\"valueRange\": [" << std::to_string(range.getLowValue().getFloatValue()) << ", "
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
                  << "\"origin\": \"" << absRef << "\"," << std::endl;
    }

    if (descriptor.getTickResolution().assigned())
    {
        const auto resolution = descriptor.getTickResolution();
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "\"tickResolution\": {\n"
                  << std::setw(indent * (indentLevel + 2)) << ""
                  << "\"num\": " <<  std::to_string(resolution.getNumerator())   << "," << std::endl
                  << std::setw(indent * (indentLevel + 2)) << ""
                  << "\"den\": " <<  std::to_string(resolution.getDenominator()) << " " << std::endl
                  << std::setw(indent * (indentLevel + 1)) << ""
                  << "}," << std::endl;
    }

    if (descriptor.getPostScaling().assigned())
    {
        printScaling(descriptor.getPostScaling(), indent, indentLevel + 1);
        std::cout << "," << std::endl;
    }

    const auto list = descriptor.getStructFields();
    const auto len = list.getCount();
    auto count = 0u;

    for (auto it = list.begin(); it != list.end(); ++it, ++count)
    {
        printDataDescriptor(*it, indent, indentLevel + 1);

        if (count < len - 1)
            std::cout << ",";
        std::cout << std::endl;
    }


    if (descriptor.getMetadata().assigned())
    {
        printMetadata(descriptor.getMetadata(), indent, indentLevel + 1);
    }

    std::cout << std::setw(indent * indentLevel) << "" << "}" << std::endl;
}

void AppSignal::printDimensions(const ListPtr<IDimension>& dimensions, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << "" << "\"dimensions\": [" << std::endl;

    const auto len = dimensions.getCount();
    auto count = 0u;
    for (auto it = dimensions.begin(); it != dimensions.end(); ++it, ++count)
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << "" << "{";

        std::string name = (*it).getName().assigned() ? (*it).getName() : "";
        if (!name.empty())
        {
            std::cout << std::setw(indent * (indentLevel + 2)) << ""
                      << "\"name\": \"" << name << "\",";
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
        if (count < len - 1)
            std::cout << ",";
        std::cout << std::endl;
    }

    std::cout << std::setw(indent * indentLevel) << "" << "]";
}

void printParamKeys(const daq::DictPtr<daq::IString, daq::IBaseObject>& params, std::streamsize indent, int indentLevel)
{
    const auto list = params.getKeys();
    const auto last = list.end();

    size_t length = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        ++length;
    }

    size_t count = 0;
    for (auto it = list.begin(); it != list.end(); ++it, ++count)
    {
        std::cout << std::setw((indentLevel + 1) * indent) << ""
                  << "\"" << *it << "\": " << params.get(*it).toString();

        if (count < length - 1) {
            std::cout << ",";
        }
        std::cout << std::endl;
    }
}

void AppSignal::printDataRule(const DataRulePtr& rule, std::streamsize indent, int indentLevel)
{
    auto params = rule.getParameters();
    if (!params.assigned())
        return;

    auto len = params.getCount();
    const auto delim = len > 0 ? "," : "";

    std::cout << std::setw(indent * indentLevel) << ""
              << "\"dataRule\": {" << std::endl;

    std::cout << std::setw((indentLevel + 1) * indent)
              << "" << "\"type\": " << (int)rule.getType() << delim << std::endl;

    printParamKeys(params, indent, indentLevel);
    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printDimensionRule(const DimensionRulePtr& rule, std::streamsize indent, int indentLevel)
{
    const auto params = rule.getParameters();
    if (!params.assigned())
        return;

    std::cout << std::setw(indent * indentLevel) << "" << "\"dimensionRule\": {" << std::endl;

    auto len = params.getCount();
    const auto delim = len > 0 ? "," : "";

    std::cout << std::setw((indentLevel + 1) * indent)
              << "" << "\"type\": " << (int)rule.getType() << delim << std::endl;

    printParamKeys(params, indent, indentLevel);
    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printUnit(const UnitPtr& unit, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << ""
              << "\"unit\": {" << std::endl;

    const std::string id = std::to_string(unit.getId());
    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "\"id\": " << id << "," << std::endl;

    const std::string symbol = unit.getSymbol().assigned() ? unit.getSymbol() : "";
    if (!symbol.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "\"symbol\": \"" << symbol << "\"," << std::endl;
    }

    const std::string name = unit.getName().assigned() ? unit.getName() : "";
    if (!name.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "\"name\": \"" << name << "\"," << std::endl;
    }

    const std::string quantity = unit.getQuantity().assigned() ? unit.getQuantity() : "";
    if (!quantity.empty())
    {
        std::cout << std::setw(indent * (indentLevel + 1)) << ""
                  << "\"quantity\": \"" << quantity << "\"" << std::endl;
    }

    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printScaling(const ScalingPtr& scaling, std::streamsize indent, int indentLevel)
{
    const auto params = scaling.getParameters();
    if (!params.assigned())
        return;

    std::cout << std::setw(indent * indentLevel) << ""
              << "\"postScaling\": {" << std::endl;

    auto len = params.getCount();
    const auto delim = len > 0 ? "," : "";

    std::cout << std::setw((indentLevel + 1) * indent)
              << "" << "\"type\": " << (int)scaling.getType() << delim << std::endl;

    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "\"inputSampleType\": " << (int)scaling.getInputSampleType() << "," << std::endl;

    std::cout << std::setw(indent * (indentLevel + 1)) << ""
              << "\"outputSampleType\": " << (int)scaling.getOutputSampleType() << "," << std::endl;

    printParamKeys(params, indent, indentLevel);
    std::cout << std::setw(indent * indentLevel) << "" << "}";
}

void AppSignal::printTags(const TagsPtr& tags, std::streamsize indent, int indentLevel)
{
    std::cout << std::setw(indent * indentLevel) << "" << "\"tags\": [";
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
    std::cout << std::setw(indent * indentLevel) << "" << "\"metadata\": {" << std::endl;

    printParamKeys(metadata, indent, indentLevel);

    std::cout << std::setw(indent * indentLevel) << "" << "}" << std::endl;
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

template <typename T>
auto TryGet(const nlohmann::json& json_value, const T& def)
{
    using value_type = std::remove_cv_t<
        std::remove_reference_t<decltype(def)>>;

    return json_value.is_null() ? def : json_value.template get<value_type>();
};

int AppSignal::LoadDataDescriptorFromJson(const string_view json)
{
    if (!nlohmann::json::accept(json))
        return EC_INVALID_JSON;

    try {
        auto signal = this->object.asPtr<ISignalConfig>();
        auto builder = DataDescriptorBuilder();
        const std::string empty_str = "";

        auto Has = [](const auto& json_value, const char* item) {
            return json_value.find(item) != json_value.cend();
        };

        auto GetDict = [&empty_str](const auto& json_value) {
            DictPtr<StringPtr, StringPtr> dict;

            for (const auto& [key, value] : json_value.items()) {
                dict.set(key, TryGet(value, empty_str));
            }
            return dict;
        };

        nlohmann::json root = nlohmann::json::parse(json);
        auto data = root.at("dataDescriptor");

        if(Has(data, "name"))
        {
            auto& name = data["name"];
            builder.setName(TryGet(name, empty_str));
        }

        if(Has(data, "sampleType"))
        {
            auto& sampleType = data["sampleType"];
            builder.setSampleType((daq::SampleType)TryGet<int>(sampleType, 0));
        }
        if(Has(data, "dimensions"))
        {
            // TODO: add dimensions parsing
            //auto& dimensions = data.at("dimensions"];
        }
        if(Has(data, "unit"))
        {
            auto& unit = data["unit"];
            auto unitBuilder = UnitBuilder();

            unitBuilder.setId(TryGet<int>(unit["id"], 0));
            unitBuilder.setSymbol(TryGet(unit["symbol"], empty_str));
            unitBuilder.setName(TryGet(unit["name"], empty_str));
            unitBuilder.setQuantity(TryGet(unit["quantity"], empty_str));

            builder.setUnit(
                unitBuilder.build()
            );
        }
        if(Has(data, "valueRange"))
        {
            auto& valueRange = data["valueRange"];

            builder.setValueRange(Range(
                TryGet<double>(valueRange[0], 0),
                TryGet<double>(valueRange[1], 0)
            ));
        }
        if(Has(data, "dataRule"))
        {
            auto& dataRule = data["dataRule"];
            auto dataRuleBuilder = DataRuleBuilder();
            dataRuleBuilder.setType((DataRuleType)dataRule["type"].get<int>());

            for (const auto& [key, value] : dataRule.items()) {
                if(key == "type")
                    dataRuleBuilder.setType((DataRuleType)TryGet<int>(value, 0));
                else
                    dataRuleBuilder.addParameter(key, TryGet<double>(value, 0));
            }

            builder.setRule(dataRuleBuilder.build());
        }
        if(Has(data, "metadata"))
        {
            auto& metadata = data["metadata"];
            builder.setMetadata(GetDict(metadata));
        }
        if(Has(data, "origin"))
        {
            auto& origin = data["origin"];
            builder.setOrigin(TryGet(origin, empty_str));
        }
        if(Has(data, "tickResolution"))
        {
            auto& tickResolution = data["tickResolution"];
            auto num = TryGet(tickResolution["num"], 0);
            auto den = TryGet(tickResolution["den"], 0);
            builder.setTickResolution(Ratio(num, den));
        }
        if(Has(data, "postScaling"))
        {
            // untested
            auto& postScaling = data["postScaling"];
            auto type = (ScalingType)TryGet(postScaling["type"], 0);
            auto inputSampleType = (SampleType)TryGet(postScaling["inputSampleType"], 0);
            auto outputSampleType = (ScaledSampleType)TryGet(postScaling["outputSampleType"], 0);

            DictPtr<StringPtr, StringPtr> params;

            int i = 0;
            for (const auto& [key, value] : postScaling.items()) {
                //skip first 3
                if(i < 3) goto end;

                params.set(key, TryGet(value, empty_str));

                end: ++i;
            }
            builder.setPostScaling(Scaling(inputSampleType, outputSampleType, type, params));
        }

        try {
            return (signal.setDescriptor(builder.build()), EC_OK);
        } catch(const std::exception& err) {
            std::cerr << err.what() << std::endl;
            return EC_OPENDAQ_ERROR;
        }
    } catch(...) {}
    return EC_INVALID_JSON;
}

int AppSignal::getCount(const SignalPtr& signal, const string_view item)
{
    if (item == "related")
    {
        return signal.getRelatedSignals().getCount();
    }

    return AppPropertyObject::getCount(signal, item);
}

END_NAMESPACE_OPENDAQ
