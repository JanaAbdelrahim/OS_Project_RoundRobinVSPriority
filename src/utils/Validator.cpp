#include "utils/Validator.h"
#include <cctype>
#include <unordered_map>
using namespace std;

static bool isNumber(const string &str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

static bool isNonNegativeNumber(const string &str) {
    return isNumber(str) && stoi(str) >= 0;
}

static bool isPositiveNumber(const string &str) {
    return isNumber(str) && stoi(str) > 0;
}


static string validateRequiredFields(const string& id,const string& arrival,const string& burst,const string& priority)
{
    if (id.empty() || arrival.empty() || burst.empty() || priority.empty())
        return "All fields are required";

    return "OK";
}



// ID
string Validator::validateId(const string& id, const vector<Process>& existing) {
    if (id.empty())
        return "ID cannot be empty";

    int count = 0;
    for (const auto &p : existing) {
        if (p.getId() == id)
            count++;
    }

    if (count >= 1)
        return "ID already exists";

    return "OK";
}

// Arrival
string Validator::validateArrival(string& val) {
    if (!isNonNegativeNumber(val))
        return "Arrival time must be a non-negative number";

    return "OK";
}

// Burst
string Validator::validateBurst(string& val) {
    if (!isPositiveNumber(val))
        return "Burst time must be a number > 0";

    return "OK";
}

// Priority
string Validator::validatePriority(string& val) {
    if (!isPositiveNumber(val))
        return "Priority must be a number > 0";

    return "OK";
}

// Quantum
string Validator::validateQuantum(string& val) {
    if (!isPositiveNumber(val))
        return "Quantum must be a number> 0";

    return "OK";
}

// Process
vector<string> Validator::validateProcess(vector<Process>& process, int Quantum)
{
    vector<string> errors;

    // Quantum check
    string qVal = to_string(Quantum);
    string qCheck = validateQuantum(qVal);
    if (qCheck != "OK") {
        errors.push_back("Quantum -> " + qCheck);
    }


    unordered_map<string, int> idCount;
    for (const auto &p : process) {
        idCount[p.getId()]++;
    }

    for (auto &p : process) {


        string reqCheck = validateRequiredFields(
            p.getId(),
            to_string(p.getArrivalTime()),
            to_string(p.getBurstTime()),
            to_string(p.getPriority())
        );

        if (reqCheck != "OK") {
            errors.push_back("Process " + p.getId() + " -> " + reqCheck);
            continue;
        }

        //ID validation
        if (idCount[p.getId()] > 1) {
            errors.push_back("Process " + p.getId() + " -> ID already exists");
        }

        //Arrival
        string arrivalVal = to_string(p.getArrivalTime());
        if (validateArrival(arrivalVal) != "OK") {
            errors.push_back("Process " + p.getId() + " -> Arrival time must be >= 0");
        }

        //Burst
        string burstVal = to_string(p.getBurstTime());
        if (validateBurst(burstVal) != "OK") {
            errors.push_back("Process " + p.getId() + " -> Burst time must be > 0");
        }

        //Priority
        string priorityVal = to_string(p.getPriority());
        if (validatePriority(priorityVal) != "OK") {
            errors.push_back("Process " + p.getId() + " -> Priority must be >= 0");
        }
    }

    return errors;
}