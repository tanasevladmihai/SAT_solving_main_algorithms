#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <unordered_set>
#include <chrono>

using Clause = std::set<int>;
using Formula = std::set<Clause>;

Formula dpEliminate(Formula formula, int var) {
    Formula newFormula;
    std::vector<Clause> posClauses, negClauses, otherClauses;
    // Partition clauses
    for (const auto& clause : formula) {
        if (clause.count(var)) posClauses.push_back(clause);
        else if (clause.count(-var)) negClauses.push_back(clause);
        else otherClauses.push_back(clause);
    }
    // Resolve clauses with var and -var
    for (const auto& c1 : posClauses) {
        for (const auto& c2 : negClauses) {
            Clause resolvent;
            for (int lit : c1) if (lit != var) resolvent.insert(lit);
            for (int lit : c2) if (lit != -var) resolvent.insert(lit);
            newFormula.insert(resolvent);
        }
    }
    // Add clauses without var
    for (const auto& clause : otherClauses) newFormula.insert(clause);
    return newFormula;
}

bool dpProcedure(Formula formula) {
    while (!formula.empty()) {
        if (formula.count({})) return false; // Unsatisfiable
        // Find a variable to eliminate
        std::unordered_set<int> vars;
        for (const auto& clause : formula) {
            for (int lit : clause) vars.insert(std::abs(lit));
        }
        if (vars.empty()) return true; // Satisfiable
        int var = *vars.begin();
        formula = dpEliminate(formula, var);
    }
    return true; // Satisfiable
}

int main() {
    Formula formula;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        std::istringstream iss(line);
        Clause clause;
        int lit;
        while (iss >> lit) clause.insert(lit);
        formula.insert(clause);
    }
    auto start = std::chrono::high_resolution_clock::now();

    bool satisfiable = dpProcedure(formula);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << (satisfiable ? "Satisfiable" : "Unsatisfiable") << std::endl;
    std::cout << elapsed_seconds.count() << "s" << std::endl;
    return 0;
}