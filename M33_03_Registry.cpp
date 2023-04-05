#include <iostream>
#include <vector>
#include <sstream>

// Interface class
class Interface {

public:

    static const int sizeOfSplitter = 30;

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

// User commands class
class UserCmd {

public:

    enum CmdList{
        non = -1, add, del, find, print, finish
    };

private:

    struct userCmdWrite{
        const char* name;
        const char* description;
    };

    static constexpr userCmdWrite userCmdList[(int) finish + 1] {
            "add", " key value] - Add value by key",
            "del", " key] - Delete write(-s) by key",
            "find", " key] - Find write(s) by key",
            "print", "] - Print storage content",
            "exit", "] - Finish work"
    };

public:

    static void outUserCmdList() {

        std::cout << "Available commands:\n";
        const userCmdWrite* cmd_Pnt = userCmdList - 1;
        while (++cmd_Pnt < std::end(userCmdList))
            std::cout << " - [" << cmd_Pnt->name << cmd_Pnt->description << std::endl;

    }

    static CmdList getCmdByName(const std::string& cmdName) {

        const userCmdWrite* cmd_Pnt = userCmdList - 1;
        while (++cmd_Pnt < std::end(userCmdList))
            if (cmd_Pnt->name == cmdName) return (CmdList) (cmd_Pnt - userCmdList);

        return UserCmd::non;

    }

};

template<typename T1, typename T2>
class Storage {

    struct Pair {
        T1 key;
        T2 value;
        Pair(T1 key, T2 value) : key(key), value(value) {};
    };
    std::vector<Pair> store;

    void outPair(const Pair& pair) const {
        std::cout << "Key: " << pair.key << "; Value: " << pair.value;
    }

public:

    void add(const T1& key, const T2& value) {

        store.emplace_back(key, value);

        std::cout << "Added: ";
        outPair(*(--store.cend()));
        std::cout << std::endl;

    }

    void del(const T1& key) {

        bool success = false;
        typename std::vector<Pair>::iterator it
            = store.begin();
        while (it != store.end()) {
            if (it->key == key) {
                std::cout << "Remove: ";
                outPair(*it);
                std::cout << std::endl;
                store.erase(it);
                success = true;
            } else it++;
        }
        if (!success) std::cout << "Key: " << key << " - is not found!\n";

    }

    void find(const T1& key) const {

        bool success = false;
        for (const Pair& el : store)
            if (el.key == key) {
                std::cout << "Find: ";
                outPair(el);
                std::cout << std::endl;
                success = true;
            }

        if (!success) std::cout << "Key: " << key << " - is not found!\n";

    }

    void print() const {

        if (store.empty()) std::cout << "Storage empty\n";
        else {

            std::cout << "Storage content:\n";
            for (const Pair& el : store) {
                std::cout << "- ";
                outPair(el);
                std::cout << std::endl;
            }

        }

    }

};

template<typename T1, typename T2>
void workBlock(Storage<T1, T2>& storage) {

    UserCmd::outUserCmdList();
    Interface::outSplitter();

    while (true) {

        std::string userInput, parCmd;
        T1 par1;
        T2 par2;

        // Get user input
        std::cout << "Enter command:\n";
        Interface::refreshInStream();
        std::getline(std::cin, userInput);
        Interface::outSplitter();

        // Split command and parameters
        std::stringstream userStream(userInput);
        userStream >> parCmd;

        // Get command and parameters
        UserCmd::CmdList userCmd = UserCmd::getCmdByName(parCmd);
        if (userCmd == UserCmd::add) userStream >> par1 >> par2;
        else if (userCmd == UserCmd::find || userCmd == UserCmd::del) userStream >> par1;

        if (userStream.fail()) {
            std::cout << "Wrong entered data!\n";
            Interface::outSplitter();
            UserCmd::outUserCmdList();
        }
        else if (userCmd == UserCmd::finish) break;
        else if (userCmd == UserCmd::add) storage.add(par1, par2);
        else if (userCmd == UserCmd::del) storage.del(par1);
        else if (userCmd == UserCmd::find) storage.find(par1);
        else if (userCmd == UserCmd::print) storage.print();
        else {
            std::cout << "Wrong command!\n";
            Interface::outSplitter();
            UserCmd::outUserCmdList();
        }

        Interface::outSplitter();

    }

    std::cout << "Finish work\n";

}

int main() {

    //Storage<int, int> storage;
    //Storage<int, double> storage;
    //Storage<int, std::string> storage;
    //Storage<double, int> storage;
    //Storage<double, double> storage;
    //Storage<double, std::string> storage;
    //Storage<std::string, int> storage;
    //Storage<std::string, double> storage;
    Storage<std::string, std::string> storage;

    Interface::outHeader("Storage");

    workBlock(storage);

    return 0;

}
