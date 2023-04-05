#include <iostream>
#include <vector>

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

class Person {

public:

    static int getRndVal(int minVal, int maxVal) {
        return minVal + rand() % (++maxVal - minVal);
    }

    static std::string getPluralS(int value) {
        return (value == 1 ? "" : "s");
    }

    static int askSlavesCount(const char* inviteText) {

        int result = 0;

        // Enter count of slaves...
        while(true) {

            std::cout << inviteText << " (>0):\n";
            Interface::refreshInStream();
            std::cin >> result;
            Interface::outSplitter();

            if (!std::cin.fail() && result > 0) break;
            std::cout << "Wrong enter! Repeat, please...\n";
            Interface::outSplitter();

        }

        return result;

    }

    const char* busyWorkerReport = "\"Every worker is busy!\"";

    // Subordinates
    Person* director = nullptr; // nullptr - Head of The Company
    int slavesCount = 0;
    Person** slaves = nullptr;

    // Description
    std::string name;
    int orderNum = 0;

    std::string getFormatName(const std::string* currName = nullptr) {
        return '\"' + (currName == nullptr ? name : *currName) + '\"';
    }

    std::string getAutoPostfix() {

        std::string result;

        Person* currNumerator = this;
        while(currNumerator != nullptr) {
            if (currNumerator->orderNum > 0)
                result.insert(0, ' ' + std::to_string(currNumerator->orderNum));
            currNumerator = (Person*) currNumerator->director;
        }

        return result;

    }

    void outHired() {
        std::cout << "Hired " << getFormatName() << std::endl;
    }

    void fire() {

        // Fired slaves
        if (slaves != nullptr) {

            Person** slave_Ptr = slaves;
            while (slave_Ptr < slaves + slavesCount) {
                if ((*slave_Ptr)->slavesCount > 0)
                    (*slave_Ptr)->fire();
                delete (*slave_Ptr++); // Slave
            }

        }

    }

};

class Worker: private Person {

public:

    enum TaskType {
        non = -1, A, B, C // top - general task
    };
    static constexpr char* taskTypeNames[(int) C  + 1] = {"A", "B", "C"};

    // Tasks (easy decision)
    TaskType task = non;

    Worker(class Person* director, int orderNum) {

        this->director = director;
        this->orderNum = orderNum;

        name = "Worker" + getAutoPostfix();
        outHired();

    }

};

class Manager: private Person {

public:

    Manager(class Person* director, int orderNum) {

        this->director = director;
        this->orderNum = orderNum;

        name = "Manager" + getAutoPostfix();
        outHired();

        // Create Workers
        std::string inviteText = "Enter count of workers in command "
                + (orderNum == 0 ? "" : std::to_string(orderNum));
        slavesCount = askSlavesCount(inviteText.c_str());
        if (slavesCount > 0) {

            slaves = new Person*[slavesCount];

            for (int slaveIndex = 0; slaveIndex < slavesCount; ++slaveIndex)
                slaves[slaveIndex] = (Person*) new Worker(this, slaveIndex + 1);

            Interface::outSplitter();

        }

    }

    // true if free workers are
    bool startTask(int taskId) {

        bool isFreeWorker = false;

        // Delegation
        std::cout << getFormatName() << std::endl;
        std::cout << "- got task #" << taskId << std::endl;

        // Find free worker
        int freeWorkers = 0;
        bool hasSlaves = slaves != nullptr;
        Worker** slaves_Pnt = (Worker**) slaves;
        Worker** slaves_End = slaves_Pnt + slavesCount;
        if (hasSlaves) {
            slaves_Pnt--;
            while(++slaves_Pnt < slaves_End && (*slaves_Pnt)->task != Worker::non);
            freeWorkers = (int) (slaves_End - slaves_Pnt);

        }

        // Check worker
        if (freeWorkers > 0) {

            std::cout << "- have " << freeWorkers
                      << " free worker" << getPluralS(freeWorkers) << std::endl;

            // Randomizer init
            int rndSeed = taskId + orderNum;
            srand(rndSeed);
            std::cout << "- think about task by random seed "
                      << rndSeed << std::endl;

            // Get sub task
            int subTaskCount = getRndVal(1, freeWorkers);
            std::cout << "- thought out " << subTaskCount
                      << " subtask" << getPluralS(subTaskCount) << std::endl;

            // Delegate subtasks
            if (hasSlaves) {

                while (subTaskCount-- > 0) {

                    (*slaves_Pnt)->task = (Worker::TaskType) getRndVal(
                            (int) Worker::A, (int) Worker::C);
                    std::cout << "- set subtask type \""
                              << Worker::taskTypeNames[(*slaves_Pnt)->task] << "\" to "
                              << ((Person*) *slaves_Pnt)->getFormatName() << std::endl;

                    freeWorkers--;
                    slaves_Pnt++;

                }

            }

            isFreeWorker = freeWorkers > 0;

            if (!isFreeWorker) std::cout << "- report: " << busyWorkerReport << "\n";

        } else std::cout << "- haven't free workers for delegate task!\n";

        return isFreeWorker;

    }

};

class HeadOfCompany: private Person {

public:

    HeadOfCompany() {

        name = "Head of company";
        outHired();

        // Create Workers
        slavesCount = askSlavesCount("Enter count of commands");
        if (slavesCount > 0) {

            slaves = new Person*[slavesCount];

            for (int slaveIndex = 0; slaveIndex < slavesCount; ++slaveIndex)
                slaves[slaveIndex] = (Person*) new Manager(this, slaveIndex + 1);

        }

    }

    // true if free workers are
    bool startTask(int taskId) {

        bool isFreeWorker = false;

        std::cout << getFormatName() << std::endl;
        std::cout << "- set task #" << taskId
                  << " for every manager" << std::endl;

        // Just delegate
        if (slaves != nullptr) {
            Person** slaves_Pnt = slaves;
            while (slaves_Pnt < slaves + slavesCount)
                isFreeWorker = ((Manager*) *slaves_Pnt++)->startTask(taskId) || isFreeWorker;
        }

        // Worker control
        if (!isFreeWorker) {
            std::cout << getFormatName() << std::endl;
            std::cout << "- got report: " << busyWorkerReport << std::endl;
            std::cout << "  from every manager!\n";
        }

        return isFreeWorker;

    }

    void fire() {
        Person::fire();
    }

};

int main() {

    Interface::outHeader("The Company");

    HeadOfCompany* company = new HeadOfCompany;
    std::cout << "The Company created!\n";
    Interface::outSplitter();

    // Management (we always have free staff with start)
    int taskId = 0;
    while (true) {

        // Get task ID
        std::cout << "Enter task ID\n";
        std::cout << "(integer value more then 0):\n";
        Interface::refreshInStream();
        std::cin >> taskId;
        Interface::outSplitter();

        // Wrong enter
        if (std::cin.fail() || taskId <= 0) {
            std::cout << "Wrong enter! Repeat, please...\n";
        } else if (!company->startTask(taskId)) break;
        Interface::outSplitter();

    }
    Interface::outSplitter();

    // Destruction objects
    std::cout << "All tasks are completed...\n";
    company->fire();
    std::cout << "Staff are fired...\n";
    delete company;
    std::cout << "The Company is disbanded...\n";

    return 0;

}
