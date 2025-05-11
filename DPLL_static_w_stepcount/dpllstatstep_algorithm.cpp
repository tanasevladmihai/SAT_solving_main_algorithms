#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <map>

using Literal = int;
using Clause = std::vector<Literal>;
using Formula = std::vector<Clause>;
using Assignment = std::unordered_map<Literal, bool>;

static int decision_count = 0;
static int propagation_count = 0;

// Compute static frequencies from the original formula
std::map<int, int> computeStaticFrequencies(const Formula& formula) {
    std::map<int, int> freq;
    for (const auto& clause : formula) {
        for (Literal lit : clause) {
            freq[std::abs(lit)]++;
        }
    }
    return freq;
}

// Select the next unassigned variable based on static frequencies
Literal getNextVariable(const std::map<int, int>& freq, const Assignment& assignment) {
    for (auto it = freq.rbegin(); it != freq.rend(); ++it) {
        int var = it->first;
        if (assignment.find(var) == assignment.end() && assignment.find(-var) == assignment.end()) {
            return var;
        }
    }
    return 0;
}

bool unitPropagation(Formula& formula, Assignment& assignment) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& clause : formula) {
            if (clause.size() == 1) {
                Literal unit = clause[0];
                if (assignment.find(unit) == assignment.end()) {
                    assignment[unit] = true;
                    assignment[-unit] = false;
                    changed = true;
                    propagation_count++;
                }
            }
        }
        formula.erase(std::remove_if(formula.begin(), formula.end(), [&](const Clause& c) {
            for (Literal lit : c) {
                if (assignment.find(lit) != assignment.end() && assignment[lit]) {
                    return true;
                }
            }
            return false;
        }), formula.end());
        for (auto& clause : formula) {
            clause.erase(std::remove_if(clause.begin(), clause.end(), [&](Literal lit) {
                if (assignment.find(lit) != assignment.end()) {
                    return !assignment[lit];
                }
                return false;
            }), clause.end());
            if (clause.empty()) {
                return false;
            }
        }
    }
    return true;
}

bool dpll(Formula formula, Assignment& assignment, const std::map<int, int>& freq) {
    if (!unitPropagation(formula, assignment)) {
        return false;
    }
    if (formula.empty()) {
        return true;
    }
    Literal var = getNextVariable(freq, assignment);
    if (var == 0) {
        return true;
    }
    decision_count++;
    Assignment newAssignment = assignment;
    newAssignment[var] = true;
    newAssignment[-var] = false;
    if (dpll(formula, newAssignment, freq)) {
        assignment = newAssignment;
        return true;
    }
    decision_count++;
    newAssignment = assignment;
    newAssignment[var] = false;
    newAssignment[-var] = true;
    if (dpll(formula, newAssignment, freq)) {
        assignment = newAssignment;
        return true;
    }
    return false;
}

int main() {
    Formula formula;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        std::istringstream iss(line);
        Clause clause;
        int lit;
        while (iss >> lit) {
            clause.push_back(lit);
        }
        formula.push_back(clause);
    }
    auto freq = computeStaticFrequencies(formula);
    Assignment assignment;
    bool satisfiable = dpll(formula, assignment, freq);
    std::cout << (satisfiable ? "Satisfiable" : "Unsatisfiable") << std::endl;
    if (satisfiable) {
        std::cout << "Assignment: ";
        for (const auto& pair : assignment) {
            if (pair.first > 0) {
                std::cout << pair.first << "=" << pair.second << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Decision Steps: " << decision_count << std::endl;
    std::cout << "Propagation Steps: " << propagation_count << std::endl;
    return 0;
}