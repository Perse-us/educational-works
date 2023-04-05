#include <iostream>
#include <vector>
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

// Initialization
enum Dish {
    pizza, soup, steak, salad, sushi
};
const char* getDishName(const Dish& dish) {
    if (dish == pizza) return "\"Pizza\"";
    else if (dish == soup) return "\"Soup\"";
    else if (dish == steak) return "\"Steak\"";
    else if (dish == salad) return "\"Salad\"";
    else if (dish == sushi) return "\"Sushi\"";
    else return "\"<unknown>\"";
}

const int minOrderTime_Sec = 5;
const int maxOrderTime_Sec = 10;
const int minCookTime_Sec = 5;
const int maxCookTime_Sec = 15;
const int courierTime_Sec = 30;
const int deliveryCount = 10;
bool stopWork = false;

std::mutex work_lock; // One mutex enough here
std::vector<Dish> kitchenList;
//std::mutex kitchen_lock;
std::vector<Dish> deliveryList;
//std::mutex delivery_lock;

int getRndVal(int minVal, int maxVal) {
    return minVal + rand() % (++maxVal - minVal);
}

void orders() {

    // Initialisation
    std::string staffName = "Call-center: ";

    work_lock.lock();
    srand(std::time(nullptr));
    std::cout << staffName << "in progress...\n";
    work_lock.unlock();

    while (true) {

        // Wait order
        work_lock.lock();
        bool finish = stopWork;
        int orderWait_Sec = getRndVal(minOrderTime_Sec, maxOrderTime_Sec);
        work_lock.unlock();
        //
        if (finish) break;
        std::this_thread::sleep_for(std::chrono::seconds(orderWait_Sec));

        // Get order (to begin of list)
        work_lock.lock();
        Dish dish = (Dish) getRndVal(pizza, sushi);
        std::cout << staffName << getDishName(dish) << " order received\n";
        kitchenList.insert(kitchenList.begin(), dish);
        work_lock.unlock();

    }

}

void kitchen() {

    // Initialisation
    std::string staffName = "Kitchen: ";

    work_lock.lock();
    srand(std::time(nullptr));
    std::cout << staffName << "is waiting for orders...\n";
    work_lock.unlock();

    while (true) {

        // Check order
        work_lock.lock();
        bool finish = stopWork;
        bool haveOrder = !kitchenList.empty();
        work_lock.unlock();

        if (finish) break;
        if (haveOrder) {

            // Cooking
            work_lock.lock();
            Dish dish = *--kitchenList.cend();
            kitchenList.pop_back();
            int cookWait_Sec = getRndVal(minCookTime_Sec, maxCookTime_Sec);
            std::cout << staffName << getDishName(dish) << " start cooking. Ready in "
                << cookWait_Sec << " sec\n";
            work_lock.unlock();
            //
            std::this_thread::sleep_for(std::chrono::seconds(cookWait_Sec));

            // Issue
            work_lock.lock();
            deliveryList.push_back(dish);
            std::cout << staffName << getDishName(dish) << " ready! Wait for delivery\n";
            work_lock.unlock();

        }

    }

}

void delivery() {

    // Initialisation
    std::string staffName = "Courier: ";

    work_lock.lock();
    int dlv_CountDown = deliveryCount;
    int dlv_Period = courierTime_Sec;
    std::cout << staffName << "was woken up!..\n";
    work_lock.unlock();

    while (dlv_CountDown > 0) {

        // Delivery remain report
        work_lock.lock();
        std::cout << staffName << "Remain: " << dlv_CountDown << " order(s)\n";
        work_lock.unlock();

        // Wait courier
        std::this_thread::sleep_for(std::chrono::seconds(dlv_Period));

        // Delivery
        work_lock.lock();
        std::cout << staffName;
        if (deliveryList.empty())
            std::cout << "There are not orders for delivery!\n";
        else {
            std::cout << "Delivered:\n";
            std::vector<Dish>::const_iterator deliveryList_Pnt = --deliveryList.cbegin();
            while (++deliveryList_Pnt < deliveryList.cend()) {
                std::cout << "- " << getDishName(*deliveryList_Pnt) << std::endl;
                dlv_CountDown--;
            }
            deliveryList.clear();
        }
        work_lock.unlock();

    }

    work_lock.lock();
    Interface::outSplitter();
    std::cout << staffName << "The delivery plan is completed\n";
    stopWork = true;
    work_lock.unlock();

}

int main() {

    Interface::outHeader("Kitchen");

    std::thread orders_work(orders);
    std::thread kitchen_work(kitchen);
    std::thread delivery_work(delivery);

    if (delivery_work.joinable()) delivery_work.join();
    kitchen_work.detach();
    orders_work.detach();

    //std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;

}
