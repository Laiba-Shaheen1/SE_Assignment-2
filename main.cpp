#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <random>
#include <functional>
#include <sstream>
#include <map>

using namespace std;

// --- Creational Pattern: Factory for Ant Types ---

// Abstract Ant Class
class Ant {
public:
    virtual void performAction() = 0;
    virtual string getType() const = 0;
    virtual ~Ant() = default;
};

class Drone : public Ant {
public:
    void performAction() override {
        cout << "Drone is searching for food." << endl;
    }
    string getType() const override { return "Drone"; }
};

class Warrior : public Ant {
public:
    void performAction() override {
        cout << "Warrior is hunting enemies or searching for food." << endl;
    }
    string getType() const override { return "Warrior"; }
};

class Queen : public Ant {
public:
    void performAction() override {
        cout << "Queen is spawning an egg." << endl;
    }
    string getType() const override { return "Queen"; }
};

// Factory Class for Ant Creation
class AntFactory {
public:
    static unique_ptr<Ant> createAnt(const string& antType) {
        if (antType == "drone") {
            return make_unique<Drone>();
        } else if (antType == "warrior") {
            return make_unique<Warrior>();
        } else if (antType == "queen") {
            return make_unique<Queen>();
        }
        return nullptr;
    }
};

// --- Structural Pattern: Builder for AntFarm ---

class Room {
public:
    virtual void addAnt(unique_ptr<Ant> ant) = 0;
    virtual void performActions() = 0;
    virtual ~Room() = default;
};

class WorkerRoom : public Room {
    vector<unique_ptr<Ant>> ants;
public:
    void addAnt(unique_ptr<Ant> ant) override {
        ants.push_back(std::move(ant));
    }

    void performActions() override {
        for (auto& ant : ants) {
            ant->performAction();
        }
    }
};

class RestRoom : public Room {
    vector<unique_ptr<Ant>> ants;
public:
    void addAnt(unique_ptr<Ant> ant) override {
        ants.push_back(std::move(ant));
    }

    void performActions() override {
        for (auto& ant : ants) {
            ant->performAction();
        }
    }
};

class AntFarm {
    string name;
    vector<unique_ptr<Room>> rooms;
    vector<unique_ptr<Ant>> ants;
    map<string, int> resources;

public:
    AntFarm(const string& name) : name(name) {}

    void addRoom(unique_ptr<Room> room) {
        rooms.push_back(std::move(room));
    }

    void addAnt(unique_ptr<Ant> ant) {
        ants.push_back(std::move(ant));
    }

    void addResource(const string& resource, int amount) {
        resources[resource] += amount;
    }

    void performActions() {
        for (auto& room : rooms) {
            room->performActions();
        }
        for (auto& ant : ants) {
            ant->performAction();
        }
    }

    const string& getName() const { return name; }

    void displaySummary() {
        cout << "Colony: " << name << endl;
        cout << "Resources: " << endl;
        for (const auto& res : resources) {
            cout << res.first << ": " << res.second << endl;
        }
        cout << "Ants: " << endl;
        for (const auto& ant : ants) {
            cout << " - " << ant->getType() << endl;
        }
    }
};


class Meadow {
    unordered_map<int, unique_ptr<AntFarm>> colonies;
    int nextColonyId = 1;
    static Meadow* instance;

    Meadow() {}

public:
    static Meadow* getInstance() {
        if (!instance) {
            instance = new Meadow();
        }
        return instance;
    }

    int spawnColony(const string& speciesType) {
        unique_ptr<AntFarm> newFarm = make_unique<AntFarm>("Farm" + to_string(nextColonyId));
        int numAnts = (rand() % 5) + 5;
        for (int i = 0; i < numAnts; ++i) {
            auto ant = AntFactory::createAnt(speciesType);
            newFarm->addAnt(std::move(ant));
        }
        colonies[nextColonyId] = std::move(newFarm);
        return nextColonyId++;
    }

    void addResourceToColony(int colonyId, const string& resource, int amount) {
        if (colonies.find(colonyId) != colonies.end()) {
            colonies[colonyId]->addResource(resource, amount);
        }
    }

    void performActions() {
        for (auto& [id, farm] : colonies) {
            farm->performActions();
        }
    }

    bool checkSimulationEnd() {
        return colonies.size() <= 1;
    }

    void displayColonyStatus(int colonyId) {
        if (colonies.find(colonyId) != colonies.end()) {
            colonies[colonyId]->displaySummary();
        } else {
            cout << "Colony not found!" << endl;
        }
    }

    unordered_map<int, unique_ptr<AntFarm>>& getColonies() {
        return colonies;
    }
};

Meadow* Meadow::instance = nullptr;



class Mediator {
    Meadow* meadow;

public:
    Mediator(Meadow* meadow) : meadow(meadow) {}

    void tick(int ticks = 1) {
        for (int i = 0; i < ticks; ++i) {
            meadow->performActions();
        }
    }
};



void processCommand(const string& command) {
    Meadow* meadow = Meadow::getInstance();
    Mediator mediator(meadow);
    stringstream ss(command);
    string cmd;
    ss >> cmd;

    if (cmd == "spawn") {
        int x, y;
        string species;
        ss >> x >> y >> species;
        int colonyId = meadow->spawnColony(species);
        cout << "Colony " << colonyId << " of species " << species << " spawned at position (" << x << ", " << y << ")." << endl;
    }
    else if (cmd == "give") {
        int colonyId, amount;
        string resource;
        ss >> colonyId >> resource >> amount;
        meadow->addResourceToColony(colonyId, resource, amount);
        cout << "Gave " << amount << " " << resource << " to colony " << colonyId << "." << endl;
    }
    else if (cmd == "tick") {
        int ticks = 1;
        if (ss >> ticks) {
            mediator.tick(ticks);
        } else {
            mediator.tick();
        }
        cout << "Performed " << ticks << " tick(s)." << endl;
    }
    else if (cmd == "summary") {
        int colonyId;
        ss >> colonyId;
        meadow->displayColonyStatus(colonyId);
    }
    else {
        cout << "Invalid command." << endl;
    }
}


int main() {
    string command;
    cout << "Welcome to the Ant Farm Simulation! Type commands to control the simulation." << endl;
    cout << "Available commands: spawn X Y T, give I R A, tick [T], summary I" << endl;

    while (true) {
        cout << "> ";
        getline(cin, command);
        if (command == "exit") {
            break;
        }
        processCommand(command);
    }

    cout << "Simulation ended!" << endl;
    return 0;
}
