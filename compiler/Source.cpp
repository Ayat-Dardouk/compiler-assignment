// ayat dardouk & kinanah abu eideh
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

enum TokenType {
    DECL, INTEGER, STRING, ENDDECL, IDENTIFIER, REAL_VALUE, STRING_VALUE,
    OPERATOR, SEPARATOR, INVALID, KEYWORD
};

struct Token {
    TokenType type;
    string lexeme;
    int line;
};

const vector<string> keywords = {
    "Decl", "Integer", "String", "Enddecl",
    "Read", "Print", "If", "Then", "Else", "Endif",
    "While", "Do", "Endwhile", "Break", "Continue",
    "Main", "Return", "Create", "Open", "Write",
    "Seek", "Close", "Delete"
};

const vector<string> operators = { "=", "+", "-", "*", "/", ">", "<", ">=", "<=", "!=", "==", "&&", "||", "^", "@" };
const vector<char> singleCharOperators = { '=', '+', '-', '*', '/', '>', '<', '^', '@' };
const vector<string> separators = { "(", ")", "{", "}", "[", "]", ";" };

bool isKeyword(const string& word) {
    return find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool isOperator(const string& lexeme) {
    return find(operators.begin(), operators.end(), lexeme) != operators.end();
}

bool isSeparator(char ch) {
    return find(separators.begin(), separators.end(), string(1, ch)) != separators.end();
}

TokenType getTokenType(const string& lexeme) {
    if (isKeyword(lexeme)) return KEYWORD;
    if (regex_match(lexeme, regex("[A-Za-z][A-Za-z0-9_]*"))) return IDENTIFIER;
    if (regex_match(lexeme, regex("-?[0-9]+"))) return INTEGER;
    if (regex_match(lexeme, regex("-?[0-9]*\\.[0-9]+"))) return REAL_VALUE;
    if (lexeme.size() >= 2 && lexeme.front() == '\'' && lexeme.back() == '\'') return STRING_VALUE;
    if (isOperator(lexeme)) return OPERATOR;
    if (isSeparator(lexeme[0])) return SEPARATOR;
    return INVALID;
}

string tokenTypeToString(TokenType type) {
    switch (type) {
    case DECL: return "Decl";
    case INTEGER: return "Integer";
    case STRING: return "String";
    case ENDDECL: return "Enddecl";
    case IDENTIFIER: return "Identifier";
    case REAL_VALUE: return "Real_Value";
    case STRING_VALUE: return "String_Value";
    case OPERATOR: return "Operator";
    case SEPARATOR: return "Separator";
    case KEYWORD: return "Keyword";
    default: return "Invalid";
    }
}

void tokenize(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName);
    string line;
    int lineNumber = 0;

    if (!inputFile.is_open()) {
        cerr << "Error opening input file." << endl;
        return;
    }
    if (!outputFile.is_open()) {
        cerr << "Error opening output file." << endl;
        return;
    }

    while (getline(inputFile, line)) {
        lineNumber++;

        // Skip comments
        if (line.find("##") == 0) {
            continue;
        }

        istringstream stream(line);
        string word;
        while (stream >> word) {
            // Handle multi-word strings
            if (word.front() == '\'' && word.back() != '\'') {
                string rest;
                while (stream >> rest) {
                    word += " " + rest;
                    if (rest.back() == '\'') break;
                }
            }

            // Tokenize word by word
            size_t i = 0;
            while (i < word.length()) {
                if (isSeparator(word[i])) {
                    outputFile << tokenTypeToString(SEPARATOR) << endl;
                    i++;
                }
                else if (find(singleCharOperators.begin(), singleCharOperators.end(), word[i]) != singleCharOperators.end()) {
                    string op(1, word[i]);
                    if (i + 1 < word.length() && find(singleCharOperators.begin(), singleCharOperators.end(), word[i + 1]) != singleCharOperators.end()) {
                        string doubleOp = op + word[i + 1];
                        if (isOperator(doubleOp)) {
                            op = doubleOp;
                            i++;
                        }
                    }
                    outputFile << tokenTypeToString(OPERATOR) << ", lexeme=" << op << endl;
                    i++;
                }
                else {
                    size_t j = i;
                    while (j < word.length() && !isSeparator(word[j]) && find(singleCharOperators.begin(), singleCharOperators.end(), word[j]) == singleCharOperators.end()) {
                        j++;
                    }
                    string lexeme = word.substr(i, j - i);
                    TokenType type = getTokenType(lexeme);
                    if (type == INVALID) {
                        outputFile << "Error: invalid token " << lexeme << " at line " << lineNumber << endl;
                        break;
                    }
                    outputFile << tokenTypeToString(type);
                    if (type == IDENTIFIER || type == INTEGER || type == REAL_VALUE || type == STRING_VALUE || type == KEYWORD) {
                        outputFile << ", lexeme=" << lexeme;
                    }
                    outputFile << endl;
                    i = j;
                }
            }
        }
    }

    inputFile.close();
    outputFile.close();
}

int main() {
    string inputFileName = "input.txt";
    string outputFileName = "output.txt";
    tokenize(inputFileName, outputFileName);
    cout << "Tokenization complete. Check the output file for results." << endl;
    return 0;
}
