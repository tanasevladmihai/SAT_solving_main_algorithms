#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using Literal = int;
using Clause = std::vector<Literal>;
using Formula = std::vector<Clause>;
using Assignment = std::unordered_map<Literal, bool>;

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
                }
            }
        }
        // Simplify formula based on assignment
        formula.erase(std::remove_if(formula.begin(), formula.end(), [&](const Clause& c) {
            for (Literal lit : c) {
                if (assignment.find(lit) != assignment.end() && assignment[lit]) {
                    return true;  // Clause satisfied
                }
            }
            return false;
        }), formula.end());
        for (auto& clause : formula) {
            clause.erase(std::remove_if(clause.begin(), clause.end(), [&](Literal lit) {
                if (assignment.find(lit) != assignment.end()) {
                    return !assignment[lit];  // Remove false literals
                }
                return false;
            }), clause.end());
            if (clause.empty()) {
                return false;  // Conflict
            }
        }
    }
    return true;
}

bool dpll(Formula formula, Assignment& assignment) {
    if (!unitPropagation(formula, assignment)) {
        return false;  // Conflict detected
    }
    if (formula.empty()) {
        return true;  // Satisfiable
    }
    // Choose a variable to assign
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
        return true;  // All variables assigned
    }
    // Try assigning true
    Assignment newAssignment = assignment;
    newAssignment[var] = true;
    newAssignment[-var] = false;
    if (dpll(formula, newAssignment)) {
        assignment = newAssignment;
        return true;
    }
    // Try assigning false
    newAssignment = assignment;
    newAssignment[var] = false;
    newAssignment[-var] = true;
    if (dpll(formula, newAssignment)) {
        assignment = newAssignment;
        return true;
    }
    return false;  // Backtrack
}

int main() {
    // Input format: one clause per line, e.g., "1 2" for (x1 v x2), "-1 3" for (-x1 v x3)
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

    auto start = std::chrono::high_resolution_clock::now();

    bool satisfiable = dpll(formula, assignment);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

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
    std::cout << "Solving time: " << elapsed_seconds.count() << " s " << std::endl;
    return 0;
}