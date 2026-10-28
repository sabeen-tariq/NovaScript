#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
using namespace std;

unordered_map<string, int> variables;

int getValue(const string& value) {
    if (variables.count(value))
        return variables[value];

    try {
        size_t pos;
        int number = stoi(value, &pos);
        if (pos == value.size())
            return number;
    } catch (...) {}

    throw runtime_error("Unknown variable: " + value);
}

void executeLine(const string& line) {
    stringstream ss(line);
    string command, name, equal, a, op, b;
    ss >> command;

    if (command.empty() || command[0] == '#')
        return;

    if (command == "LET") {
        ss >> name >> equal >> a;

        if (name.empty() || equal != "=" || a.empty())
            throw runtime_error("Usage: LET name = value");

        int result = getValue(a);

        if (ss >> op >> b) {
            int y = getValue(b);

            if (op == "+") result += y;
            else if (op == "-") result -= y;
            else if (op == "*") result *= y;
            else if (op == "/" && y != 0) result /= y;
            else if (op == "/" && y == 0)
                throw runtime_error("Division by zero");
            else
                throw runtime_error("Unknown operator");
        }

        variables[name] = result;
    }
    else if (command == "PRINT") {
    string remaining;
    getline(ss, remaining);

    size_t start = remaining.find_first_not_of(" \t");

    if (start == string::npos)
        throw runtime_error("Usage: PRINT value");

    remaining = remaining.substr(start);

    if (remaining.size() >= 2 &&
        remaining.front() == '"' &&
        remaining.back() == '"') {
        cout << remaining.substr(1, remaining.size() - 2) << '\n';
        return;
    }

    ss.clear();
    ss.str(remaining);

    ss >> a;

    int result = getValue(a);

    if (ss >> op >> b) {
        int y = getValue(b);

        if (op == "+") result += y;
        else if (op == "-") result -= y;
        else if (op == "*") result *= y;
        else if (op == "/" && y != 0) result /= y;
        else if (op == "/" && y == 0)
            throw runtime_error("Division by zero");
        else
            throw runtime_error("Unknown operator");
    }

    cout << result << '\n';
}
    else {
        throw runtime_error("Unknown command: " + command);
    }
}
void resetVariables() {
    variables.clear();
}

int main(int argc, char* argv[]) {
    resetVariables();
    cout << "NovaScript Interpreter v1.2\n";

    if (argc > 1) {
        ifstream file(argv[1]);

        if (!file) {
            cerr << "Error: Cannot open file " << argv[1] << '\n';
            return 1;
        }

        string line;
        int lineNumber = 0;

        while (getline(file, line)) {
            lineNumber++;
            try {
                executeLine(line);
            } catch (const exception& e) {
                cerr << "Line " << lineNumber
                     << ": " << e.what() << '\n';
                return 1;
            }
        }
    }
    else {
        string line;

        while (true) {
            cout << "nova> ";

            if (!getline(cin, line) ||
                line == "EXIT")
                break;

            try {
                executeLine(line);
            } catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }

    return 0;
}