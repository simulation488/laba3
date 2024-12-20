#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <limits>

class IResident {
public:
    virtual double getTotalCost() const = 0;
    virtual const std::string& getName() const = 0;
    virtual ~IResident() {}
};

class ResidentWithBenefits : public IResident {
private:
    std::string name;
    std::map<std::string, double> services;

public:
    ResidentWithBenefits(const std::string& name) : name(name) {}

    void addService(const std::string& serviceType, double amount) {
        if (amount < 0) {
            throw std::invalid_argument("Ошибка: количество услуги не может быть отрицательным.");
        }
        if (services[serviceType] + amount > 100) {
            throw std::invalid_argument("Ошибка: количество услуги не может превышать 100.");
        }
        services[serviceType] += amount;
    }

    double getTotalCost() const override {
        double totalCost = 0.0;
        for (const auto& service : services) {
            totalCost += service.second * 0.8; // 20% скидка
        }
        return totalCost;
    }

    const std::string& getName() const override { return name; }
};

class ResidentWithoutBenefits : public IResident {
private:
    std::string name;
    std::map<std::string, double> services;

public:
    ResidentWithoutBenefits(const std::string& name) : name(name) {}

    void addService(const std::string& serviceType, double amount) {
        if (amount < 0) {
            throw std::invalid_argument("Ошибка: количество услуги не может быть отрицательным.");
        }
        if (services[serviceType] + amount > 100) {
            throw std::invalid_argument("Ошибка: количество услуги не может превышать 100.");
        }
        services[serviceType] += amount;
    }

    double getTotalCost() const override {
        double totalCost = 0.0;
        for (const auto& service : services) {
            totalCost += service.second;
        }
        return totalCost;
    }

    const std::string& getName() const override { return name; }
};

class HousingManagementSystem {
private:
    std::vector<std::unique_ptr<IResident>> residents;

public:
    void addResident(IResident* resident) {
        if (resident == nullptr) {
            throw std::invalid_argument("Ошибка: нельзя добавить пустого жильца.");
        }
        residents.emplace_back(resident);
    }

    double calculateTotalCosts() const {
        double total = 0.0;
        for (const auto& resident : residents) {
            total += resident->getTotalCost();
        }
        return total;
    }

    IResident* findResident(const std::string& name) {
        for (const auto& resident : residents) {
            if (resident->getName() == name) {
                return resident.get();
            }
        }
        return nullptr;
    }
};

void addResidentToSystem(HousingManagementSystem& system) {
    std::string name;
    char type;

    std::cout << "Введите имя жильца: ";
    std::cin >> name;

    while (true) {
        std::cout << "Введите тип жильца (w - с льготами, n - без льгот): ";
        std::cin >> type;

        if (type == 'w' || type == 'n') {
            break; // Корректный ввод, выходим из цикла
        }
        else {
            std::cerr << "Ошибка: некорректный ввод. Используйте 'w' или 'n'." << std::endl;
        }
    }

    try {
        if (type == 'w') {
            system.addResident(new ResidentWithBenefits(name));
        }
        else {
            system.addResident(new ResidentWithoutBenefits(name));
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void addServiceToResident(HousingManagementSystem& system) {
    std::string name;
    std::string serviceType;
    double amount;

    std::cout << "Введите имя жильца, которому хотите добавить услугу: ";
    std::cin >> name;

    // Предложение доступных услуг
    std::cout << "Доступные услуги:" << std::endl;
    std::cout << "1. Услуга A" << std::endl;
    std::cout << "2. Услуга B" << std::endl;
    std::cout << "3. Услуга C" << std::endl;

    std::cout << "Введите тип услуги (A, B, C): ";
    std::cin >> serviceType;

    // Приведение типа услуги к нижнему регистру для удобства
    for (auto& c : serviceType) c = tolower(c);

    std::cout << "Введите количество услуги: ";
    while (!(std::cin >> amount) || amount < 0) {
        std::cerr << "Ошибка: введите корректное количество услуги (положительное число)." << std::endl;
        std::cin.clear(); // Сбросить состояние потока
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорировать неверный ввод
    }

    IResident* resident = system.findResident(name);
    if (resident) {
        try {
            if (auto* residentWithBenefits = dynamic_cast<ResidentWithBenefits*>(resident)) {
                residentWithBenefits->addService(serviceType, amount);
            }
            else if (auto* residentWithoutBenefits = dynamic_cast<ResidentWithoutBenefits*>(resident)) {
                residentWithoutBenefits->addService(serviceType, amount);
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Ошибка: жильца с именем \"" << name << "\" не существует." << std::endl;
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    HousingManagementSystem hms;

    std::string choice;
    do {
        while (true) {
            std::cout << "Выберите действие: (a) добавить жильца, (s) добавить услугу, (c) подсчитать общую стоимость, (q) выйти: ";
            std::cin >> choice;

            // Проверяем, что введенная строка состоит из одного символа
            if (choice.length() == 1 && (choice[0] == 'a' || choice[0] == 's' || choice[0] == 'c' || choice[0] == 'q')) {
                break; // Корректный ввод, выходим из цикла
            }
            else {
                std::cerr << "Ошибка: некорректный ввод. Используйте 'a', 's', 'c' или 'q'." << std::endl;
            }
        }

        switch (choice[0]) { // Используем первый символ для выбора действия
        case 'a':
            addResidentToSystem(hms);
            break;
        case 's':
            addServiceToResident(hms);
            break;
        case 'c':
        {
            double totalCosts = hms.calculateTotalCosts();
            std::cout << "Общая стоимость всех оказанных услуг: $" << totalCosts << std::endl;
        }
        break;
        case 'q':
            std::cout << "Выход из программы." << std::endl;
            break;
        }
    } while (choice[0] != 'q');

    return 0;
}
