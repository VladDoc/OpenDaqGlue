#ifdef _WIN32
#include "dlfcn-win32/src/dlfcn.h"
const char* modulename = "opendaq-bridge.dll";
#else
#include <dlfcn.h>
const char* modulename = "./libopendaq-bridge.so";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "ColoredPrinter.h"
#include "ErrorCodes.h"

#define UGLYCAST(x) (*(void **) (&x))

#define GETFUN(x, handle) (UGLYCAST(x) = dlsym(handle, #x))

typedef void* DaqObjectPtr;
typedef void** DaqObjectPtrArray;


char*		  (*LibraryInfo)		     (void);
void 		  (*LibraryHelp) 			 (void);
void          (*StdOut_PipeToString)     (void);
const char*   (*StdOut_GetBufferString)  (void);
void          (*StdOut_Default)          (void);
void          (*StdOut_EraseBuffer)      (void);

DaqObjectPtr (*Instance_New)         	 (void);
void         (*OpenDaqObject_Free)       (DaqObjectPtr self);
void         (*StringPool_Free)       	 (const char* str);

int          (*OpenDaqObject_Print)   (DaqObjectPtr self, const char* item);
int          (*OpenDaqObject_List)    (DaqObjectPtr self, const char* type);
const char*  (*OpenDaqObject_Get)     (DaqObjectPtr self, const char* item);
int          (*OpenDaqObject_GetCount)(DaqObjectPtr self, const char* itemArray);
int          (*OpenDaqObject_Set)     (DaqObjectPtr self, const char* item, const char* value);
DaqObjectPtr (*OpenDaqObject_Select)  (DaqObjectPtr self, const char* type, uint64_t index);
void         (*OpenDaqObject_Help)    (DaqObjectPtr self);

DaqObjectPtr (*Device_Add)              (DaqObjectPtr device, const char* type, const char* value);
DaqObjectPtr (*Device_AddDevice)        (DaqObjectPtr device, const char* connectionString);
DaqObjectPtr (*Device_AddFunctionBlock) (DaqObjectPtr device, const char* fbId);
int          (*Device_Remove)           (DaqObjectPtr device, const char* type, uint64_t index);
int          (*Device_RemoveDevice)     (DaqObjectPtr device, uint64_t index);
int          (*Device_RemoveFunctionBlock)(DaqObjectPtr device, uint64_t index);

const char*  (*Device_GetAvailableDeviceConnectionString)(DaqObjectPtr device, uint64_t index);
const char*  (*Device_GetAvailableFunctionBlockID)(DaqObjectPtr device, uint64_t index);

int          (*Device_LoadConfiguration)(DaqObjectPtr device, const char* json);
int          (*Device_LoadConfigurationFromFile)(DaqObjectPtr device, const char* json_path);
const char*  (*Device_SaveConfiguration)(DaqObjectPtr device);
int          (*Device_SaveConfigurationToFile)(DaqObjectPtr device, const char* json_path);

int          (*InputPort_Connect)(DaqObjectPtr port, const char* signalid, DaqObjectPtr instance);
int          (*InputPort_Disconnect)(DaqObjectPtr port);

int          (*Signal_Read)(DaqObjectPtr signal, uint64_t NumOfSamples, int timeout);

double*      (*Signal_GetSampleReadings)(DaqObjectPtr signal);
int64_t*     (*Signal_GetSampleTimeStamps)(DaqObjectPtr signal);

int          (*Signal_GetSampleReadingsToArray)(DaqObjectPtr signal, double* array, uint64_t len);
int          (*Signal_GetSampleTimeStampsToArray)(DaqObjectPtr signal, int64_t* array, uint64_t len);

int          (*Signal_GetSampleCountOfRead)(DaqObjectPtr signal);
int          (*Signal_EraseSamples)(DaqObjectPtr signal);
const char*  (*TimeStampToString)(int64_t timestamp);

int          (*Signal_SendDataPacket)(DaqObjectPtr signal, double* data, uint64_t count);
int          (*Signal_SendTestDataPacket)(DaqObjectPtr signal, uint64_t count, double sine_range);

int          (*Signal_LoadDataDescriptorFromJson)(DaqObjectPtr signal, const char* json);
int          (*Signal_LoadDataDescriptorFromJsonFile)(DaqObjectPtr signal, const char* path);

