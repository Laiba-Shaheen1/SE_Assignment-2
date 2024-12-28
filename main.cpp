#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <random>

using namespace std;

// Abstract Base Class for Ant Farm
class AntFarmBase {
public:
    virtual void tick() = 0;
    virtual void giveResource(const string& resource, int amount) = 0;
    virtual void displaySummary() const = 0;
    virtual ~AntFarmBase() = default;
};

// Derived AntFarm Class
class AntFarm : public AntFarmBase {
    string species;
    int food = 0;
    int workers = 0;

public:
    AntFarm(const string& species) : species(species) {}

    void tick() override {
        cout << "Tick for " << species << " farm: Workers are gathering food.\n";
        food += workers * 10; // Simplified logic
    }

    void giveResource(const string& resource, int amount) override {
        if (resource == "food") {
            food += amount;
        } else if (resource == "workers") {
            workers += amount;
        }
        cout << "Added " << amount << " " << resource << " to " << species << " farm.\n";
    }

    void displaySummary() const override {
        cout << "Summary for " << species << " farm:\n";
        cout << "Food: " << food << ", Workers: " << workers << "\n";
    }
};

// Singleton Meadow Class
class Meadow {
    unordered_map<int, shared_ptr<AntFarmBase>> colonies;
    static Meadow* instance;
    int nextId = 1;

    Meadow() {}

public:
    static Meadow* getInstance() {
        if (!instance) instance = new Meadow();
        return instance;
    }

    int spawnColony(int x, int y, const string& species) {
        cout << "Spawning " << species << " colony at (" << x << ", " << y << ").\n";
        colonies[nextId] = make_shared<AntFarm>(species);
        return nextId++;
    }

    void giveResource(int colonyId, const string& resource, int amount) {
        if (colonies.find(colonyId) != colonies.end()) {
            colonies[colonyId]->giveResource(resource, amount);
        } else {
            cout << "Error: Colony ID " << colonyId << " not found.\n";
        }
    }

    void tick(int times) {
        for (int i = 0; i < times; ++i) {
            for (auto& [id, farm] : colonies) {
                farm->tick();
            }
        }
    }

    void displaySummary(int colonyId) {
        if (colonies.find(colonyId) != colonies.end()) {
            colonies[colonyId]->displaySummary();
        } else {
            cout << "Error: Colony ID " << colonyId << " not found.\n";
        }
    }
};

// Initialize Singleton instance
Meadow* Meadow::instance = nullptr;

// Command Processor
void processCommand(const string& command) {
    auto meadow = Meadow::getInstance();

    if (command.find("spawn") == 0) {
        int x, y;
        string species;
        sscanf(command.c_str(), "spawn %d %d %s", &x, &y, &species[0]);
        int id = meadow->spawnColony(x, y, species);
        cout << "Colony spawned with ID: " << id << "\n";
    } else if (command.find("give") == 0) {
        int colonyId, amount;
        char resource[50];
        sscanf(command.c_str(), "give %d %s %d", &colonyId, resource, &amount);
        meadow->giveResource(colonyId, resource, amount);
    } else if (command.find("tick") == 0) {
        int ticks = 1;
        sscanf(command.c_str(), "tick %d", &ticks);
        meadow->tick(ticks);
    } else if (command.find("summary") == 0) {
        int colonyId;
        sscanf(command.c_str(), "summary %d", &colonyId);
        meadow->displaySummary(colonyId);
    } else {
        cout << "Invalid command.\n";
    }
}

int main() {
    cout << "Welcome to the AntFarm Simulation!\n";
    cout << "Available commands: spawn, give, tick, summary, exit\n";
    string command;
    while (true) {
        cout << "> ";
        getline(cin, command);
        if (command == "exit") break;
        processCommand(command);
    }
    return 0;
}
