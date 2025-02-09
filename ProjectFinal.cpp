#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <memory>
#include <iomanip>

class User {
protected:
    std::string name;
    int id;
    std::string workoutPlan;
    double height;   // in meters
    double weight;   // in kilograms

public:
    User(std::string uname, int uid) 
        : name(uname), id(uid), workoutPlan(""), height(0), weight(0) {}

    virtual ~User() {}

    virtual std::string getMembershipType() const = 0; // Pure virtual function

    void setWorkoutPlan(const std::string& plan) { workoutPlan = plan; }
    
    virtual void display() const {
        std::cout << "\nName: " << name
                  << "\nID: " << id
                  << "\nMembership: " << getMembershipType()
                  << "\nWorkout Plan: " << (workoutPlan.empty() ? "None" : workoutPlan)
                  << "\nHeight(in m): " << height << " m"
                  << "\nWeight(in kg): " << weight << " kg"
                  << "\n";
    }

    int getId() const { return id; }
    std::string getWorkoutPlan() const { return workoutPlan; }
    std::string getName() const { return name; }

    void setHeight(double h) { height = h; }
    void setWeight(double w) { weight = w; }
    double getHeight() const { return height; }
    double getWeight() const { return weight; }

    double calculateBMI() const {
        if (height <= 0 || weight <= 0) {
            return -1; // Indicates invalid data
        }
        return weight / (height * height);
    }
};

class BasicMember : public User {
public:
    BasicMember(std::string uname, int uid) : User(uname, uid) {}

    std::string getMembershipType() const override {
        return "Basic";
    }
};

class PremiumMember : public User {
public:
    PremiumMember(std::string uname, int uid) : User(uname, uid) {}

    std::string getMembershipType() const override {
        return "Premium";
    }
};

class VIPMember : public User {
public:
    VIPMember(std::string uname, int uid) : User(uname, uid) {}

    std::string getMembershipType() const override {
        return "VIP";
    }
};

class GymSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    int nextID;

    void saveAllToFile() {
        std::ofstream file("gym_members.txt");
        if (file.is_open()) {
            for (const auto& user : users) {
                file << user->getName() << "|"
                     << user->getId() << "|"
                     << user->getMembershipType() << "|"
                     << user->getWorkoutPlan() << "|"
                     << user->getHeight() << "|"
                     << user->getWeight() << "\n";
            }
            file.close();
        }
    }

public:
    GymSystem() : nextID(1) {
        loadFromFile();
        if (!users.empty()) {
            auto maxIt = std::max_element(users.begin(), users.end(),
                [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) { 
                    return a->getId() < b->getId(); 
                });
            nextID = (*maxIt)->getId() + 1;
        }
    }

    void loadFromFile() {
        std::ifstream file("gym_members.txt");
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, membershipType, workoutPlan, idStr, heightStr, weightStr;
            int id;
            double height, weight;

            std::getline(ss, name, '|');
            std::getline(ss, idStr, '|');
            std::getline(ss, membershipType, '|');
            std::getline(ss, workoutPlan, '|');
            std::getline(ss, heightStr, '|');
            std::getline(ss, weightStr);

            try {
                id = std::stoi(idStr);
                height = std::stod(heightStr);
                weight = std::stod(weightStr);
            } catch (const std::invalid_argument&) {
                std::cerr << "Warning: Invalid data found in file. Skipping entry.\n";
                continue;
            }

            if (membershipType == "Basic") {
                users.push_back(std::make_unique<BasicMember>(name, id));
            } else if (membershipType == "Premium") {
                users.push_back(std::make_unique<PremiumMember>(name, id));
            } else if (membershipType == "VIP") {
                users.push_back(std::make_unique<VIPMember>(name, id));
            } else {
                std::cerr << "Warning: Unknown membership type '" << membershipType << "'. Skipping entry.\n";
                continue;
            }

            users.back()->setWorkoutPlan(workoutPlan);
            users.back()->setHeight(height);
            users.back()->setWeight(weight);
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

        double height, weight;
        std::cout << "Enter height in meters: ";
        while (!(std::cin >> height) || height <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter a valid height in meters: ";
        }
        std::cout << "Enter weight in kilograms: ";
        while (!(std::cin >> weight) || weight <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter a valid weight in kilograms: ";
        }

        if (membership == "Basic") {
            users.push_back(std::make_unique<BasicMember>(name, nextID));
        } else if (membership == "Premium") {
            users.push_back(std::make_unique<PremiumMember>(name, nextID));
        } else if (membership == "VIP") {
            users.push_back(std::make_unique<VIPMember>(name, nextID));
        }

        users.back()->setHeight(height);
        users.back()->setWeight(weight);
        nextID++;

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
            [id](const std::unique_ptr<User>& u) { return u->getId() == id; });

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
        for (const auto& user : users) {
            user->display();
        }
    }

    void createWorkoutPlan() {
        int id;
        std::cout << "\nEnter your Gym ID: ";
        std::cin >> id;

        for (auto& user : users) {
            if (user->getId() == id) {
                std::string goal;
                std::cout << "\nEnter fitness goal (Weight Loss/Muscle Gain): ";
                std::cin.ignore();
                std::getline(std::cin, goal);

                if (goal == "Weight Loss" || goal == "Muscle Gain") {
                    user->setWorkoutPlan(goal);
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
            if (user->getId() == id) {
                std::string plan = user->getWorkoutPlan();
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

    void calculateBMI() const {
        int id;
        std::cout << "Enter your Gym ID: ";
        std::cin >> id;
    
        for (const auto& user : users) {
            if (user->getId() == id) {
                double bmi = user->calculateBMI();
                if (bmi == -1) {
                    std::cout << "\nHeight or weight not set for this user.\n";
                } else {
                    std::cout << "\nBMI for " << user->getName() << ": " << std::fixed << std::setprecision(1) << bmi << "\n";
    
                    // Indicating health levels
                    if (bmi < 18.5) {
                        std::cout << "Status: Underweight - Consider a balanced diet to gain healthy weight.\n";
                    } else if (bmi >= 18.5 && bmi < 24.9) {
                        std::cout << "Status: Healthy weight - Keep maintaining a balanced lifestyle!\n";
                    } else if (bmi >= 25 && bmi < 29.9) {
                        std::cout << "Status: Overweight - Try regular exercise and a healthy diet.\n";
                    } else {
                        std::cout << "Status: Obese - It is advisable to consult a healthcare professional for guidance.\n";
                    }
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
                  << "4. Create Workout Plan\n5. Display Workout Plan\n6. Calculate BMI\n7. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: gym.addMember(); break;
            case 2: gym.removeMember(); break;
            case 3: gym.displayMembers(); break;
            case 4: gym.createWorkoutPlan(); break;
            case 5: gym.displayWorkoutPlan(); break;
            case 6: gym.calculateBMI(); break;
            case 7: std::cout << "Exiting...\n"; break;
            default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 7);

    return 0;
}
