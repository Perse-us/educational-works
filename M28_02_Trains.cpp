#include <iostream>
#include <thread>
#include <mutex>

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

const int trainCount = 3;
std::mutex out_lock;
std::mutex station_lock;

void train(char name, int travelTime) {

    const char* departWord = "depart";
    std::string trainId = "Train \"";
    trainId.append(1, name);
    trainId.append(1, '\"');

    // Start out
    out_lock.lock();
    std::cout << trainId << " start to Station\n";
    out_lock.unlock();

    // Moved
    std::this_thread::sleep_for(std::chrono::seconds(travelTime));

    // Approach
    if (!station_lock.try_lock()) {
        out_lock.lock();
        std::cout << trainId << " waiting arriving to Station!\n";
        out_lock.unlock();
        station_lock.lock();
    }

    // Arrive
    out_lock.lock();
    std::cout << trainId << " arrived to Station\n";
    std::cout << "Enter \"" << departWord << "\" for depart\n";
    out_lock.unlock();

    // Wait for depart
    while (true) {

        std::string userInput;
        Interface::refreshInStream();
        std::getline(std::cin, userInput);

        if (userInput == departWord) break;

        out_lock.lock();
        std::cout << "Wrong command! Repeat, please...\n";
        out_lock.unlock();

    }

    // Depart
    out_lock.lock();
    std::cout << trainId << " departed from Station\n";
    out_lock.unlock();
    station_lock.unlock();

}

int main() {

    Interface::outHeader("Trains in move");

    // Initialisation
    const char trainNames[trainCount] = {'A', 'B', 'C'};
    std::thread trains[trainCount];

    // Enter start data
    out_lock.lock();
    for (int trainIndex = 0; trainIndex < trainCount; ++trainIndex) {
        char trainName = trainNames[trainIndex];

        std::cout << "Enter time in the way for train \""
            << trainName << "\":\n";

        int travelTime;
        Interface::refreshInStream();
        std::cin >> travelTime;
        if (std::cin.fail() || travelTime <= 0) {
            std::cout << "Wrong travel time! Repeat, please...\n";
            trainIndex--;
        } else trains[trainIndex] = std::thread(train, trainName, travelTime);

        Interface::outSplitter();

    }

    // Start travel
    std::cout << "Start travels\n";
    Interface::outSplitter();
    out_lock.unlock();

    // Join thread
    std::thread* trains_Pnt = std::end(trains);
    while (--trains_Pnt >= trains)
        if (trains_Pnt->joinable()) trains_Pnt->join();

    // Finish work
    Interface::outSplitter();
    std::cout << "Every train was departed!\n";

    return 0;

}
