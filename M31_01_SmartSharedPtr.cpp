#include <iostream>
#include <memory>

// Interface class
class Interface {

public:

    static const int sizeOfSplitter = 40;

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

// Classes
class Toy {

    std::string name;

public:

    Toy(const char* name) : name(name) {};
    Toy() = default;

    const std::string& getName() { return name; }

};

class shared_ptr_toy {

    Toy* toyObj = nullptr;
    long* toyCnt = nullptr;

    void setByRef(const shared_ptr_toy& srcObj) {
        toyObj = srcObj.toyObj;
        toyCnt = srcObj.toyCnt;
        if (toyCnt != nullptr) (*toyCnt)++;
    }

    void removeInstance() {

        if(toyObj == nullptr && toyCnt == nullptr) return;

        if (toyCnt == nullptr || --(*toyCnt) <= 0) {

            // Control Toy-object
            if (toyObj != nullptr)
                std::cout << "- Sim: Removing object Toy: \"" << toyObj->getName() << "\"...\n";
            else std::cout << "- Sim: Wrong object Toy for remove!\n";

            // Remove
            delete toyObj;
            delete toyCnt;

        }

        // Clear used
        toyObj = nullptr;
        toyCnt = nullptr;

    }

public:

    // Constructor
    shared_ptr_toy(Toy* toy) : toyObj(toy), toyCnt(new long(1)) {};
    // Constructor by ref
    shared_ptr_toy(shared_ptr_toy& srcObj) { setByRef(srcObj); }
    // Set constructor
    shared_ptr_toy& operator=(shared_ptr_toy& srcObj) {

        // It's me
        if (this == &srcObj) return *this;

        // Delete current use
        removeInstance();
        // Change object
        setByRef(srcObj);

        return *this;

    }

    Toy* get() { return toyObj; }

    long use_count() { return (toyCnt == nullptr ? 0 : *toyCnt); }

    void reset() { removeInstance(); }

    ~shared_ptr_toy() { removeInstance(); }

};

class Cat {

    shared_ptr_toy myToy = nullptr;

public:

