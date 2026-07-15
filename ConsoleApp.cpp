#include "ConsoleApp.h"

#include <iostream>

ConsoleApp::ConsoleApp(SampleDataSource& samples, OrderDataSource& orders,
    ProductionQueueDataSource& productionQueue)
    : samples_(samples)
    , orders_(orders)
    , productionQueue_(productionQueue)
{
}

void ConsoleApp::run()
{
    bool keepRunning = true;
    while (keepRunning)
    {
        printMenu();

        std::string choice;
        if (!readChoice(choice))
        {
            break;
        }

        keepRunning = handleChoice(choice);
    }
}

void ConsoleApp::printMenu() const
{
    std::cout << "\n=== DataMonitor ===\n";
    std::cout << "0. 종료\n";
    std::cout << "선택: ";
}

bool ConsoleApp::readChoice(std::string& choice) const
{
    return static_cast<bool>(std::getline(std::cin, choice));
}

bool ConsoleApp::handleChoice(const std::string& choice)
{
    if (choice == "0")
    {
        return false;
    }

    std::cout << "알 수 없는 메뉴입니다: " << choice << "\n";
    return true;
}
