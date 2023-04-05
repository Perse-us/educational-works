#include <iostream>

// Saving calculated ways for acceleration of the code
// Vector is more optimal for dynamic size equal size of the stairs but another libraries aren't let :)
// Array size = 50 - enough for minJump = 1,
// for minJumps > 1 should use more size of memory for comfortable execution time
const int jumpsMemorySize = 50;
int jumpsMemory[jumpsMemorySize];
const int jumps_EmptyValue = -1;

void outSplitter (char sign = '-') {
    std::string splitter;
    splitter.assign(25, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "Rabbit & stairs" << std::endl;
    outSplitter('=');
}

bool isValidMemorySteps(const int& steps) {
    return steps >= 0 && steps < jumpsMemorySize;
}

// I don't know how, but it works... :))
// Just I found regularity: quantity ways are sum of ways for every size of the first jump
// And it looks like I completed and 3rd task too... completely by accident :)
int countRabbitWays(const int& steps,
                    bool& overflow,
                    const int& maxJump = 3,
                    const int& minJump = 1) {

    // Wrong values
    if (minJump <= 0 || maxJump < minJump || steps <= 0) return 0;

    // Check saved memory
    if (isValidMemorySteps(steps)) {
        int savedJumps = jumpsMemory[steps];
        if (savedJumps != jumps_EmptyValue) return savedJumps;
    }

    int result = 0;

    for (int presNum = (maxJump < steps ? maxJump : steps); presNum >= minJump; --presNum) {
        int prevResult = result;
        if (steps == presNum) result++; // One final jump
        else {
            int addWays = countRabbitWays(steps - presNum, overflow, maxJump, minJump);
            if (overflow) {
                result = addWays;
                break;
            } else result += addWays;
        }
        // Easy check overflow
        if (result < 0) {
            overflow = true;
            result = prevResult;
            break;
        }
    }

    // Save memory
    if (isValidMemorySteps(steps)) jumpsMemory[steps] = result;

    return result;

}

int main() {

    // Initialization
    int* jumpsMemoryPointer = jumpsMemory;
    while (jumpsMemoryPointer - jumpsMemory < jumpsMemorySize)
        *jumpsMemoryPointer++ = jumps_EmptyValue;

    outHeader();

    for (int i = -1; i <= 50; ++i) {
        bool overflow = false;
        int countOfWays = countRabbitWays(i, overflow);
        std::cout << "Steps: " << i << " - Ways: ";
        if (overflow) std::cout << "more then ";
        std::cout << countOfWays << std::endl;
    }

    return 0;

}
