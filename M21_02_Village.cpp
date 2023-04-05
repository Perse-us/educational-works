#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>

void outSplitter (char sign = '-') {
    std::string splitter;
    splitter.assign(25, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "Village description" << std::endl;
    outSplitter('=');
}

// Start initialization
const int floorsMin = 1, floorsMax = 3;
const int roomsMin = 2, roomsMax = 4;
const double doubleRoundPrecision = .01;
const double doublePrecision = .0001;
const double roomMinSize = .1;
const double areaMinSize = roomMinSize * roomsMax;
const double floorMinHeight = .5;

// Data position description
int landIndex = 0;
int buildingIndex = 0;
int floorIndex = 0;
int roomIndex = 0;

// Building types
enum class buildingsType{
    House, Garage, Barn, Bath, Other
};
const int buildingsTypeCount = 1 + (const int) buildingsType::Other;
const char* buildingsName[buildingsTypeCount]
        = {"House", "Garage", "Barn", "Bath", "Other"};

void outAllBuildingsName() {

    std::cout << "Available build types:\n";
    for (int i = 0; i < buildingsTypeCount; ++i)
        std::cout << i + 1 << " - \"" << buildingsName[i] << "\"\n";

}

// Rooms types
enum class roomsType{
    Living, Bed, Kitchen, Bath, Children, Other
};
const int roomsTypeCount = 1 + (const int) roomsType::Other;
const char* roomsName[roomsTypeCount]
    = {"Living room", "Bedroom","Kitchen", "Bathroom", "Children's room", "Other"};

void outAllRoomsName() {

    std::cout << "Available room types:\n";
    for (int i = 0; i < roomsTypeCount; ++i)
        std::cout << i + 1 << " - \"" << roomsName[i] << "\"\n";

}

// Structure description
struct roomDescription {
    roomsType type = roomsType::Other;
    std::string typeIfOther;
    double area = 0.; // Control with building
};

struct floorDescription {
    double height = 0;
    std::vector<roomDescription> rooms;
};

struct buildDescription {
    buildingsType type = buildingsType::Other;
    std::string typeIfOther;
    double area = 0; // Control with land
    std::vector<floorDescription> floors;
    bool chimney = false;
};

struct landDescription {
    int number = 0; // Unique
    double area = 0;
    std::vector<buildDescription> buildings;
};

bool getAvailChimney(const buildingsType& building) {
    return building == buildingsType::House
        || building == buildingsType::Bath;
}

bool getAvailFloors(const buildingsType& building) {
    return building == buildingsType::House;
}

void outLandData(const landDescription& landData) {

    // Land data out
    std::cout << "Land num. " << landData.number << " data:\n";
    std::cout << "Area: " << landData.area << std::endl;

    // Buildings description
    std::cout << "Buildings: " << landData.buildings.size() << std::endl;
    for (int buildIndex = 0; buildIndex < landData.buildings.size(); ++buildIndex) {

        outSplitter();

        std::cout << "Building " << (buildIndex + 1) << ":\n";
        const buildDescription& presBuilding = landData.buildings[buildIndex];

        // Data of buildings
        std::cout << "Type of build: " << buildingsName[(int) presBuilding.type]
            << (presBuilding.chimney ? " with chimney" : "");
        if (presBuilding.type == buildingsType::Other
                && !presBuilding.typeIfOther.empty())
            std::cout << " (" << presBuilding.typeIfOther << ")";
        std::cout << std::endl;
        std::cout << "Area of build: " << presBuilding.area << std::endl;

        // Floors description
        if (getAvailFloors(presBuilding.type)) {

            std::cout << "Floors in the build: " << presBuilding.floors.size() << std::endl;
            for (floorIndex = 0; floorIndex < presBuilding.floors.size(); ++floorIndex) {

                const char* floorIdent = "  ";

                std::cout << "Floor " << floorIndex + 1 << ":\n";
                const floorDescription& presFloor = presBuilding.floors[floorIndex];

                // Data of floor
                std::cout << floorIdent << "Height of floor: " << presFloor.height << std::endl;
                std::cout << floorIdent << "Rooms at floor: " << presFloor.rooms.size() << std::endl;

                // Rooms description
                for (roomIndex = 0; roomIndex < presFloor.rooms.size(); ++roomIndex) {

                    const char* roomIdent = "    ";

                    std::cout << floorIdent << "Room " << (roomIndex + 1) << ":\n";
                    const roomDescription& presRoom = presFloor.rooms[roomIndex];

                    // Data of room
                    std::cout << roomIdent << "Type of room: " << roomsName[(int) presRoom.type];
                    if (presRoom.type == roomsType::Other
                            && !presRoom.typeIfOther.empty())
                        std::cout << " (" << presRoom.typeIfOther << ")";
                    std::cout << std::endl;
                    std::cout << roomIdent << "Area of room: " << presRoom.area << std::endl;

                }

            }

        }

    }

}

void outLandDataIdsPos(int landIdx, int buildIdx = -1,
                     int floorIdx = -1, int roomIdx = -1) {
    std::cout << "Land: " << landIdx + 1;
    if (buildIdx >= 0) std::cout << ", Build: " << buildIdx + 1;
    if (floorIdx >= 0) std::cout << ", Floor: " << floorIdx + 1;
    if (roomIdx >= 0) std::cout << ", Room: " << roomIdx + 1;
    std::cout << std::endl;

}

std::stringstream getUserInputString() {

    std::string userInput;
    std::getline(std::cin, userInput);

    return std::stringstream (userInput);

}

void doubleRef_Round(double& value) {
    value = round(value / doubleRoundPrecision) * doubleRoundPrecision;
}

bool double_CheckEqual(double val_1, double val_2) {
    return val_1 <= val_2 + doublePrecision
        && val_1 >= val_2 - doublePrecision;
}

void enterRoomsData(floorDescription& presFloor,
                    buildDescription& presBuilding) {

    double buildingAreaRemain = presBuilding.area;
    for (roomIndex = 0; roomIndex < presFloor.rooms.size(); ++roomIndex) {

        // Room data
        std::stringstream userStream;
        roomDescription& presRoom = presFloor.rooms[roomIndex];

        // Get type of room
        while (true) {

            int selectRoomType = 1 + (int) roomsType::Other;

            // Get data
            outLandDataIdsPos(landIndex, buildingIndex,
                              floorIndex, roomIndex);
            outAllRoomsName();
            std::cout << "Enter type of room (from list above):\n";
            userStream = getUserInputString();
            userStream >> selectRoomType;
            outSplitter();

            // Check data
            if (selectRoomType < 1 || selectRoomType > roomsTypeCount)
                std::cout << "Wrong selected type!\n";
            else {
                presRoom.type = (roomsType)(selectRoomType - 1);
                break;
            }
            outSplitter(); // Wrong input

        }
        std::cout << "Selected room type: "
                  << roomsName[(int) presRoom.type] << std::endl;
        outSplitter();

        // Get type if other
        if (presRoom.type == roomsType::Other) {
            std::cout << "Enter extra type of room:\n";
            userStream = getUserInputString();
            userStream >> presRoom.typeIfOther;
            outSplitter();
        }

        // Get other data of room
        // Area
        double availRoomArea = buildingAreaRemain
                               - roomMinSize * (double) (presFloor.rooms.size() - roomIndex - 1);
        bool availRoomAreaEnter = true;
        if (double_CheckEqual(roomMinSize, availRoomArea)) {
            availRoomAreaEnter = false;
            presRoom.area = roomMinSize;
            std::cout << "Area of room set to: " << presRoom.area << " (minimum size)\n";
            outSplitter();
        }

        if (availRoomAreaEnter) {

            while (true) {

                // Get data
                outLandDataIdsPos(landIndex, buildingIndex,
                                  floorIndex, roomIndex);
                std::cout << "Enter description of room\n";
                std::cout << "- area of room (>=" << roomMinSize
                          << ", <=" << availRoomArea << "):\n";
                userStream = getUserInputString();
                userStream >> presRoom.area;
                doubleRef_Round(presRoom.area);
                outSplitter();

                // Check data
                if (presRoom.area < roomMinSize - doublePrecision
                    || presRoom.area > availRoomArea + doublePrecision)
                    std::cout << "Wrong room area!\n";
                else break;
                outSplitter(); // Wrong data

            }

        }

        buildingAreaRemain -= presRoom.area;

    }

}

void enterFloorsData(buildDescription& presBuilding) {

    for (floorIndex = 0; floorIndex < presBuilding.floors.size(); ++floorIndex) {

        // Floor data
        std::stringstream userStream;
        floorDescription& presFloor = presBuilding.floors[floorIndex];

        // Get floor data
        // Height, rooms
        int countOfRooms = 0;
        while (true) {

            outLandDataIdsPos(landIndex, buildingIndex, floorIndex);
            std::cout << "Enter description of floor\n";
            std::cout << "- height of floor (>=" << floorMinHeight << ", fractional valid)\n";
            std::cout << "- count of rooms (" << roomsMin << "..." << roomsMax << ")\n";
            std::cout << "(over space):\n";
            userStream = getUserInputString();
            userStream >> presFloor.height >> countOfRooms;
            doubleRef_Round(presFloor.height);
            outSplitter();

            // Check data
            if (presFloor.height < floorMinHeight - doublePrecision)
                std::cout << "Wrong floor height!\n";
            else if (countOfRooms < roomsMin || countOfRooms > roomsMax)
                std::cout << "Wrong count of rooms!\n";
            else break;
            outSplitter(); // Wrong input

        }

        // Enter data of rooms
        presFloor.rooms.resize(countOfRooms);
        enterRoomsData(presFloor, presBuilding);

    }

}

void enterBuildingsData(landDescription& presLand) {

    std::stringstream userStream;

    // Enter data of builds
    double landAreaRemain = presLand.area;
    for (buildingIndex = 0; buildingIndex < presLand.buildings.size(); ++buildingIndex) {

        // Building data
        buildDescription& presBuilding = presLand.buildings[buildingIndex];

        // Get type of build
        outLandDataIdsPos(landIndex, buildingIndex);
        if (buildingIndex == 0) {
            presBuilding.type = buildingsType::House; // Required living house
            std::cout << "Type of the first building set to: "
                      << buildingsName[(int) presBuilding.type] << std::endl;
        } else {

            while (true) {

                int selectBuildingType = 1 + (int) buildingsType::Other;

                // Get data
                outAllBuildingsName();
                std::cout << "Enter type of build (from list above):\n";
                userStream = getUserInputString();
                userStream >> selectBuildingType;
                outSplitter();

                // Check data
                if (selectBuildingType < 1 || selectBuildingType > buildingsTypeCount)
                    std::cout << "Wrong selected type!\n";
                else {
                    presBuilding.type = (buildingsType) (selectBuildingType - 1);
                    break;
                }
                outSplitter(); // Wrong input
                outLandDataIdsPos(landIndex, buildingIndex);

            }

            std::cout << "Selected building type: "
                      << buildingsName[(int) presBuilding.type] << std::endl;

        }
        outSplitter();

        // Get type if other
        if (presBuilding.type == buildingsType::Other) {
            std::cout << "Enter extra type of building:\n";
            userStream = getUserInputString();
            userStream >> presBuilding.typeIfOther;
            outSplitter();
        }

        // Get other data of build
        // Area, chimney, floors
        int countOfFloors = 0;
        double availLandArea = landAreaRemain
                               - areaMinSize * (double) (presLand.buildings.size() - buildingIndex - 1);
        bool availChimneyEnter = getAvailChimney(presBuilding.type);
        bool availFloorsEnter = getAvailFloors(presBuilding.type);
        bool availBuildingAreaEnter = true;
        if (double_CheckEqual(areaMinSize, availLandArea)) {
            availBuildingAreaEnter = false;
            presBuilding.area = areaMinSize;
            std::cout << "Area of building set to: " << presBuilding.area << " (minimum size)\n";
            outSplitter();
        }

        // Check need of enter
        if (availBuildingAreaEnter || availChimneyEnter || availFloorsEnter) {

            while (true) {

                // Get data
                outLandDataIdsPos(landIndex, buildingIndex);
                std::cout << "Enter description of build\n";
                if (availBuildingAreaEnter)
                    std::cout << "- area of building (>=" << areaMinSize << ", <=" << availLandArea << ")\n";
                if (availFloorsEnter)
                    std::cout << "- count of floors (" << floorsMin << "..." << floorsMax << ")\n";
                if (availChimneyEnter)
                    std::cout << "- chimney (yes - if presents)\n";
                std::cout << "(over space):\n";
                userStream = getUserInputString();
                if (availBuildingAreaEnter)
                    userStream >> presBuilding.area;
                if (availFloorsEnter)
                    userStream >> countOfFloors;
                if (availChimneyEnter) {
                    std::string chimneyState;
                    userStream >> chimneyState;
                    presBuilding.chimney = chimneyState == "yes";
                }
                doubleRef_Round(presBuilding.area);
                outSplitter();

                // Check data
                if (availBuildingAreaEnter
                    && (presBuilding.area < areaMinSize - doublePrecision
                        || presBuilding.area > availLandArea + doublePrecision))
                    std::cout << "Wrong building area!\n";
                else if (availFloorsEnter
                         && (countOfFloors < floorsMin
                             || countOfFloors > floorsMax))
                    std::cout << "Wrong count of floors!\n";
                else break;
                outSplitter(); // Wrong data

            }

        }

        // Fix available area
        landAreaRemain -= presBuilding.area;

        // Enter data of floors
        if (availFloorsEnter) {

            presBuilding.floors.resize(countOfFloors);
            enterFloorsData(presBuilding);

        }

    }

}

void enterLandsData(std::vector<landDescription>& lands) {

    for (landIndex = 0; landIndex < lands.size(); ++landIndex) {

        // Lands data
        std::stringstream userStream;
        landDescription& presLand = lands[landIndex];
        int countOfBuildings = 0;

        while (true) {

            // Get data
            outLandDataIdsPos(landIndex);
            std::cout << "Enter description of land\n";
            std::cout << "- unique number of land (>0)\n";
            std::cout << "- count of buildings (>0)\n";
            std::cout << "- area of land (>=" << areaMinSize << " per bld., frac. valid)\n";
            std::cout << "(over space):\n";
            userStream = getUserInputString();
            userStream >> presLand.number >> countOfBuildings >> presLand.area;
            doubleRef_Round(presLand.area);
            outSplitter();

            // Check data
            bool numberIsUnique = true;
            for (int prevLandIndex = 0; prevLandIndex < landIndex; ++prevLandIndex)
                if (lands[prevLandIndex].number == presLand.number) {
                    numberIsUnique = false;
                    break;
                }
            if (!numberIsUnique)
                std::cout << "The number is not unique!\n";
            else if (presLand.number <= 0)
                std::cout << "Wrong number of land!\n";
            else if (countOfBuildings <= 0)
                std::cout << "Wrong count of buildings!\n";
            else if (presLand.area < areaMinSize * countOfBuildings - doublePrecision)
                std::cout << "Wrong lands area!\n";
            else break;
            outSplitter(); // Wrong data

        }

        // Enter data of builds
        presLand.buildings.resize(countOfBuildings);
        enterBuildingsData(presLand);

        // Out land data
        std::cout << "Entered data of land:\n";
        outSplitter('=');
        outLandData(presLand);
        outSplitter('=');

    }

}

int main() {

    // Formatted
    std::cout.setf(std::ios::fixed);
    std::cout.precision(0);
    double presRound = doubleRoundPrecision;
    while (presRound < 1 - doublePrecision) {
        presRound *= 10;
        std::cout.precision(std::cout.precision() + 1);
    }

    outHeader();

    std::stringstream userStream;

    // Enter count of land
    int countOfLands = 0;
    while(true) {

        // Get data
        std::cout << "Enter count of lands (>0):\n";
        userStream = getUserInputString();
        userStream >> countOfLands;
        outSplitter();

        // Check data
        if (countOfLands <= 0)
            std::cout << "Wrong count of lands!\n";
        else break;
        outSplitter(); // Wrong data

    }

    // Lands iterations
    std::vector<landDescription> lands(countOfLands);
    enterLandsData(lands);

    // Pause
    std::cout << "Enter something for result:\n";
    getUserInputString();

    // Out result data
    outSplitter('#');
    std::cout << "Result data of village:\n";
    outSplitter('=');
    for (landIndex = 0; landIndex < lands.size(); ++landIndex) {
        outLandData(lands[landIndex]);
        outSplitter('=');
    }

    return 0;

}
