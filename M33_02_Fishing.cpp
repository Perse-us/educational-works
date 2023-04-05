#include <iostream>
#include <exception>

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

class Pond {
public:

    const static int fieldSize = 9;
    const static int fishCount = 1;
    const static int bootCount = 3;

    class CaughtFish : public std::exception {
    public:
        const char * what() const noexcept override {
            return "You caught the Fish! Congratulation!!!";
        }
    };
    class CaughtBoot : public std::exception {
    public:
        const char * what() const noexcept override {
            return "You caught the Boot!";
        }
    };

private:

    enum Content {
        empty, fish, boot
    };

    Content field[fieldSize] = {empty};

    void rndGenerate(int count, Content content) {
        while (count-- > 0) {
            int pos = rand() % fieldSize;
            if (field[pos] == empty) field[pos] = content;
            else ++count;
        }
    }

public:

    Pond () {
        rndGenerate(fishCount, fish);
        rndGenerate(bootCount, boot);
    }

    void catchFish(int cell) {
        if (--cell < 0 || cell >= fieldSize) throw std::invalid_argument("Wrong cell number!");
        if (field[cell] == fish) throw CaughtFish();
        if (field[cell] == boot) throw CaughtBoot();
    }

    std::string getPond() {
        std::string result;
        for(const Content content : field)
            result.append((content == fish ? "F " : (content == boot ? "B " : "x ")));
        return result;
    }

};

int main() {

    // Initialisation
    std::srand(time(nullptr));
    Pond pond;

    Interface::outHeader("Fishing");

    // Cheat mode
    std::cout << "Pond state:\n";
    std::cout << pond.getPond() << std::endl;
    Interface::outSplitter();

    while (true) {

        // User input
        int userIn;
        std::cout << "Enter target cell (1..." << Pond::fieldSize << ")\n";
        Interface::refreshInStream();
        std::cin >> userIn;

        try {
            if (std::cin.fail()) throw std::invalid_argument("Wrong input!");
            pond.catchFish(userIn);
            std::cout << "You caught nothing...\n";
        } catch (const std::invalid_argument& er) {
            // Wrong cell
            std::cerr << er.what() << std::endl;
        }
        catch (const std::exception& res) {
            // Catch final result (simple version)
            std::cout << res.what() << std::endl;
            break;
        }
/*
        // If need different caught
        catch (const Pond::CaughtFish& res) {
            std::cout << res.what() << std::endl;
            break;
        } catch (const Pond::CaughtBoot& res) {
            std::cout << res.what() << std::endl;
            break;
        }
*/

        Interface::outSplitter();

    }
    Interface::outSplitter();
    std::cout << "Game over\n";

    return 0;

}
