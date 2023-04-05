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

class Talent {
public:
    virtual std::string getType() = 0; // "Pure" virtual function
    // Default virtual destructor (not possible "pure" - compile error!!!)
    virtual ~Talent() = default;
};

class TalentDancing : virtual public Talent {
public:
    std::string getType() {
        return "Dancing";
    }
};

class TalentCounting : virtual public Talent {
public:
    std::string getType() {
        return "Counting";
    }
};

class TalentSwimming : virtual public Talent {
public:
    std::string getType() {
        return "Swimming";
    }
};

class Animal {

    std::vector<Talent*> talents;

    bool isNewTalent(Talent* newTalent) {

        if (newTalent == nullptr) return false;

        std::string newTalentType = newTalent->getType();
        for(Talent* currTalent : talents)
            if (currTalent->getType() == newTalentType) return false;

        return true;

    }

    virtual std::string animalType(bool defArticle) = 0;

protected:

    std::string name;

public:

    virtual void voice() = 0;

    void addTalent(Talent* newTalent) {

        if (newTalent == nullptr) return;

        if (isNewTalent(newTalent)) talents.push_back(newTalent);
        else {
            delete newTalent;
            newTalent = nullptr;
        }

    }

    void showTalents() {

        std::cout << "This is " << animalType(false) << " named \"" << name << "\"" << std::endl;
        std::cout << "And it can";
        if (talents.empty()) {
            std::cout << "... nothing" << std::endl;
            std::cout << "It's just " << animalType(true) << std::endl;
        } else {
            std::cout << ":" << std::endl;
            for (Talent* currTalent : talents)
                std::cout << "- " << currTalent->getType() << std::endl;
        }

    }

    // Custom virtual destructor
    virtual ~Animal() {
        // Remove talents of animal
        for (Talent* talent : talents)
            delete talent;
        talents.clear();
    };

};

class Cat : virtual public Animal {
protected:

    Cat() = default;

public:

    Cat(const std::string& name) {
        this->name = name;
    }

    void voice() {
        std::cout << "Meow!\n";
    }

    virtual std::string animalType(bool defArticle) {
        std::string result = (defArticle ? "the" : "a");
        return result.append(" cat");
    };

};

class Dog : virtual public Animal {
protected:

    Dog() = default;

public:

    Dog(const std::string& name) {
        this->name = name;
    }

    void voice() {
        std::cout << "Bark!\n";
    }

    virtual std::string animalType(bool defArticle) {
        std::string result = (defArticle ? "the" : "a");
        return result.append(" dog");
    };

};

class CatDog : public Cat, public Dog {
protected:

    CatDog() = default;

public:

    CatDog(const std::string& name) {
        this->name = name;
    }

    void voice() {
        std::cout << "Meow-Bark!\n";
    }

    virtual std::string animalType(bool defArticle) {
        std::string result = (defArticle ? "the" : "a");
        return result.append(" cat-dog");
    };

};

int main() {

    Interface::outHeader("Talented animals");

    Animal* animal = nullptr;

    animal = new Cat("Mila");
    animal->addTalent(new TalentCounting);
    animal->addTalent(new TalentSwimming);
    animal->addTalent(new TalentDancing);
    animal->addTalent(new TalentCounting);
    animal->addTalent(new TalentSwimming);
    animal->addTalent(new TalentDancing);
    animal->showTalents();
    animal->voice();
    delete animal;
    animal = nullptr;

    Interface::outSplitter();
    animal = new Dog("Friend");
    animal->addTalent(new TalentDancing);
    animal->addTalent(new TalentSwimming);
    animal->showTalents();
    animal->voice();
    delete animal;
    animal = nullptr;

    Interface::outSplitter();
    animal = new Cat("Tom");
    animal->addTalent(new TalentCounting);
    animal->addTalent(new TalentDancing);
    animal->showTalents();
    animal->voice();
    delete animal;
    animal = nullptr;

    Interface::outSplitter();
    animal = new Dog("Wolf");
    animal->addTalent(new TalentCounting);
    animal->showTalents();
    animal->voice();
    delete animal;
    animal = nullptr;

    Interface::outSplitter();
    animal = new CatDog("What is it!?");
    animal->showTalents();
    animal->voice();
    delete animal;
    animal = nullptr;

    return 0;

}
