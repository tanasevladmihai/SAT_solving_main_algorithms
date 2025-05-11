#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <chrono>

using Clause = std::set<int>;  // Positive for variable, negative for negation
using Formula = std::set<Clause>;

Formula resolve(const Formula& formula) {
    Formula newFormula = formula;
    bool added = true;
    while (added) {
        added = false;
        for (auto it1 = newFormula.begin(); it1 != newFormula.end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != newFormula.end(); ++it2) {
                for (int lit : *it1) {
                    if (it2->find(-lit) != it2->end()) {
                        Clause resolvent;
                        for (int l : *it1) if (l != lit) resolvent.insert(l);
                        for (int l : *it2) if (l != -lit) resolvent.insert(l);
                        if (resolvent.empty()) {
                            return {resolvent};  // Empty clause found, unsatisfiable
                        }
                        if (newFormula.find(resolvent) == newFormula.end()) {
                            newFormula.insert(resolvent);
                            added = true;
                        }
                        break;
                    }
                }
            }
        }
    }
    return newFormula;
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
            clause.insert(lit);
        }
        formula.insert(clause);
    }

    auto start = std::chrono::high_resolution_clock::now();

    Formula result = resolve(formula);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if (result.find({}) != result.end()) {
        std::cout << "Unsatisfiable" << std::endl;
    } else {
        std::cout << "Resolution did not derive empty clause" << std::endl;
    }

    std::cout << "Solving time: " << elapsed_seconds.count() << "s" << std::endl;
    return 0;
}