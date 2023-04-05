#include <iostream>
#include <cassert>
#include <sstream>

// Sensor initialisation
float sensorTempOutside = 0.f, sensorTempInside = 22.f;
bool sensorMoveOutside = false, sensorMoveInside = false;
int switchesState = 0;
int systemLightColour = 0;

enum switches {
    MAIN_POWER = 1,
    HOUSE_SOCKETS = 2,
    LIGHT_INSIDE = 4,
    LIGHT_OUTSIDE = 8,
    HEATERS_INSIDE = 16,
    HEATERS_OUT_WATER_PIPE = 32,
    CONDITIONER = 64
};

void outSplitter (char sign) {
    std::string splitter;
    splitter.assign(25, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "Smart house" << std::endl;
    outSplitter('=');
}

std::string intToString(int number) {

    char result[12];

    sprintf(result, "%d", number);

    return (std::string) result;

}

std::string presDateTimeForOut(int day, int hour) {

    assert(day < 100 && day >= 0 && hour <= 23 && hour >= 0);

    std::string result = "Day: ";

    // Add day
    char buffer[3];
    sprintf(buffer, "%2d", day);
    result += buffer;

    // Add hour
    result += "  Hour: ";
    sprintf(buffer, "%2d", hour);
    if (buffer[3] == ' ') buffer[0] = '0';
    result += buffer;
    result += ":00";

    return result;

}

bool isValidNumericString(std::string checkedString) {

    // Permit (digit - required and correct mark):
    // 1st char: sign, dot, digit
    // 2nd char: dot (if 1st - not dot), digit
    // 3rd char: digit
    bool dotFound = false;

    for (int i = 0; i < 3; ++i) {
        char checkedChar = checkedString[i];
        if (checkedChar >= '0' && checkedChar <= '9') return true; // Correct number string
        else {
            // Not digit char
            if (i == 0) {
                if (checkedChar == '.') dotFound = true;
                else if (checkedChar != '+' && checkedChar != '-') return false;
            } else if (i == 1 && checkedChar == '.' && dotFound
                       || i > 1) return false;
        }
    }

    return false;

}

std::string switchStateString (switches switchMask) {

    return (switchesState & switchMask ? "ON" : "OFF");

}

// Change switchingSystem state if required
// Return confirmation string of the instruction
std::string switchSystemState(switches switchingSystem, std::string systemDescription, bool requiredState) {

    std::string result;

    if (requiredState !=
        (bool) (switchesState & switchingSystem)) {
        // Incorrect switchingSystem state
        switchesState ^= switchingSystem;
        result = systemDescription + " - " + switchStateString(switchingSystem) + "\n";
    }

    return result;

}

void smartModule(int hour) {

    // Parameters
    // Temperatures
    float tempAccuracy = .01f;
    float temp_OutWaterPipeHeater_On = 0.f;
    float temp_OutWaterPipeHeater_Off = 5.f;
    float temp_InHouseHeater_On = 22.f;
    float temp_InHouseHeater_Off = 25.f;
    float temp_Conditioner_On = 30.f;
    float temp_Conditioner_Off = 25.f;
    // Times
    int time_OutLight_On = 16;
    int time_OutLight_Off = 5;
    // Light colour
    int timeLightColourDay_Start = 0;
    int time_LightColorDay_Finish = 16;
    int time_LightColorNight_Start = 20;
    int lightColour_Day = 5000;
    int lightColour_Night = 2700;
    int lightColorEveningStep = (lightColour_Night - lightColour_Day)
        / (time_LightColorNight_Start - time_LightColorDay_Finish);

    // Sensors states
    outSplitter('-');
    std::cout << "Sensors states:\n";
    std::cout << "Outside temperature: " << sensorTempOutside << std::endl;
    std::cout << "Inside temperature: " << sensorTempInside << std::endl;
    std::cout << "Outside moving: " << (sensorMoveOutside ? "yes" : "no") << std::endl;
    std::cout << "Inside moving: " << (sensorMoveInside ? "yes" : "no") << std::endl;

    // Prepare instruction
    std::string instruction;
    // Outside water pipe heater
    instruction += switchSystemState(
            HEATERS_OUT_WATER_PIPE, "Outside water pipe heater",
            sensorTempOutside < (temp_OutWaterPipeHeater_On - tempAccuracy)
                || (bool) (switchesState & HEATERS_OUT_WATER_PIPE)
                    && sensorTempOutside <= (temp_OutWaterPipeHeater_Off + tempAccuracy));

    // Inside house heater
    instruction += switchSystemState(
            HEATERS_INSIDE, "Inside house heater",
            sensorTempInside < (temp_InHouseHeater_On - tempAccuracy)
            || (bool) (switchesState & HEATERS_INSIDE)
               && sensorTempInside < (temp_InHouseHeater_Off - tempAccuracy));

    // Conditioner
    instruction += switchSystemState(
            CONDITIONER, "Conditioner",
            sensorTempInside >= (temp_Conditioner_On - tempAccuracy)
            || (bool) (switchesState & CONDITIONER)
               && sensorTempInside > (temp_Conditioner_Off + tempAccuracy));

    // Outside light
    instruction += switchSystemState(
            LIGHT_OUTSIDE, "Outside light",
            sensorMoveOutside
                && (time_OutLight_Off > hour || time_OutLight_On <= hour));

    // Inside house light
    instruction += switchSystemState(
            LIGHT_INSIDE, "Inside light",
            sensorMoveInside);
    // Colour temperature inside house light
    if (switchesState & LIGHT_INSIDE) {
        // Define colour
        int newLightColour = lightColour_Day;
        if (time_LightColorNight_Start <= hour || timeLightColourDay_Start > hour)
            newLightColour = lightColour_Night;
        else if (time_LightColorDay_Finish < hour)
            newLightColour = lightColour_Day
                    + lightColorEveningStep * (hour - time_LightColorDay_Finish);
        // Set instruction
        instruction += "Inside light colour";
        if (systemLightColour != newLightColour) {
            instruction += " set to";
            systemLightColour = newLightColour;
        }
        instruction += " : " + intToString(systemLightColour) + '\n';
    }

    // Check main power (without power sequence)
    if (switchesState & ~MAIN_POWER) instruction += switchSystemState(
            MAIN_POWER, "Main power", true);

    // Confirm instruction
    if (!instruction.empty()) {
        outSplitter('-');
        std::cout << instruction;
    }

}

int main() {

    // Initialization
    int day = 1, hour = 0;

    outHeader();

    while (day <= 2) {

        // Get data of sensors
        std::string userInput;
        std::cout << presDateTimeForOut(day, hour) << std::endl;
        std::cout << "enter sensors data, separated by spaces,\n";
        std::cout << "moving state input - yes/no\n";
        std::cout << "(Temp_Outside Temp_Inside Move_Outside Move_Inside):\n";
        std::getline(std::cin, userInput);

        // Enter sensors data
        bool correctSensorData = true;
        std::string sensorDataString;
        std::stringstream userInputStream(userInput);
        // Outside temperature
        userInputStream >> sensorDataString;
        if (isValidNumericString(sensorDataString)) sensorTempOutside = std::stof(sensorDataString);
        else {
            std::cerr << "Wrong outside temperature!\n";
            correctSensorData = false;
        }
        // Inside temperature
        userInputStream >> sensorDataString;
        if (isValidNumericString(sensorDataString)) sensorTempInside = std::stof(sensorDataString);
        else {
            std::cerr << "Wrong inside temperature!\n";
            correctSensorData = false;
        }
        // Outside moving
        userInputStream >> sensorDataString;
        if (sensorDataString == "yes") sensorMoveOutside = true;
        else if (sensorDataString == "no") sensorMoveOutside = false;
        else {
            std::cerr << "Wrong outside moving state!\n";
            correctSensorData = false;
        }
        // Inside moving
        userInputStream >> sensorDataString;
        if (sensorDataString == "yes") sensorMoveInside = true;
        else if (sensorDataString == "no") sensorMoveInside = false;
        else {
            std::cerr << "Wrong inside moving state!\n";
            correctSensorData = false;
        }

        if (correctSensorData) {

            // Smart operation
            smartModule(hour);

            // Next hour
            hour = (hour == 23 ? 0 : hour + 1);
            if (hour == 0) ++day;

        }

        outSplitter('-');

    }

    std::cout << "Beta period finished" << std::endl;

    return 0;

}
