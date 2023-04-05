#include <iostream>
#include <vector>

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
        non = -1, add, call, sms, list, exit
    };

private:

    struct userCmdWrite{
        const char* name;
        const char* description;
    };

    static constexpr userCmdWrite userCmdList[(int) exit + 1] {
            "add", "Add contact",
            "call", "Send call",
            "sms", "Send SMS",
            "list", "Contacts list",
            "exit", "Turn off"
    };

public:

    static void outUserCmdList() {

        std::cout << "Available commands:\n";
        const userCmdWrite* cmd_Pnt = userCmdList - 1;
        while (++cmd_Pnt < std::end(userCmdList))
            std::cout << " - \"" << cmd_Pnt->name << "\" - "
                      << cmd_Pnt->description << std::endl;

    }

    static CmdList getCmdByName(const std::string& cmdName) {

        const userCmdWrite* cmd_Pnt = userCmdList - 1;
        while (++cmd_Pnt < std::end(userCmdList))
            if (cmd_Pnt->name == cmdName) return (CmdList) (cmd_Pnt - userCmdList);

        return UserCmd::non;

    }

};

// Phone number class
// Input number format:  +70000000000
// Output number format: +7 (000) 000-00-00
class PhoneNumber{

    static const int numDiv = 10;
    static const long long nonNum = -1;
    static const int numLength = 10;

    static long long getNumberByString(std::string& inStr) {

        // Prefix and length check
        std::string pref = "+7";
        if (inStr.length() != numLength + pref.length()
            || inStr.substr(0, pref.length()) != pref)
                return nonNum;

        // Get number
        long long result = 0;
        std::string::const_iterator inStr_Pnt = --inStr.cbegin() + (int) pref.length();
        while (++inStr_Pnt < inStr.cend()) {

            if(*inStr_Pnt < '0' || *inStr_Pnt > '9')
                return nonNum;

            result = (*inStr_Pnt - '0') + result * 10;

        }

        return result;

    }

    static std::string getFormatNumber(long long workNumber) {

        std::string result;
        if (workNumber == nonNum) return result;

        for (int numPos = 0; numPos < numLength; ++numPos) {

            result.insert(0, 1, (char) (workNumber % numDiv + '0'));
            workNumber /= numDiv;

            if (numPos == 1 || numPos == 3) result.insert(0,  "-");
            else if (numPos == 6) result.insert(0, ") ");
            else if (numPos == 9) result.insert(0, "+7 (");

        }

        return result;

    }

    long long number = nonNum;

public:

    // Return true if number was set
    bool changeNumber(std::string& numStr) {

        long long result = getNumberByString(numStr);
        bool correctNum = result != nonNum;
        if (correctNum) number = result;

/*
        std::cout << "Current in number:\n";
        std::cout << number << std::endl;
        std::cout << getNumber() << std::endl;
*/

        return correctNum;

    }

    // Get number in comfortable format
    std::string getNumber() {

        return getFormatNumber(number);

    }

    // Check equal of entered number
    bool isThisNumber(std::string& numStr) {
        return number != nonNum
            && number == getNumberByString(numStr);
    }

};

// Contact class
struct Contact {
    std::string name;
    PhoneNumber number;
};

// Phone class
class Phone {

    enum CallType{
        Call, SMS
    };

    static void outContactDescription(Contact* addBook_Write) {
        if (addBook_Write == nullptr) return;
        std::cout << " - \"" << addBook_Write->name << "\"\n";
        std::cout << "   " << addBook_Write->number.getNumber() << std::endl;
    }

    std::vector<Contact*> addressBook;

    void sendMaker(CallType callType) {

        std::string userInput;

        std::cout << "Enter subscriber's name or phone:\n";
        std::getline(std::cin, userInput);

        if (!userInput.empty()) {

            Interface::outSplitter();

            // Find subscriber
            std::vector<Contact*>::const_iterator addBook_Pnt
                = --addressBook.cbegin();
            while (++addBook_Pnt < addressBook.cend()
                && (*addBook_Pnt)->name != userInput
                && !(*addBook_Pnt)->number.isThisNumber(userInput));

            // Sender
            if (addBook_Pnt < addressBook.cend()) {

                // Get SMS text
                std::string smsText;
                bool isSms = callType == SMS;
                if (isSms) {
                    std::cout << "Enter SMS text:\n";
                    std::getline(std::cin, smsText);
                }

                // Send
                std::cout << "Send " << (isSms ? "SMS" : "call") << "...\n";
                outContactDescription(*addBook_Pnt);
                if (isSms) std::cout << "With text:\n   " << smsText << std::endl;

            } else std::cout << "Write with \"" << userInput << "\" is not found!\n";

        }

    }

public:

    void addContact() {

        std::string name, number;

        // Get name
        std::cout << "Enter name of contact:\n";
        std::getline(std::cin, name);
        if (name.empty()) return;
        Interface::outSplitter();

        while(true) {

            // Get number
            std::cout << "Enter phone number (+7<10 digits>):\n";
            std::getline(std::cin, number);
            if (number.empty()) return;
            Interface::outSplitter();

            // Add contact
            Contact* contact = new Contact;
            contact->name = name;
            if (contact->number.changeNumber(number)) {
                addressBook.push_back(contact);
                std::cout << "Add contact:\n";
                outContactDescription(contact);
                break;
            }

            // Repeat phone ask
            delete contact;
            std::cout << "Wrong phone number! Repeat please\n";
            Interface::outSplitter();

        }

    }

    void listContacts() {

        if (addressBook.empty())
            std::cout << "There are not contacts in phone!\n";

        else {

            std::cout << "Address book list:\n";
            std::vector<Contact*>::const_iterator addBook_Pnt
                = --addressBook.cbegin();
            while(++addBook_Pnt < addressBook.cend())
                outContactDescription(*addBook_Pnt);

        }

    }

    void deleteContacts() {

        while(!addressBook.empty()) {

            Contact* contact = *--addressBook.end();
            delete contact;
            addressBook.pop_back();

        }

    }

    void sendCall() {
        sendMaker(Call);
    }

    void sendSms() {
        sendMaker(SMS);
    }

};

int main() {

    Interface::outHeader("Mobile phone");

    Phone* phone = new Phone;

    while (true) {

        // Out command list
        UserCmd::outUserCmdList();
        Interface::outSplitter();

        // Get command
        std::string userInput;
        std::cout << "Enter command:\n";
        std::getline(std::cin, userInput);
        UserCmd::CmdList userCmd = UserCmd::getCmdByName(userInput);
        Interface::outSplitter();

        // Select command
        if (userCmd == UserCmd::add) phone->addContact();
        else if (userCmd == UserCmd::call) phone->sendCall();
        else if (userCmd == UserCmd::sms) phone->sendSms();
        else if (userCmd == UserCmd::list) phone->listContacts();
        else if (userCmd == UserCmd::exit) break;
        else std::cout << "Wrong command!\n";

        Interface::outSplitter();

    }

    phone->deleteContacts();
    std::cout << "Contact list is cleared!\n";

    delete phone;
    std::cout << "Turned off!\n";

    return 0;

}
