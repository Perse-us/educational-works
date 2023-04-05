#include <iostream>
#include <vector>
#include <cassert>

// Interface class
class Interface {

public:

    static const int sizeOfSplitter = 20;

    static void outSplitter (char sign = '-') {

        std::string splitter;
        splitter.assign(sizeOfSplitter, sign);
        std::cout << splitter << std::endl;

    }

    static void outHeader(const char* headerText) {

        outSplitter('=');
        std::cout << headerText << std::endl;
        outSplitter('=');

    }

    // Stream block
    static void refreshInStream() {
        std::cin.clear();
        std::cin.seekg(0);
    }

};

class Branch {

    // Initialization
    const static int treesCount = 5;
    const char* nonOwnerName = "None";
    enum Level {
        forest, tree, bigBranch, midBranch
    };

    // Population
    int population = 0;
    bool isHouse = false;
    std::string owner; // empty() - if none

    // Branch hierarchy
    Branch* parent = nullptr; // nullptr - is forest
    int childrenCount = 0;
    Branch** children = nullptr;

    static int getRndVal(int minVal, int maxVal) {
        return minVal + rand() % (maxVal - minVal + 1);
    }

    static std::string getOutShift(Level currLevel) {

        std::string result;
        int steps = (int) currLevel;

        while(steps-- > 0) result.append("  ");

        return result;

    }

    static Branch* getNeighbourBranch(Branch* currBranch) {

        Level currLevel = currBranch->getCurrLevel();
        Branch* resultBranch = currBranch;

        while (currLevel > bigBranch) {
            currLevel = (Level) ((int) currLevel - 1);
            resultBranch = resultBranch->parent;
        }

        return resultBranch;

    }

    std::string formatOwnerName(std::string& ownerName) {
        return (ownerName.empty() ? "<" + (std::string) nonOwnerName + ">" : '\"' + ownerName + '\"');
    }

    Level getCurrLevel() {

        int level = 0;
        Branch* currParent = parent;

        while (currParent != nullptr) {
            level++;
            currParent = currParent->parent;
        };

        return (Level) level;

    }

    // Out branch ID
    std::string getChildId(Level currLevel, int curIndex) {

        std::string result = ' ' + std::to_string(curIndex + 1)
                + " from " + std::to_string(childrenCount);

        if (currLevel == forest) return "Tree" + result;
        else if (currLevel == tree) return "Big branch" + result;
        else if (currLevel == bigBranch) return "Middle branch" + result;

        return "";

    }

public:

    // Constructor
    Branch(Branch* parent = nullptr, bool isHouse = false):
        parent(parent), isHouse(isHouse) {

        Level currLevel = getCurrLevel();
        bool itIsHouse = true;
        if (currLevel == forest) {
            // Trees in the forest
            childrenCount = treesCount; // Trees
            itIsHouse = false;
        } else if (currLevel == tree)
            childrenCount = getRndVal(3, 5); // Big branches
        else if (currLevel == bigBranch)
            childrenCount = getRndVal(2, 3); // Middle branches

        // Init children branches
        if (childrenCount > 0) {
            // Grow new branches
            children = new Branch*[childrenCount];
            Branch** children_Pnt = children;
            while (children_Pnt < children + childrenCount)
                *children_Pnt++ = new Branch(this, itIsHouse);
        }

    }

    // Population process
    void populate(bool autoPopulate, int* totalNum = nullptr, Branch* uniqZone = nullptr) {

        // Parameters initialization
        int statTotalNum = 0;
        if (totalNum == nullptr) totalNum = &statTotalNum;
        if (uniqZone == nullptr) {
            uniqZone = this;
            while (uniqZone->parent != nullptr)
                uniqZone = uniqZone->parent;
        }

        // Initialization
        Level currLevel = getCurrLevel();
        std::string shift = getOutShift(currLevel);

        if (currLevel == forest) {
            std::cout << "Populating the Forest...\n";
            if (autoPopulate) std::cout << "Auto-population...\n";
        }

        // Populate current branch
        if (isHouse) {

            std::string ownerName;

            if (autoPopulate) {

                // Get auto settler
                if (getRndVal(0, 3) == 0)
                    ownerName = nonOwnerName;
                else ownerName = "Mer " + std::to_string(++*totalNum);

            } else
                // Get manual settler
                while (true) {

                    std::cout << shift << "Enter owner's name\n";
                    std::cout << shift << "(\"" << nonOwnerName << "\" - if none):\n";
                    std::getline(std::cin, ownerName);

                    if (ownerName.empty()) std::cout << "Empty owner's name!";
                    else if (uniqZone->getBranchByOwner(ownerName) != nullptr)
                        std::cout << "Owner " << formatOwnerName(ownerName)
                            << " is already used!";
                    else break;

                    std::cout << std::endl << "Repeat, please!\n";

                }

            // Correction owner's name
            if (ownerName == nonOwnerName) ownerName.clear();

            // Populate
            setOwner(ownerName);
            if (!autoPopulate && !owner.empty())
                std::cout << formatOwnerName(ownerName) << " populated\n";

        }

        // Populate children branches
        if (children != nullptr)
            for (int childIndex = 0; childIndex < childrenCount; ++childIndex) {
                if (!autoPopulate)
                    std::cout << shift << getChildId(currLevel, childIndex) << std::endl;
                children[childIndex]->populate(autoPopulate, totalNum, uniqZone);
            }

    }