const char*  (*DataDescriptor_SaveToJson)(DaqObjectPtr signal);
int          (*DataDescriptor_SaveToJsonFile)(DaqObjectPtr signal, const char* path);

int 		 (*MultiReader_ReadToArrays)(int64_t multiReaderId,
										 uint64_t NumOfSamples, int timeout,
										 double** data, int64_t** timestamps);

int64_t      (*MultiReader_Bind)(DaqObjectPtrArray signals, uint64_t NumOfSignals);
int          (*MultiReader_UnBind)(int64_t multiReaderId);


void InitFunctions(void* handle)
{

	GETFUN(LibraryHelp, handle);
	GETFUN(LibraryInfo, handle);
	GETFUN(StdOut_Default, handle);
	GETFUN(StdOut_GetBufferString, handle);
	GETFUN(StdOut_PipeToString, handle);
	GETFUN(StdOut_EraseBuffer, handle);

	GETFUN(Instance_New, handle);
	GETFUN(OpenDaqObject_Free, handle);
	GETFUN(StringPool_Free, handle);

	GETFUN(OpenDaqObject_Print, handle);
	GETFUN(OpenDaqObject_List, handle);
	GETFUN(OpenDaqObject_Get, handle);
	GETFUN(OpenDaqObject_GetCount, handle);
	GETFUN(OpenDaqObject_Set, handle);
	GETFUN(OpenDaqObject_Select, handle);
	GETFUN(OpenDaqObject_Help, handle);

	GETFUN(Device_Add, handle);
	GETFUN(Device_AddDevice, handle);
	GETFUN(Device_AddFunctionBlock, handle);
	GETFUN(Device_Remove, handle);
	GETFUN(Device_RemoveDevice, handle);
	GETFUN(Device_RemoveFunctionBlock, handle);

	GETFUN(Device_GetAvailableDeviceConnectionString, handle);
	GETFUN(Device_GetAvailableFunctionBlockID, handle);

	GETFUN(Device_LoadConfiguration, handle);
	GETFUN(Device_LoadConfigurationFromFile, handle);
	GETFUN(Device_SaveConfiguration, handle);
	GETFUN(Device_SaveConfigurationToFile, handle);

	GETFUN(InputPort_Connect, handle);
	GETFUN(InputPort_Disconnect, handle);

	GETFUN(Signal_Read, handle);
	GETFUN(Signal_GetSampleReadings, handle);
	GETFUN(Signal_GetSampleTimeStamps, handle);
	GETFUN(Signal_GetSampleReadingsToArray, handle);
	GETFUN(Signal_GetSampleTimeStampsToArray, handle);
	GETFUN(Signal_GetSampleCountOfRead, handle);
	GETFUN(Signal_EraseSamples, handle);

	GETFUN(Signal_SendDataPacket, handle);
	GETFUN(Signal_SendTestDataPacket, handle);

	GETFUN(Signal_LoadDataDescriptorFromJson, handle);
	GETFUN(Signal_LoadDataDescriptorFromJsonFile, handle);

	GETFUN(DataDescriptor_SaveToJson, handle);
	GETFUN(DataDescriptor_SaveToJsonFile, handle);

	GETFUN(TimeStampToString, handle);
	GETFUN(MultiReader_ReadToArrays, handle);
	GETFUN(MultiReader_Bind, handle);
	GETFUN(MultiReader_UnBind, handle);
}

void SaveToCSV(const char* path, double* values, int size)
{
	FILE* f = fopen(path, "w");
	if(!f) return;

	for(int i = 0; i < size-1; ++i) {
		fprintf(f, "%f,", values[i]);
	}

	fprintf(f, "%f\n", values[size-1]);
}


void Test_CheckStdOutRedirect()
{
	StdOut_PipeToString();
	LibraryHelp();
	const char* str = StdOut_GetBufferString();
	StdOut_Default();
	printf("%s", str);
	const char* str2 = LibraryInfo();
	assert(str);
	assert(str2);

	puts(str);
	puts(str2);

	StdOut_EraseBuffer();

	Success();
	puts("Test_CheckStdOutRedirect: Success\n");
	ResetColors();
}


