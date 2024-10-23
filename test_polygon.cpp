#include <coreobjects/unit_factory.h>
#include <opendaq/context_factory.h>
#include <opendaq/data_rule_factory.h>
#include <opendaq/packet_factory.h>
#include <opendaq/reader_factory.h>
#include <opendaq/scheduler_factory.h>
#include <opendaq/signal_factory.h>
#include <opendaq/time_reader.h>
#include <opendaq/opendaq.h>

#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

using namespace daq;
using namespace date;

SignalConfigPtr setupExampleSignal();
SignalPtr setupExampleDomain(const SignalPtr& value);
DataPacketPtr createPacketForSignal(const SignalPtr& signal, SizeT numSamples, Int offset = 0);
DataDescriptorPtr setupDescriptor(SampleType type, const daq::DataRulePtr& rule = nullptr);
DataDescriptorBuilderPtr setupDescriptorBuilder(SampleType type, const daq::DataRulePtr& rule = nullptr);

/*
 * Example 1: Reading time with Time Reader
 */
void example1(const SignalConfigPtr& signal)
{
    auto reader = StreamReader(signal);

    constexpr int num = 1000;

    // Signal produces 5 samples
    auto packet = createPacketForSignal(signal, num);
    auto data = static_cast<double*>(packet.getData());
    for(auto i = 0u; i < num; ++i)
        data[i] = i+1;

    signal.sendPacket(packet);

    auto timeReader = TimeReader(reader);

    SizeT count = num;
    double values[num]{};
    std::chrono::system_clock::time_point timeStamps[num]{};

    // read with Time Reader
    timeReader.readWithDomain(values, timeStamps, &count);
    //assert(count == num);

    for (SizeT i = 0u; i < count; ++i)
    {
        std::cout << timeStamps[i] << ": " << values[i] << std::endl;
        assert(values[i] == i + 1);
    }

    std::cout << std::endl;
}

/*
 * Example 2: Reading time with the wrapped reader
 */
void example2(const SignalConfigPtr& signal)
{
    auto reader = StreamReader(signal);

    // Signal produces 5 samples
    auto packet = createPacketForSignal(signal, 5);
    auto data = static_cast<double*>(packet.getData());
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;
    data[4] = 5;
    signal.sendPacket(packet);

    auto timeReader = TimeReader(reader);

    SizeT count{5};
    double values[5]{};
    std::chrono::system_clock::time_point timeStamps[5]{};

    // read with the wrapped Reader
    reader.readWithDomain(values, timeStamps, &count);
    assert(count == 5);

    for (SizeT i = 0u; i < count; ++i)
    {
        std::cout << timeStamps[i] << ": " << values[i] << std::endl;
        assert(values[i] == i + 1);
    }
}

void example3()
{
    // Create a fresh openDAQ(TM) instance that we will use for all the interactions with the openDAQ(TM) SDK
    daq::InstancePtr instance = daq::Instance(MODULE_PATH);

    // This will ignore daq ref and daq audio
    // Find and connect to a device hosting an openDAQ(TM) OPC UA server
    const auto availableDevices = instance.getAvailableDevices();
    daq::DevicePtr device;
    for (const auto& deviceInfo : availableDevices)
    {
        for (const auto & capability : deviceInfo.getServerCapabilities())
        {
            if (capability.getProtocolName() == "openDAQ OpcUa")
            {
                std::cout << capability.getConnectionString();
                device = instance.addDevice(capability.getConnectionString());
                break;
            }
        }	
    }

    // Exit if no device is found
    if (!device.assigned())
        return;
    
    // Get the first device channel
    daq::ChannelPtr channel = device.getChannels()[0];

    // Get the first channel signal
    daq::SignalPtr signal = channel.getSignals()[0];

    // Output 40 samples using reader
    using namespace std::chrono_literals;
    daq::StreamReaderPtr reader = daq::StreamReader<double, uint64_t>(signal);

    // Allocate buffer for reading double samples
    double samples[100];
    int cnt = 0;
    while (cnt < 40)
    {
        std::this_thread::sleep_for(25ms);

        // Read up to 100 samples, storing the amount read into `count`
        daq::SizeT count = 100;
        reader.read(samples, &count);
        if (count > 0)
        {
            std::cout << samples[count - 1] << std::endl;
            cnt++;
        }
    }

    std::cout << "Press \"enter\" to exit the application..." << std::endl;
    std::cin.get();
}


/*
 * ENTRY POINT
 */
int main(int /*argc*/, const char* /*argv*/ [])
{
    SignalConfigPtr signal = setupExampleSignal();
    signal.setDomainSignal(setupExampleDomain(signal));

    /*
      The output in both examples should be:
      
        2022-09-27 00:02:03.0000000: 1
        2022-09-27 00:02:03.0010000: 2
        2022-09-27 00:02:03.0020000: 3
        2022-09-27 00:02:03.0030000: 4
        2022-09-27 00:02:03.0040000: 5
     */

    example1(signal);
    //example2(signal);
    //example3();

    return 0;
}

/*
 * Set up the Signal with Float64 data
 */
SignalConfigPtr setupExampleSignal()
{
    auto logger = Logger();
    auto context = Context(Scheduler(logger, 1), logger, nullptr, nullptr);

    auto signal = Signal(context, nullptr, "example signal");
    signal.setDescriptor(setupDescriptor(SampleType::Float64));

    return signal;
}

SignalPtr setupExampleDomain(const SignalPtr& value)
{
    auto domainDataDescriptor = setupDescriptorBuilder(SampleTypeFromType<ClockTick>::SampleType, daq::LinearDataRule(1, 0))
                                .setOrigin("2022-09-27T00:02:03+00:00")
                                .setTickResolution(Ratio(1, 1000))
                                .setUnit(Unit("s", -1, "seconds", "time"))
                                .build();

    auto domain = Signal(value.getContext(), nullptr, "domain signal");
    domain.setDescriptor(domainDataDescriptor);

    return domain;
}

DataDescriptorPtr setupDescriptor(daq::SampleType type, const daq::DataRulePtr& rule)
{
    return setupDescriptorBuilder(type, rule).build();
}

DataDescriptorBuilderPtr setupDescriptorBuilder(daq::SampleType type, const daq::DataRulePtr& rule)
{
    // Set up the data descriptor with the provided Sample-Type
    const auto dataDescriptor = daq::DataDescriptorBuilder().setSampleType(type);

    // For the Domain, we provide a Linear Rule to generate time-stamps
    if (rule.assigned())
        dataDescriptor.setRule(rule);

    return dataDescriptor;
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
