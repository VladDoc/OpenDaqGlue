#include "app_property_object.h"
#include <coreobjects/eval_value_factory.h>
#include <iostream>
#include <iomanip>

BEGIN_NAMESPACE_OPENDAQ

bool AppPropertyObject::processCommand(OpenDaqObject& propObj, const std::vector<std::string>& command)
{
    auto propPtr = propObj.object.asPtr<IPropertyObject>();
    if (command.empty())
        return false;

    if (command[0] == "list")
        return list(propPtr, command[1]);
    if (command[0] == "print" || command[0] == "get")
        return print(propPtr, command[1]);
    if (command[0] == "set")
        return set(propPtr, command[1], command[2]);
    if (command[0] == "help")
        return (help(), true);

    return false;
}

int AppPropertyObject::set(const PropertyObjectPtr& prop, const string_view item, const string_view value)
{
    auto itemPtr = daq::String(item.data(), item.size());
    auto valuePtr = daq::String(value.data(), value.size());
    if (!prop.hasProperty(itemPtr))
    {
        std::cout << "Property not available." << std::endl;
        return EC_PROPERTY_DOESNT_EXIST;
    }

    try
    {
        const auto eval = EvalValue(valuePtr);
        const BaseObjectPtr evaluated = eval.getResult();
        prop.setPropertyValue(itemPtr, evaluated);
        return EC_OK;
    }
    catch (...)
    {
        std::cout << "Failed to set property value." << std::endl;
        return EC_PROPERTY_DOESNT_EXIST;
    }
}

int AppPropertyObject::print(const PropertyObjectPtr& prop, const string_view item)
{
    auto str = get(prop, item);
    return str.data() != nullptr ? EC_OK : EC_PROPERTY_DOESNT_EXIST;
}

::String AppPropertyObject::get(const PropertyObjectPtr& propObj, const string_view item)
{
    auto stringPtr = daq::String(item.data(), item.size());
    if (!propObj.hasProperty(stringPtr))
    {
        std::cout << "Property not available." << std::endl;
        return nullptr;
    }

    try
    {
        const StringPtr propStr = propObj.getPropertyValue(stringPtr).toString();
        std::cout << "Name : " << stringPtr << std::endl;
        std::cout << "Value : " << propStr << std::endl;
        printProperty(propObj.getProperty(stringPtr));

        return propStr.toStdString();
    }
    catch (...)
    {
        std::cout << "Failed to get property value." << std::endl;
    }
    return nullptr;
}

int AppPropertyObject::list(const PropertyObjectPtr& propObj, const string_view item)
{
    if (item != "properties")
        return EC_PROPERTY_DOESNT_EXIST;

    const auto properties = propObj.getVisibleProperties();
    int cnt = 0;
    for (auto propInfo : properties)
    {
        try
        {
            const std::string name = propInfo.getName();
            const std::string value = propObj.getPropertyValue(name).toString();
            std::cout << "[" + std::to_string(cnt) + "] " << name << " : " << value << std::endl;
            cnt++;
        }
        catch (...)
        {
        }
    }
    return EC_OK;
}

void AppPropertyObject::help()
{
    std::cout << std::setw(25) << std::left << "list properties"
              << "Lists all visible properties and their current values as strings." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "set <property> <value>"
              << "Sets the value of <property> to <value>. Evaluates the passed <value> using" << std::endl
              << std::setw(25) << ""
              << "the openDAQ EvalValue system." << std::endl
              << std::endl;

    std::cout << std::setw(25) << std::left << "get <property>"
              << "Gets the value of <property>, as well as the Property information. Does not" << std::endl
              << std::setw(25) << ""
              << "output coercer, validator, callableInfo, and refProperty information." << std::endl
              << std::endl;
}

void AppPropertyObject::printSingleInfo(const std::string& type, const std::string& info)
{
    std::cout << type << " : " << info << std::endl;
}

void AppPropertyObject::printProperty(const PropertyPtr& info)
{
    // Missing validator, coercer, callableInfo, refProperty, onValueChanged
    try
    {
        std::string type = coreTypeToString(info.getValueType());
        std::string description = info.getDescription().assigned() ? info.getDescription() : "";
        std::string defaultValue = info.getDefaultValue().assigned() ? info.getDefaultValue().toString() : "";
        std::string unit = info.getUnit().assigned() && info.getUnit().getSymbol().assigned() ? info.getUnit().getSymbol() : "";
        std::string min = info.getMinValue().assigned() ? info.getMinValue().toString() : "";
        std::string max = info.getMaxValue().assigned() ? info.getMaxValue().toString() : "";
        std::string selectionValues = info.getSelectionValues().assigned() ? info.getSelectionValues().toString() : "";
        
        std::string visible = info.getVisible() ? "True" : "False";
        std::string readOnly = info.getReadOnly() ? "True" : "False";
        std::string isReferenced = info.getIsReferenced() ? "True" : "False";

        printSingleInfo("Type", type);
        printSingleInfo("Description", description);
        printSingleInfo("Default value", defaultValue);
        printSingleInfo("Unit", unit);
        printSingleInfo("Min", min);
        printSingleInfo("Max", max);
        printSingleInfo("Visible", visible);
        printSingleInfo("Read only", readOnly);
        printSingleInfo("Selection values", selectionValues);
        printSingleInfo("Is referenced", isReferenced);
    }
    catch(...)
    {
        std::cout << "Property is invalid." << std::endl;
    }
}

std::string AppPropertyObject::coreTypeToString(CoreType type)
{
    switch (type)
    {
        case ctBool:
            return "Bool";
        case ctInt:
            return "Int";
        case ctFloat:
            return "Float";
        case ctString:
            return "String";
        case ctList:
            return "List";
        case ctDict:
            return "Dictionary";
        case ctRatio:
            return "Ratio";
        case ctProc:
            return "Procedure";
        case ctObject:
            return "Object";
        case ctBinaryData:
            return "Binary data";
        case ctFunc:
            return "Function";
        case ctComplexNumber:
            return "Complex number";
        case ctUndefined:
            return "Undefined";
        case ctStruct:
            return "Struct";
        default: ;
    }
    return "";
}

END_NAMESPACE_OPENDAQ