static const char* test_config_path = "config.json";
void Test_ChangeConfig()
{
	DaqObjectPtr instance = Instance_New();
	assert(instance);

	Device_LoadConfiguration(instance, "");
	OpenDaqObject_Free(instance);

	instance = Instance_New();

	do {
		PrintInfo("Loading Test Config");
		const char* old_config = Device_SaveConfiguration(instance);
		Device_LoadConfigurationFromFile(instance, test_config_path);
		const char* str = Device_SaveConfiguration(instance);
		assert(strcmp(old_config, str) != 0);

		puts(str);

		StringPool_Free(str);

		OpenDaqObject_Free(instance);

		instance = Instance_New();
		Device_LoadConfiguration(instance, "");

		StringPool_Free(old_config);
	} while(0);

	OpenDaqObject_Free(instance);
}


#define NUM_CHANNELS 2
#define NUM_CHANNELS_STR "2"
#define SAMPLE_RATE 1000
#define SAMPLE_RATE_STR "1000"
#define READING_TIMEOUT 40
#define NUM_SAMPLES 20

void Test_MultiRead()
{
	DaqObjectPtr instance = Instance_New();
	assert(instance);

	const char* connectionStringDev = NULL;
	do {
		int devIndex = 0;
		connectionStringDev =
			Device_GetAvailableDeviceConnectionString(instance, devIndex);
		assert(connectionStringDev);
		Info();
		printf("Connection String for Device[%d]: %s\n", devIndex, connectionStringDev);
		ResetColors();
	} while(0);
	DaqObjectPtr dev = NULL;
	do {
		PrintInfo("Adding Device:");

		dev = Device_AddDevice(instance, connectionStringDev);

		OpenDaqObject_Set(dev, "NumberOfChannels", NUM_CHANNELS_STR);
		OpenDaqObject_Set(dev, "GlobalSampleRate", SAMPLE_RATE_STR);
		OpenDaqObject_Set(dev, "AcquisitionLoopTime", "10");

		OpenDaqObject_List(dev, "properties");
	} while(0);


	DaqObjectPtr channels[NUM_CHANNELS];

	do {
		PrintInfo("Selecting Channels:");

		for(int i = 0; i < NUM_CHANNELS; ++i) {
			channels[i] = OpenDaqObject_Select(dev, "channel", i);
			assert(channels[i]);
			Info();
			printf("Channel #%d:\n", i);
			ResetColors();
			OpenDaqObject_List(channels[i], "properties");
		}
	} while(0);

	DaqObjectPtr signals[NUM_CHANNELS];
	do {
		PrintInfo("Selecting Signals:");

		for(int i = 0; i < NUM_CHANNELS; ++i) {
			signals[i] = OpenDaqObject_Select(channels[i], "signal", 0);
			assert(signals[i]);
			Info();
			printf("Signal #%d:\n", i);
			ResetColors();
			OpenDaqObject_List(signals[i], "properties");
		}
	} while(0);

	int totalSamples = SAMPLE_RATE * 1; // sample rate * seconds

	double* 	data[NUM_CHANNELS];
	int64_t* 	times[NUM_CHANNELS];

	for(int i = 0; i < NUM_CHANNELS; ++i) {
		data[i] 	= calloc(totalSamples, sizeof(*data[i]));
		times[i] 	= calloc(totalSamples, sizeof(*times[i]));
	}

	// create temp pointers to hold current positions within the sub-buffers
	double* tempDataPtrs[NUM_CHANNELS];
	int64_t* tempTimesPtrs[NUM_CHANNELS];

	for(int i = 0; i < NUM_CHANNELS; ++i) {
		tempDataPtrs[i] = &data[i][0];
		tempTimesPtrs[i] = &times[i][0];
	}

	do {
		PrintInfo("Reading Samples From All Signals");

		int multireaderId = MultiReader_Bind(signals, NUM_CHANNELS);

		int current = 0;
		while(current < totalSamples) {
			int count =
				MultiReader_ReadToArrays(multireaderId, NUM_SAMPLES, READING_TIMEOUT,
										 tempDataPtrs, tempTimesPtrs);

			printf("Count: %d\n", count);
			if(count == 0) continue;

			// shift temp pointers by the amount read
			for(int i = 0; i < NUM_CHANNELS; ++i) {
				tempDataPtrs[i] += count;
				tempTimesPtrs[i] += count;
			}

			if(current + count > totalSamples)
				count = totalSamples - current;

			current += count;
		}
		MultiReader_UnBind(multireaderId);

		assert(current == totalSamples);

		for(int i = 0; i < totalSamples; ++i) {
			const char* timestr = TimeStampToString(times[0][i]);

			printf("%f:%s\n", data[0][i], timestr);
		}

		for(int i = 0; i < NUM_CHANNELS; ++i) {
			char output[128];
			sprintf(output, "output-channel-%d.csv", i);

			SaveToCSV(output, data[i], totalSamples);
		}
	} while(0);

	for(int i = 0; i < NUM_CHANNELS; ++i) {
		OpenDaqObject_Free(signals[i]);
	}

	for(int i = 0; i < NUM_CHANNELS; ++i) {
		OpenDaqObject_Free(channels[i]);
	}

	OpenDaqObject_Free(dev);
}