    Cat(shared_ptr_toy myToy) : myToy(myToy) {};

};

// Object makers
shared_ptr_toy make_shared_toy(const char* name) {
    shared_ptr_toy newObj(new Toy(name));
    return newObj;
}

shared_ptr_toy make_shared_toy(Toy& toy) {
    shared_ptr_toy newObj(new Toy(toy));
    return newObj;
}

shared_ptr_toy make_shared_toy(Toy* toy) {
    shared_ptr_toy newObj(new Toy(*toy));
    return newObj;
}

int main() {

    Interface::outHeader("Custom shared pointer");

    // Pre-initialization
    Toy* to_1 = new Toy("Rope");
    Toy to_2("Ray");

    {
        // Standard set
        std::cout << "Standard behaviour...\n";
        Interface::outSplitter();
        //
        std::shared_ptr<Toy> t_0_1(std::make_shared<Toy>("Ball"));
        std::shared_ptr<Toy> t_0_2(std::make_shared<Toy>("Ring"));
        std::shared_ptr<Toy> t_0_1_check(t_0_1);
        std::cout << "Before set,\nBall: " << t_0_1.use_count() << "; Ring: " << t_0_2.use_count()
                  << "; Copy Ball: " << t_0_1_check.use_count() << std::endl;
        t_0_1 = t_0_2;
        Interface::outSplitter();
        std::cout << "After set,\nCopy Ring: " << t_0_1.use_count() << "; Ring: " << t_0_2.use_count()
                  << "; Copy Ball: " << t_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        std::shared_ptr<Toy> t_0_3(t_0_1);
        std::cout << "After create,\nCopy Ring: " << t_0_1.use_count() << "; Ring: " << t_0_2.use_count()
                  << "; Copy Copy Ring: " << t_0_3.use_count() << "; Copy Ball: " << t_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        t_0_1.reset();
        std::cout << "After reset,\nReset Copy Ring: " << t_0_1.use_count() << "; Ring: " << t_0_2.use_count()
                  << "; Copy Copy Ring: " << t_0_3.use_count() << "; Copy Ball: " << t_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        t_0_1.reset();
        std::cout << "After repeated reset,\nReset Copy Ring: " << t_0_1.use_count() << "; Ring: " << t_0_2.use_count()
                  << "; Copy Copy Ring: " << t_0_3.use_count() << "; Copy Ball: " << t_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        // Release "Ball"
        t_0_1_check = t_0_1;
        t_0_1_check = t_0_2;
        std::cout << "After release \"Ball\",\nReset Copy Ring: " << t_0_1.use_count() << "; Ring: " << t_0_2.use_count()
                  << "; Copy Copy Ring: " << t_0_3.use_count() << "; Copy Ball: " << t_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        // Check get
        std::cout << "Check get: Toy \"" << t_0_1_check.get()->getName() << "\"\n";
        Interface::outSplitter();

        // "Rope"s
        std::shared_ptr<Toy> t_1_1(std::make_shared<Toy>(*to_1));
        std::cout << "Shared 1 for same pointer: " << t_1_1.use_count() << std::endl;
        std::shared_ptr<Toy> t_1_2(std::make_shared<Toy>(*to_1));
        std::cout << "Shared 2 for same pointer: " << t_1_2.use_count() << std::endl;
        std::cout << "Used different controlled objects set\n";
        Interface::outSplitter();

        // "Ray"s;
        std::shared_ptr<Toy> t_2_1(std::make_shared<Toy>(to_2));
        std::cout << "Shared 1 for same reference: " << t_2_1.use_count() << std::endl;
        std::shared_ptr<Toy> t_2_2(std::make_shared<Toy>(to_2));
        std::cout << "Shared 2 for same reference: " << t_2_2.use_count() << std::endl;
        std::cout << "Used different controlled objects set\n";
    }

    Interface::outSplitter('=');

    {

        // Simulation set
        std::cout << "Simulate behaviour...\n";
        Interface::outSplitter();
        //
        shared_ptr_toy lt_0_1(make_shared_toy("Ball"));
        shared_ptr_toy lt_0_2(make_shared_toy("Ring"));
        shared_ptr_toy lt_0_1_check(lt_0_1);
        std::cout << "Before set,\nBall: " << lt_0_1.use_count() << "; Ring: " << lt_0_2.use_count()
                  << "; Copy Ball: " << lt_0_1_check.use_count() << std::endl;
        lt_0_1 = lt_0_2;
        Interface::outSplitter();
        std::cout << "After set,\nCopy Ring: " << lt_0_1.use_count() << "; Ring: " << lt_0_2.use_count()
                  << "; Copy Ball: " << lt_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        shared_ptr_toy lt_0_3(lt_0_1);
        std::cout << "After create,\nCopy Ring: " << lt_0_1.use_count() << "; Ring: " << lt_0_2.use_count()
                  << "; Copy Copy Ring: " << lt_0_3.use_count() << "; Copy Ball: " << lt_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        lt_0_1.reset();
        std::cout << "After reset,\nReset Copy Ring: " << lt_0_1.use_count() << "; Ring: " << lt_0_2.use_count()
                  << "; Copy Copy Ring: " << lt_0_3.use_count() << "; Copy Ball: " << lt_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        lt_0_1.reset();
        std::cout << "After repeated reset,\nReset Copy Ring: " << lt_0_1.use_count() << "; Ring: " << lt_0_2.use_count()
                  << "; Copy Copy Ring: " << lt_0_3.use_count() << "; Copy Ball: " << lt_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        // Release "Ball"
        lt_0_1_check = lt_0_1;
        lt_0_1_check = lt_0_2;
        std::cout << "After release \"Ball\",\nReset Copy Ring: " << lt_0_1.use_count() << "; Ring: " << lt_0_2.use_count()
                  << "; Copy Copy Ring: " << lt_0_3.use_count() << "; Copy Ball: " << lt_0_1_check.use_count() << std::endl;
        Interface::outSplitter();
        // Check get
        std::cout << "Check get: Toy \"" << lt_0_1_check.get()->getName() << "\"\n";
        Interface::outSplitter();
        // Reference create / delete
        shared_ptr_toy newBall = make_shared_toy("New Ball");
        shared_ptr_toy* lt_0_5 = new shared_ptr_toy(newBall);
        std::cout << "New Balls (by ref): " << lt_0_5->use_count() << std::endl;
        shared_ptr_toy* lt_0_6 = new shared_ptr_toy(*lt_0_5);
        std::cout << "New Balls (by ref): " << lt_0_5->use_count() << " / "
            << lt_0_6->use_count() << std::endl;
        Cat* cat = new Cat(newBall);
        std::cout << "New Balls (with owner): " << lt_0_5->use_count() << " / "
                  << lt_0_6->use_count() << std::endl;
        std::cout << "Release new balls by delete reference...\n";
        newBall.reset();
        std::cout << "New Balls (without share ptr): " << lt_0_5->use_count() << " / "
                  << lt_0_6->use_count() << std::endl;
        delete cat;
        std::cout << "New Balls (deleted owner): " << lt_0_5->use_count() << " / "
                  << lt_0_6->use_count() << std::endl;
        delete lt_0_5;
        std::cout << "New Balls (by ref): " << lt_0_6->use_count() << std::endl;
        delete lt_0_6;
        Interface::outSplitter();

        // "Rope"s
        shared_ptr_toy lt_1_1(make_shared_toy(to_1));
        std::cout << "Shared 1 for same pointer: " << lt_1_1.use_count() << std::endl;
        shared_ptr_toy lt_1_2(make_shared_toy(to_1));
        std::cout << "Shared 2 for same pointer: " << lt_1_2.use_count() << std::endl;
        std::cout << "Used different controlled objects set\n";
        Interface::outSplitter();

        // "Ray"s;
        shared_ptr_toy lt_2_1(make_shared_toy(to_2));
        std::cout << "Shared 1 for same reference: " << lt_2_1.use_count() << std::endl;
        shared_ptr_toy lt_2_2(make_shared_toy(to_2));
        std::cout << "Shared 2 for same reference: " << lt_2_2.use_count() << std::endl;
        std::cout << "Used different controlled objects set\n";
        Interface::outSplitter();

        std::cout << "Auto-destructors...\n";
    }

    Interface::outSplitter('=');

    delete to_1;

    std::cout << "Finish work\n";

    return 0;

}
