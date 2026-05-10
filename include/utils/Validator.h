#ifndef VALIDATOR_H
#define VALIDATOR_H
#include <vector>
#include "../core/Process.h"
using namespace std;

class Validator {
public:
    static string validateId(const string& id, const vector<Process>& existing);
    static string validateBurst(string& val);
    static string validatePriority(string& val);
    static string validateQuantum(string& val);
    static string validateArrival(string& val);
    static vector<string> validateProcess(vector<Process>& process,int Quantum );
};

#endif