void Test_CheckInstance()
{
	DaqObjectPtr instance = Instance_New();
	assert(instance);

	do {
		PrintInfo("Listing available devices: ");
		assert(OpenDaqObject_List(instance, "available-devices") == EC_OK);
	} while(0);

	do {
		PrintInfo("Listing function blocks:");
		assert(OpenDaqObject_List(instance, "available-function-blocks") == EC_OK);
	} while(0);

	do {
		PrintInfo("Listing properties: ");
		assert(OpenDaqObject_List(instance, "properties") == EC_OK);
	} while(0);

	do{
		PrintInfo("Help info: ");
		OpenDaqObject_Help(instance);
	}while(0);

	const char* connectionStringDev0 = NULL;

	do {
		PrintInfo("Getting available device connection string: ");

		int count = OpenDaqObject_GetCount(instance, "available-devices");
		printf("Total devices count: %d\n", count);
		int index = 5;
		if(count > 0)
			connectionStringDev0 = Device_GetAvailableDeviceConnectionString(instance, index);
		printf("Connection String for Device[%d]: %s\n", index, connectionStringDev0);
	} while(0);

	//connectionStringDev0 = "daqref://device0";

	DaqObjectPtr dev0 = NULL;
	do {
		PrintInfo("Adding Device:");

		dev0 = Device_AddDevice(instance, connectionStringDev0);

		//dev0 = OpenDaqObject_Select(instance, "device", 0);
		assert(dev0);
		const char * name = OpenDaqObject_Get(dev0, "name");
		printf("Device Name: %s\n", name);

		OpenDaqObject_List(dev0, "properties");

		OpenDaqObject_Set(dev0, "NumberOfChannels", "1");
		OpenDaqObject_Set(dev0, "GlobalSampleRate", "10000");
		OpenDaqObject_Set(dev0, "AcquisitionLoopTime", "10");

		OpenDaqObject_List(dev0, "properties");

		StringPool_Free(name);
	} while(0);

	DaqObjectPtr fb0 = NULL;
	do {
		PrintInfo("Adding Function block:");

		fb0 = Device_AddFunctionBlock(dev0, "RefFBModuleRenderer");
		assert(fb0);

		OpenDaqObject_List(fb0, "properties");

		//OpenDaqObject_Set(fb0, "NumberOfChannels", "1");
		//OpenDaqObject_Set(fb0, "GlobalSampleRate", "10000");
		//OpenDaqObject_Set(fb0, "AcquisitionLoopTime", "10");
	} while(0);
	getchar();

	DaqObjectPtr channel;
	do {
		PrintInfo("Reading Channel Properties");

		channel = OpenDaqObject_Select(dev0, "channel", 0);
		assert(channel);

		OpenDaqObject_List(channel, "signals");
		OpenDaqObject_List(channel, "input-ports");

		OpenDaqObject_Set(channel, "Amplitude", "6");
		OpenDaqObject_Set(channel, "Frequency", "20");
		OpenDaqObject_Set(channel, "UseGlobalSampleRate", "false");
		OpenDaqObject_Set(channel, "SampleRate", "44100");

		const char* sampleRateReading = OpenDaqObject_Get(channel, "SampleRate");
		assert(sampleRateReading);

		OpenDaqObject_List(channel, "properties");

		//printf("SampleRate: %s", sampleRateReading);

		StringPool_Free(sampleRateReading);
	} while(0);

	do {
		PrintInfo("Reading Sync Properties");

		DaqObjectPtr sync = OpenDaqObject_Select(dev0, "sync", 0);

		OpenDaqObject_List(sync, "properties");

		PrintInfo("Setting Value");
		OpenDaqObject_Set(sync, "SynchronizationLocked", "false");
		OpenDaqObject_List(sync, "properties");
	} while(0);

	DaqObjectPtr signal;

	do {
		DaqObjectPtr descriptor;

		PrintInfo("Reading Signal Properties");
		signal = OpenDaqObject_Select(channel, "signal", 0);
		assert(signal);
		OpenDaqObject_List(signal, "properties");

		descriptor = OpenDaqObject_Select(signal, "descriptor", 0);
		assert(descriptor);
		OpenDaqObject_List(descriptor, "properties");
		DataDescriptor_SaveToJsonFile(descriptor, "descriptor1.json");
		OpenDaqObject_Free(descriptor);

		Signal_LoadDataDescriptorFromJsonFile(signal, "test-desc1.json");
		descriptor = OpenDaqObject_Select(signal, "descriptor", 0);
		OpenDaqObject_List(descriptor, "properties");
		OpenDaqObject_Free(descriptor);

		PrintInfo("Reading Domain Signal Properties");
		DaqObjectPtr domain = OpenDaqObject_Select(signal, "domain-signal", 0);
		assert(domain);
		OpenDaqObject_List(domain, "properties");

		descriptor = OpenDaqObject_Select(domain, "descriptor", 0);
		assert(descriptor);
		OpenDaqObject_List(descriptor, "properties");
		DataDescriptor_SaveToJsonFile(descriptor, "descriptor2.json");

		Signal_LoadDataDescriptorFromJsonFile(domain, "test-desc2.json");
		descriptor = OpenDaqObject_Select(domain, "descriptor", 0);
		OpenDaqObject_List(descriptor, "properties");
		OpenDaqObject_Free(descriptor);

		OpenDaqObject_Free(domain);
	} while(0);

	DaqObjectPtr functionBlock;
	do {
		//PrintInfo("Reading Function Block Properties");
		//functionBlock = OpenDaqObject_Select(dev0, "function-block", 0);
		//assert(functionBlock);
		//OpenDaqObject_List(signal, "properties");
	} while(0);



	//const char* config;
	do {
		//PrintInfo("Reading Instance Config:");
		//config = Device_SaveConfiguration(instance);
		//assert(config);
		//printf("%s\n", config);
	} while(0);

	do {
		//PrintInfo("Applying Instance Config:");
		//assert(Device_LoadConfiguration(instance, test_config) == EC_OK);

		//const char* config2 = Device_SaveConfiguration(instance);
		//printf("%s\n", config2);
		//assert(strcmp(config2, test_config) == 0);
		//StringPool_Free(config2);
	} while(0);

	//StringPool_Free(config);

	int num = 1000;

	double data[1000] = {};
	int64_t times[1000] = {};

	do {
		PrintInfo("Reading Samples From Signal");

		channel = OpenDaqObject_Select(dev0, "channel", 0);
		assert(channel);

		signal = OpenDaqObject_Select(channel, "signal", 0);
		assert(signal);

		int current = 0;

		while(current < num) {
			int count = Signal_Read(signal, 20, 20);
			printf("Count: %d\n", count);
			//assert(count > 0);
			if(count == 0) continue;

			if(current + count > num)
				count = num - current;

			Signal_GetSampleReadingsToArray(signal, data+current, count);
			Signal_GetSampleTimeStampsToArray(signal, times+current, count);
			current += count;
		}

		assert(current == num);

		for(int i = 0; i < current; ++i) {
			const char* timestr = TimeStampToString(times[i]);

			printf("%f:%s\n", data[i], timestr);
		}

		SaveToCSV("output.csv", data, num);
	} while(0);


	do {
		PrintInfo("Sending Test Data to Signal");

		Signal_SendTestDataPacket(signal, num, 10.0);
	} while(0);

	do {
		PrintInfo("Sending Data to Signal");

		Signal_SendDataPacket(signal, data, num);
	} while(0);


	OpenDaqObject_Free(functionBlock);
	OpenDaqObject_Free(signal);
	OpenDaqObject_Free(channel);
	OpenDaqObject_Free(dev0);
	OpenDaqObject_Free(instance);

	Success();
	puts("Test_CheckInstance: Success\n");
	ResetColors();
}

void Test_Main()
{
	Test_CheckStdOutRedirect();
	//Test_ChangeConfig();
	//Test_CheckInstance();
	Test_MultiRead();
}

int main(void)
{
	char*	error;
	void*	handle;

	handle = dlopen(modulename, RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();/* Clear any existing error */

	InitFunctions(handle);

	Test_Main();

	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	dlclose(handle);
	exit(EXIT_SUCCESS);
}
