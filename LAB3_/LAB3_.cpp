#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <regex>
#include <map>
using namespace std;

string ReadFile(string path) {
    string sourceCode;

    ifstream inputFile(path);
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            sourceCode += line + "\n";
        }
        inputFile.close();
    }
    else {
        cerr << "Failed to open file." << endl;
        return "";
    }
    return sourceCode;
}

string prepareTextForLA(const string& inputText) {
    // Видалення коментарів (одно- та багаторядкових)
    string textWithoutComments = regex_replace(inputText, regex("//.*|/\\*.*?\\*/"), "");

    // Видалення зайвих пробілів і розривів рядків
    string textWithoutSpacesAndNewlines = regex_replace(textWithoutComments, regex("\\s+"), " ");

    // Видалення початкових і кінцевих пробілів
    textWithoutSpacesAndNewlines = regex_replace(textWithoutSpacesAndNewlines, regex("^\\s+|\\s+$"), "");

    return textWithoutSpacesAndNewlines;
}

// Функція розфарбовування тексту.
void colorizeText(const string& text, const string& color) {
    // Використання ANSI escape-коду для встановлення кольору тексту
    cout << "\x1B[" << color << "m" << text << "\x1B[0m";
}

// Функція для розрізнення та розфарбовування 
void highlightLexemes(const string& code) {
    // Створюємо карту, де ключ - регулярний вираз, значення - колір
    map<string, string> regexToColor = {
        { "#include <.*?>", "4" },
        { "\\b(int|float|double|char)\\b", "32" },
        { "\\b\\d+(\\.\\d+)?\\b", "31" },
        { "(\".*?\")|('.*?')", "42" },
        { "\\b(if|else|while|return|cout|cin|endl)\\b", "36" },
        { "[+\\-*/=<>]", "35" },
        { "[;{}()\\[\\]]", "33" },
        { "\\b[a-zA-Z]+\\b", "34" }
    };

    string combinedRegex;
    for (const auto& entry : regexToColor) {
        if (!combinedRegex.empty()) {
            combinedRegex += "|";
        }
        combinedRegex += "(" + entry.first + ")";
    }

    regex lexemeRegex(combinedRegex);
    smatch matches;

    size_t position = 0;
    while (regex_search(code.begin() + position, code.end(), matches, lexemeRegex)) {
        if (matches.position() > 0) {
            cout << code.substr(position, matches.position());
        }

        if (matches.str() != "") {
            // Розфарбування лексеми відповідно до карти
            string matchedText = matches.str();
            for (const auto& entry : regexToColor) {
                if (regex_match(matchedText, regex(entry.first))) {
                    colorizeText(matchedText, entry.second);
                    break;
                }
            }
        }

        position += matches.position() + matches.length();
    }

    // Вивести текст, що залишився, якщо такий є
    if (position < code.length()) {
        cout << code.substr(position);
    }
}

int main()
{
    string path = "Program.txt";

    string sourceCode = ReadFile(path), preparedCode = prepareTextForLA(sourceCode);
    highlightLexemes(preparedCode);
}