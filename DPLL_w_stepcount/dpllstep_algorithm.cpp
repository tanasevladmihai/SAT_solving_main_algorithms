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
                    propagation_count++; // Count each unit propagation
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
    Literal var = 0;
    for (const auto& clause : formula) {
        for (Literal lit : clause) {
            if (assignment.find(lit) == assignment.end() && assignment.find(-lit) == assignment.end()) {
                var = std::abs(lit);
                break;
            }
        }
        if (var != 0) break;
    }
    if (var == 0) {
        return true;
    }
    decision_count++; // Count each decision
    Assignment newAssignment = assignment;
    newAssignment[var] = true;
    newAssignment[-var] = false;
    if (dpll(formula, newAssignment)) {
        assignment = newAssignment;
        return true;
    }
    decision_count++; // Count the second decision
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