#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using Literal = int;
using Clause = std::vector<Literal>;
using Formula = std::vector<Clause>;
using Assignment = std::unordered_map<Literal, bool>;

static int decision_count = 0;
static int propagation_count = 0;

// Compute dynamic frequencies based on the current formula
std::unordered_map<int, int> computeDynamicFrequencies(const Formula& formula, const Assignment& assignment) {
    std::unordered_map<int, int> freq;
    for (const auto& clause : formula) {
        for (Literal lit : clause) {
            if (assignment.find(lit) == assignment.end() && assignment.find(-lit) == assignment.end()) {
                freq[std::abs(lit)]++;
            }
        }
    }
    return freq;
}

// Select the next unassigned variable with the highest frequency
Literal getNextVariable(const Formula& formula, const Assignment& assignment) {
    auto freq = computeDynamicFrequencies(formula, assignment);
    if (freq.empty()) return 0;
    auto max_it = std::max_element(freq.begin(), freq.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });
    return max_it->first;
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

bool dpll(Formula formula, Assignment& assignment) {
    if (!unitPropagation(formula, assignment)) {
        return false;
    }
    if (formula.empty()) {
        return true;
    }
    Literal var = getNextVariable(formula, assignment);
    if (var == 0) {
        return true;
    }
    decision_count++;
    Assignment newAssignment = assignment;
    newAssignment[var] = true;
    newAssignment[-var] = false;
    if (dpll(formula, newAssignment)) {
        assignment = newAssignment;
        return true;
    }
    decision_count++;
    newAssignment = assignment;
    newAssignment[var] = false;
    newAssignment[-var] = true;
    if (dpll(formula, newAssignment)) {
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
    Assignment assignment;
    bool satisfiable = dpll(formula, assignment);
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