    std::string getOwner() {
        return owner;
    }

    void setOwner(std::string& newOwner) {

        if (isHouse) {

            // Get population change
            int changePop = 0;
            if (owner.empty() != newOwner.empty()) {
                if (newOwner.empty()) changePop--;
                else changePop++;
            }

            // Recursive change population
            owner = newOwner;
            if (changePop != 0) {
                Branch* currBranch = this;
                while(currBranch != nullptr) {
                    currBranch->population += changePop;
                    currBranch = currBranch->parent;
                }
            }

        } else std::cout << "There is not house on this branch!\n";

    }

    // Get population
    void outPopulationList() {

        // Initialisation
        Level currLevel = getCurrLevel();
        std::string currShift = getOutShift(currLevel);

        // Forest
        if (currLevel == forest) std::cout << currShift << "The Forest\n";

        // Out population info
        std::cout << currShift << "Population: " << population << std::endl;
        if (isHouse || !owner.empty())  std::cout << currShift
            << "Owner: " << formatOwnerName(owner) << std::endl;

        // Children branches population
        if (children != nullptr) {

            for (int branchIndex = 0; branchIndex < childrenCount; ++branchIndex) {

                // Branch ID
                std::cout << currShift << getChildId(currLevel, branchIndex) << std::endl;

                // Children branches info
                children[branchIndex]->outPopulationList();

            }

        }

    }

    Branch* getBranchByOwner(std::string& ownerName) {

        if (!ownerName.empty()) {
            if (isHouse && owner == ownerName) return this;
            else if (children != nullptr)
                for (int childIndex = 0; childIndex < childrenCount; ++childIndex) {
                    Branch* branch = children[childIndex]->getBranchByOwner(ownerName);
                    if (branch != nullptr) return branch;
                }

        }

        return nullptr;

    }

    // Find count of neighbours by owner's name
    // Settlers are unique
    void getNeighborsByOwner(std::string& ownerName) {

        Branch* branch = getBranchByOwner(ownerName);
        if (ownerName.empty()) std::cout << "Wrong name!\n";
        else {
            std::cout << "Owner " << formatOwnerName(ownerName);
            if (branch == nullptr) std::cout << " is not found!\n";
            else std::cout << " has neighbours: "
                << getNeighbourBranch(branch)->population - 1 << std::endl;
        }

    }

    // Destructor
    void delBranches() {

        if (children == nullptr) return;

        for (int childIndex = 0; childIndex < childrenCount; ++childIndex) {
            Branch* branch = children[childIndex];
            branch->delBranches();
            delete branch;
            children[childIndex] = nullptr;
        }

    }

};

int main() {

    const char* autoPopWord = "yes";
    const char* stopWord = "#exit";
    srand(time(nullptr));
    std::string userInput;

    // Create Forest
    Branch* forest = new Branch;

    // Population Forest
    Interface::outHeader("Bosmer's village");
    std::cout << "Would you like auto populate the Forest\n";
    std::cout << "(recommended: \"" << autoPopWord << "\" :) )?:\n";
    std::getline(std::cin, userInput);
    Interface::outSplitter();
    forest->populate(userInput == autoPopWord);
    Interface::outSplitter();

    std::cout << "Forest population...\n";
    Interface::outSplitter();
    forest->outPopulationList();
    Interface::outSplitter();

    while (true) {

        std::cout << "Enter owner's name for neighbour found\n";
        std::cout << "(\"" << stopWord << "\" - for finish):\n";
        std::getline(std::cin, userInput);
        Interface::outSplitter();
        if (userInput == stopWord) break;

        forest->getNeighborsByOwner(userInput);
        Interface::outSplitter();

    }

    forest->delBranches();
    std::cout << "Trees are disappear!\n";
    delete forest;
    std::cout << "Forest is vanished!\n";

    return 0;

}
