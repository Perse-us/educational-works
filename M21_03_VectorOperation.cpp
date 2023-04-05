#include <iostream>
#include <sstream>
#include <cmath>

void outSplitter (char sign = '-') {
    std::string splitter;
    splitter.assign(25, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "Vector (operations)" << std::endl;
    outSplitter('=');
}

// Global initialization
const int countOfArgs = 2;

// Commands describe
struct userCmdDescribe {
    const char* name = "";
    const char* description = "";
};

enum userCmdList {
    cmd_add, cmd_sub, cmd_scale, cmd_len, cmd_norm, cmd_finish
};

const int cmdCount = 1 + (int) cmd_finish;
userCmdDescribe cmdDescription[cmdCount] = {
        "add", "Addition of vectors",
        "sub", "Subtraction of vectors",
        "scale", "Multiplying a vector by a scalar",
        "len", "Get vector length",
        "norm", "Normalization of the vector",
        "finish", "Finish work"};

struct argumentData {
    double x_1 = 0;
    double y_1 = 0;
    double x_2 = 0;
    double y_2 = 0;
};

std::stringstream getUserStream() {

    std::string userInput;
    std::getline(std::cin, userInput);

    return std::stringstream(userInput);

};

argumentData getDataFromUser() {

    argumentData arg;

    std::stringstream userStream = getUserStream();
    userStream >> arg.x_1 >> arg.y_1 >> arg.x_2 >> arg.y_2;

    return arg;

}

void getArgumentsFromUser(argumentData args[], bool argsIsVec[], int args_count = countOfArgs) {

    for (int i = 0; i < args_count; ++i) {
        if(args_count > 1) std::cout << "Argument " << i + 1 << std::endl;
        std::cout << "Enter "
            << (argsIsVec[i]
                ? "coordinates of vector\n(x1 y1 x2 y2) (over space)"
                : "value of scalar")
            << ":\n";
        args[i] = getDataFromUser();
    }

}

double vectorLength(argumentData& arg) {
    return std::sqrt(std::pow(arg.x_2 - arg.x_1, 2)
        + std::pow(arg.y_2 - arg.y_1, 2));
}

// Return true if success operation
bool operationByArguments(userCmdList vecOperation) {

    bool successOperation = true;

    argumentData args[countOfArgs];
    bool argsIsVec[countOfArgs];
    for (int i = 0; i < countOfArgs; ++i)
        argsIsVec[i] = true;

    argumentData resultVec;
    double lengthVec = 0;

    if (vecOperation == cmd_add) {
        getArgumentsFromUser(args, argsIsVec);
        resultVec.x_1 = args[0].x_1;
        resultVec.y_1 = args[0].y_1;
        resultVec.x_2 = args[1].x_2;
        resultVec.y_2 = args[1].y_2;
    } else if (vecOperation == cmd_sub) {
        getArgumentsFromUser(args, argsIsVec);
        resultVec.x_1 = args[1].x_2;
        resultVec.y_1 = args[1].y_2;
        resultVec.x_2 = args[0].x_1;
        resultVec.y_2 = args[0].y_1;
    } else if (vecOperation == cmd_scale
            || vecOperation == cmd_norm) {
        double multiplierVec;
        if (vecOperation == cmd_scale) {
            argsIsVec[1] = false;
            getArgumentsFromUser(args, argsIsVec);
            multiplierVec = args[1].x_1;
        } else {
            getArgumentsFromUser(args, argsIsVec, 1);
            multiplierVec = 1 / vectorLength(args[0]);
        }
        resultVec.x_1 = args[0].x_1;
        resultVec.y_1 = args[0].y_1;
        resultVec.x_2 = (args[0].x_2 - args[0].x_1) * multiplierVec
                + resultVec.x_1;
        resultVec.y_2 = (args[0].y_2 - args[0].y_1) * multiplierVec
                + resultVec.y_1;
    } else if (vecOperation == cmd_len) {
        getArgumentsFromUser(args, argsIsVec, 1);
        lengthVec = vectorLength(args[0]);
    } else successOperation = false;

    // Out result
    if (successOperation) {
        outSplitter();
        if (vecOperation == cmd_len)
            std::cout << "Vector length: " << lengthVec << std::endl;
        else {
            std::cout << "Result vector (x1; y1 : x2; y2):\n";
            std::cout << resultVec.x_1 << "; " << resultVec.y_1
                      << " : " << resultVec.x_2 << "; " << resultVec.y_2 << std::endl;
            std::cout << "Length: " << vectorLength(resultVec) << std::endl;
        }
    }

    return successOperation;

}

int main() {

    // Formatted
    std::cout.precision(14);

    outHeader();

    while(true) {

        // Initialization
        std::stringstream userStream;

        // Command description
        std::cout << "Available command:\n";
        for (int i = 0; i < cmdCount; ++i)
            std::cout << "\"" << cmdDescription[i].name << "\" - " << cmdDescription[i].description << "\n";
        outSplitter('=');
        std::cout << "Enter command:\n";

        // Get command
        std::string cmdUserText;
        userStream = getUserStream();
        userStream >> cmdUserText;

        // Ident command
        int cmdIndex = 0;
        while (cmdUserText != cmdDescription[cmdIndex].name
                && cmdIndex++ < cmdCount);
        outSplitter();

        if (cmdIndex < cmdCount) {

            // Use command
            auto cmdUserCommand = (userCmdList) cmdIndex;
            std::cout << cmdDescription[cmdIndex].description << std::endl;
            outSplitter();
            if (cmdUserCommand == cmd_finish) break;
            else if (!operationByArguments(cmdUserCommand))
                std::cerr << "Wrong operation!\n";

        } else std::cout << "Unknown command!\n";

        outSplitter('=');

    }

    return 0;

}
