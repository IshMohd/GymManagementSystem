#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>

class User {
private:
    std::string name;
    int id;
    std::string membershipType;
    std::string workoutPlan;

public:
    User(std::string uname, int uid, std::string type) 
        : name(uname), id(uid), membershipType(type), workoutPlan("") {}

    void setWorkoutPlan(const std::string& plan) { workoutPlan = plan; }
    
    void display() const {
        std::cout << "\nName: " << name
                  << "\nID: " << id
                  << "\nMembership: " << membershipType
                  << "\nWorkout Plan: " << (workoutPlan.empty() ? "None" : workoutPlan)
                  << "\n";
    }

    int getId() const { return id; }
    std::string getWorkoutPlan() const { return workoutPlan; }
    std::string getName() const { return name; }
    std::string getMembership() const { return membershipType; }
};

class GymSystem {
private:
    std::vector<User> users;
    int nextID;

    void saveAllToFile() {
        std::ofstream file("gym_members.txt");
        if (file.is_open()) {
            for (const auto& user : users) {
                file << user.getName() << "|"
                     << user.getId() << "|"
                     << user.getMembership() << "|"
                     << user.getWorkoutPlan() << "\n";
            }
            file.close();
        }
    }

public:
    GymSystem() : nextID(1) {
        loadFromFile();
        if (!users.empty()) {
            auto maxIt = std::max_element(users.begin(), users.end(),
                [](const User& a, const User& b) { return a.getId() < b.getId(); });
            nextID = maxIt->getId() + 1;
        }
    }

    void loadFromFile() {
        std::ifstream file("gym_members.txt");
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, membership, workoutPlan, idStr;
            int id;

            std::getline(ss, name, '|');
            std::getline(ss, idStr, '|');

            try {
                id = std::stoi(idStr);
            } catch (const std::invalid_argument&) {
                std::cerr << "Warning: Invalid ID found in file. Skipping entry.\n";
                continue;
            }

            std::getline(ss, membership, '|');
            std::getline(ss, workoutPlan);

            users.emplace_back(name, id, membership);
            users.back().setWorkoutPlan(workoutPlan);
        }
        file.close();
    }

    void addMember() {
        std::string name, membership;
        std::cout << "Enter your name: ";
        if (std::cin.peek() == '\n') std::cin.ignore();
        std::getline(std::cin, name);

        while (true) {
            std::cout << "Select Membership (Basic/Premium/VIP): ";
            std::cin >> membership;
            if (membership == "Basic" || membership == "Premium" || membership == "VIP") {
                break;
            }
            std::cout << "Invalid input. Please enter Basic, Premium, or VIP.\n";
        }

        users.emplace_back(name, nextID++, membership);
        saveAllToFile();
        std::cout << "\nMember Added! Your Gym ID: " << (nextID - 1) << "\n";
    }

    void removeMember() {
        int id;
        std::cout << "Enter Gym ID to remove: ";
        std::cin >> id;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Enter a valid Gym ID.\n";
            return;
        }

        auto it = std::remove_if(users.begin(), users.end(),
            [id](const User& u) { return u.getId() == id; });

        if (it != users.end()) {
            users.erase(it, users.end());
            saveAllToFile();
            std::cout << "\nMember removed successfully!\n";
        } else {
            std::cout << "\nUser not found!\n";
        }
    }

    void displayMembers() const {
        if (users.empty()) {
            std::cout << "\nNo registered members yet.\n";
            return;
        }
        for (const auto& user : users) user.display();
    }

    void createWorkoutPlan() {
        int id;
        std::cout << "\nEnter your Gym ID: ";
        std::cin >> id;

        for (auto& user : users) {
            if (user.getId() == id) {
                std::string goal;
                std::cout << "\nEnter fitness goal (Weight Loss/Muscle Gain): ";
                std::cin.ignore();
                std::getline(std::cin, goal);

                if (goal == "Weight Loss" || goal == "Muscle Gain") {
                    user.setWorkoutPlan(goal);
                    saveAllToFile();
                    std::cout << "\nWorkout Plan Created for " << goal << "!\n";
                } else {
                    std::cout << "\nInvalid goal. Please enter 'Weight Loss' or 'Muscle Gain'.\n";
                }
                return;
            }
        }
        std::cout << "\nUser not found!\n";
    }

    void displayWorkoutPlan() const {
        int id;
        std::cout << "\nEnter your Gym ID to view workout plan: ";
        std::cin >> id;
    
        for (const auto& user : users) {
            if (user.getId() == id) {
                std::string plan = user.getWorkoutPlan();
                if (!plan.empty()) {
                    std::cout << "\nYour Workout Plan: " << plan << "\n";
    
                    if (plan == "Muscle Gain") {
                        std::cout << "\n--- Muscle Gain Workout Plan ---\n";
                        std::cout << "Day 1: Chest & Triceps - Bench Press, Triceps Dips\n";
                        std::cout << "Day 2: Back & Biceps - Deadlifts, Pull-ups\n";
                        std::cout << "Day 3: Legs & Abs - Squats, Leg Press\n";
                        std::cout << "Day 4: Shoulders - Military Press, Shrugs\n";
                        std::cout << "Day 5: Full Body Compound Lifts\n";
                        std::cout << "Rest on Weekends\n";
                    } else if (plan == "Weight Loss") {
                        std::cout << "\n--- Weight Loss Workout Plan ---\n";
                        std::cout << "Day 1: Cardio + Full Body HIIT\n";
                        std::cout << "Day 2: Upper Body Strength + Core\n";
                        std::cout << "Day 3: Cardio + Lower Body\n";
                        std::cout << "Day 4: Active Recovery (Yoga, Light Jogging)\n";
                        std::cout << "Day 5: High-Intensity Interval Training (HIIT)\n";
                        std::cout << "Day 6: Strength Training + Core\n";
                        std::cout << "Rest on Sunday\n";
                    }
                } else {
                    std::cout << "\nNo workout plan found!\n";
                }
                return;
            }
        }
        std::cout << "\nUser not found!\n";
    }
    
    
};

int main() {
    GymSystem gym;
    int choice;
    do {
        std::cout << "\nGym Management System\n";
        std::cout << "1. Add Member\n2. Remove Member\n3. Display Members\n"
                  << "4. Create Workout Plan\n5. Display Workout Plan\n6. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: gym.addMember(); break;
            case 2: gym.removeMember(); break;
            case 3: gym.displayMembers(); break;
            case 4: gym.createWorkoutPlan(); break;
            case 5: gym.displayWorkoutPlan(); break;
            case 6: std::cout << "Exiting...\n"; break;
            default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 6);

    return 0;
}
