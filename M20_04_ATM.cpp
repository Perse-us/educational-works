#include <vector>
#include <iostream>
#include <ctime>
#include <fstream>

void outSplitter (char sign = '-') {
    std::string splitter;
    splitter.assign(25, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "ATM" << std::endl;
    outSplitter('=');
}

// Banknotes set
enum class banknotes {
    Empty, Cash100, Cash200, Cash500, Cash1000, Cash2000, Cash5000
};

banknotes maxBanknote() {
    return banknotes::Cash5000;
}

banknotes previousBanknote(banknotes presBanknote) {
    return presBanknote > banknotes::Empty
        ? (banknotes)((int)presBanknote - 1)
        : banknotes::Empty;
}

int valueOfBanknote(banknotes banknote) {

    if (banknote == banknotes::Cash100) return 100;
    else if (banknote == banknotes::Cash200) return 200;
    else if (banknote == banknotes::Cash500) return 500;
    else if (banknote == banknotes::Cash1000) return 1000;
    else if (banknote == banknotes::Cash2000) return 2000;
    else if (banknote == banknotes::Cash5000) return 5000;
    else return 0;

}

void correctionBanknoteValue(banknotes& value) {
    if (value < banknotes::Empty || value > maxBanknote())
        value = banknotes::Empty;
}

// Initialization
const std::string fileName = "cash_box_content.bin";
const int banknotesVariants = (int)maxBanknote() + 1;
const int cashBoxSize = 1000;
//
banknotes cashBox[cashBoxSize];
long long totalAmount = 0;

int totalBanknotes(const int banknotesCount[]) {

    int result = 0;

    for (int i = 1; i < banknotesVariants; ++i)
        result += banknotesCount[i];

    return result;

}

void outCashBoxContent() {

    long long totalSum = 0;

    for (int i = 0; i < cashBoxSize; ++i) {

        int presValue = valueOfBanknote(cashBox[i]);
        totalSum += presValue;

        std::cout << "Box ";
        std::cout.width(5);
        std::cout << i << " : ";
        std::cout.width(5);
        std::cout << presValue << std::endl;

    }

    outSplitter();
    std::cout << "Total sum:    " << totalSum << std::endl;
    std::cout << "Total amount: " << totalAmount << std::endl;

}

void outCashBoxBanknotesState(const int banknotesCount[]) {

    if (totalAmount > 0) {

        int totalCount = 0;
        long long totalSum = 0;

        for (int i = 1; i < banknotesVariants; ++i) {

            int presValue = valueOfBanknote((banknotes)i);
            int presCount = banknotesCount[i];
            long long presSum = presCount * presValue;

            std::cout << "Banknote ";
            std::cout.width(4);
            std::cout << presValue << ":";

            totalCount += presCount;
            totalSum += presSum;

            std::cout << " Count: ";
            std::cout.width(5);
            std::cout << presCount << " Sum: " << presSum;
            std::cout << std::endl;

        }

        outSplitter();
        if (totalSum == totalAmount)
            std::cout << "Total sum: " << totalSum << " in " << totalCount << " boxes" << std::endl;
        else std::cout << "Wrong total sum!\n";

    } else std::cout << "ATM is empty!\n";

}

void changeBanknoteCount(banknotes changingBanknote,
                         int banknotesCount[],
                         bool add = true) {

    // Empty slots are not counting
    if (changingBanknote == banknotes::Empty) return;

    int banknoteValue = valueOfBanknote(changingBanknote);
    int banknoteIndex = (int)changingBanknote;

    if (add) {
        banknotesCount[banknoteIndex]++;
        totalAmount += banknoteValue;
    } else {
        banknotesCount[banknoteIndex]--;
        totalAmount -= banknoteValue;
    }

}

// Filling cash box
// Return true if cash box changed
bool fillCashBox(int banknotesCount[]) {

    bool changed = false;

    if (totalBanknotes(banknotesCount) < cashBoxSize) {

        for (int i = 0; i < cashBoxSize; ++i)
            if (cashBox[i] == banknotes::Empty) {
                cashBox[i] = (banknotes)(rand() % (banknotesVariants - 1) + 1);
                changeBanknoteCount(cashBox[i], banknotesCount);
                changed = true;
            }

    }

    return changed;

}

void saveCashBox() {

    std::ofstream cashBox_WriteStream(fileName, std::ios::binary);

    if (cashBox_WriteStream.is_open()) {

        banknotes* presBanknote = cashBox;
        int banknotesCountDown = cashBoxSize;

        while (banknotesCountDown-- > 0) {
            cashBox_WriteStream.write((char*)presBanknote, sizeof(*presBanknote));
            presBanknote++;
        }

        cashBox_WriteStream.close();

    } else std::cout << "File \n" << fileName << "\n is not opened!\n";

}

// Read cash box from the file
void readCashBox(int banknotesCount[]) {

    std::ifstream cashBox_ReadStream(fileName, std::ios::binary);

    if (cashBox_ReadStream.is_open()) {

        int banknotesLoadCounter = cashBoxSize;
        banknotes* presBanknote = cashBox;

        while (!cashBox_ReadStream.eof() && banknotesLoadCounter-- > 0) {

            // Read banknote
            cashBox_ReadStream.read((char*)presBanknote, sizeof(*presBanknote));
            correctionBanknoteValue(*presBanknote);

            // Fill counters and sums
            changeBanknoteCount(*presBanknote, banknotesCount);

            // Next banknote
            presBanknote++;

        };

        cashBox_ReadStream.close();

    }

}

// Collect banknotes for payment
// Return true if success collected
bool getDepositSet(long long sumOfPayment,
                   std::vector<banknotes>& deposit,
                   int banknotesCount[]) {

    if (sumOfPayment == 0) return true; // Complete deposit
    if (sumOfPayment < 0) return false; // Wrong deposit

    for(banknotes presBanknote = maxBanknote();
            presBanknote > banknotes::Empty;
            presBanknote = previousBanknote(presBanknote)) {
        if (banknotesCount[(int)presBanknote] > 0) {
            int presValue = valueOfBanknote(presBanknote);

            // Add banknotes to deposit
            deposit.push_back(presBanknote);
            changeBanknoteCount(presBanknote, banknotesCount, false);

            // Next iteration
            if(getDepositSet(sumOfPayment - presValue,
                             deposit, banknotesCount))
                return true; // Return completed deposit

            // Return banknote from wrong deposit
            deposit.pop_back();
            changeBanknoteCount(presBanknote, banknotesCount);

        }

    }

    // Deposit is not completed
    return false;

}

void giveOutDeposit(std::vector<banknotes>& deposit) {

    // Success deposit - prepare payment cash
    int repayBanknotes[banknotesVariants];
    for (int i = 0; i < banknotesVariants; ++i)
        repayBanknotes[i] = 0;

    // Pik up banknotes
    for (int i = 0; i < deposit.size(); ++i) {
        banknotes presBanknote = deposit[i];
        banknotes* cashBox_Pnt = cashBox + cashBoxSize - 1;
        while (cashBox_Pnt >= cashBox) {
            if (*cashBox_Pnt == presBanknote) {
                *cashBox_Pnt = banknotes::Empty;
                repayBanknotes[(int)presBanknote]++;
                break;
            }
            cashBox_Pnt--;
        }
    }

    // Out cash
    long long paymentSum = 0;
    std::cout << "Giving out process:\n";
    for (int i = banknotesVariants - 1; i >= 0; --i) {
        if (repayBanknotes[i] > 0) {
            int presValue = valueOfBanknote((banknotes)i);
            std::cout << "Banknotes ";
            std::cout.width(5);
            std::cout << presValue << " - " << repayBanknotes[i] << std::endl;
            paymentSum += presValue * repayBanknotes[i];
        }
    }
    outSplitter();
    std::cout << "Gave out total sum: " << paymentSum << std::endl;

}

int main() {

    bool debug = false;

    // Initialization
    std::srand(std::time(nullptr)); // Init randomizer by time
    int banknotesCount[banknotesVariants];

    // Starting fill
    for (int i = 0; i < banknotesVariants; ++i)
        banknotesCount[i] = 0;
    for (int i = 0; i < cashBoxSize; ++i)
        cashBox[i] = banknotes::Empty;

    outHeader();

    // Reading cash box
    readCashBox(banknotesCount);
    std::cout << "Cash box is unlocked\n";
    outSplitter();

    if (debug) {
        // Debug
        outCashBoxContent();
        outSplitter();
    }

    // Out ATM state
    outCashBoxBanknotesState(banknotesCount);
    outSplitter();

    while (true) {

        // User input
        std::string userInput;
        std::cout << "Operations: + - Fill ATM; - Give out: e - Exit\n";
        std::cout << "Enter operation:";
        std::getline(std::cin, userInput);
        outSplitter();

        // Select operation code
        char operation = userInput[0];
        if (operation == 'e') break;
        else if (operation == '+') {

            if(fillCashBox(banknotesCount))
                std::cout << "Cash box is filled!\n";
            else std::cout << "There are not free cells in cash box!\n";

        } else if (operation == '-') {

            if (totalAmount > 0) {

                // Out available banknotes
                std::cout << "Available banknotes:\n";
                bool firstBanknote = true;
                for (int i = 1; i < banknotesVariants; ++i) {
                    if (banknotesCount[i] > 0) {
                        int presValueOfBanknote = valueOfBanknote((banknotes)i);
                        if (firstBanknote) firstBanknote = false;
                        else std::cout << ", ";
                        std::cout << presValueOfBanknote;
                    }
                }
                std::cout << std::endl;
                outSplitter();

                // Get desire sum
                int minAvailableValue = 100;
                long long sumOfPayment = 0;
                std::cout << "Enter desire sum (multiple " << minAvailableValue << "):\n";
                std::getline(std::cin, userInput);
                sumOfPayment = atoi(userInput.data());
                outSplitter();

                // Check and payment
                if (sumOfPayment <= 0 || sumOfPayment % minAvailableValue != 0)
                    std::cout << "Wrong asked sum!\n";
                else if (sumOfPayment > totalAmount)
                    std::cout << "Sum is too large for giving out!\n";
                else {

                    // Get deposit set
                    std::vector<banknotes> deposit;
                    if (getDepositSet(sumOfPayment, deposit, banknotesCount)
                        && !deposit.empty()) {

                        // Success deposit - payment cash
                        giveOutDeposit(deposit);

                    } else std::cout << "Sorry!!! Can not possible to give out asked sum!\n";

                }

            } else std::cout << "There are not banknotes to give out!\n";

        } else std::cout << "Wrong operation!\n";

        if (debug) {
            // Debug
            outSplitter();
            outCashBoxContent();
        }

        // Out ATM state
        outSplitter();
        outCashBoxBanknotesState(banknotesCount);
        outSplitter();

    }

    // Save changes
    saveCashBox();
    std::cout << "Work out. Cash box is locked\n";

    return 0;

}
