#include<bits/stdc++.h>
#include <filesystem>
#include <stdexcept> // For std::invalid_argument

using namespace std;
namespace fs = std::filesystem;

enum  TokenType{
    // single character tokens
    LEFT_BRACE, // {
    RIGHT_BRACE, // }
    LEFT_SQ, // [
    RIGHT_SQ, // ]
    COLON, // :
    COMMA, // ,
    QUOTE, // "

    // identifiers
    STRING,
    NUMBER,
    IDENTIFIER, //any other identifer

    // keywords
    TRUE,
    FALSE,
    NULL_VALUE,

    // extra util
    EOF_VALUE
};

class Token{   
public:
    TokenType type;
    string lexeme;
    string str_val;
    float decimal;
    int line; // Line number where the token starts

    // Constructor for the Token class
    Token(){
        type = EOF_VALUE;
        lexeme = "";
        str_val = "";
        decimal = 0.0f;
        line = 1;
    }

    Token(TokenType _type, string _lexeme, string _str_val, float _decimal, int _line){
        type = _type;
        lexeme = _lexeme;
        str_val = _str_val;
        decimal = _decimal;
        line = _line;
    }

    Token(TokenType _type, string _lexeme, int _line){
        type = _type;
        lexeme = _lexeme;
        line = _line;
    }
};

class Scanner{
public:
    string source;
    vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1; // Current line number

    Scanner(string input_source){
        source = input_source;
    }    

    bool isAtEnd(){
        return current >= source.size();
    }

    char advance(){
        char c = source[current];
        if (c == '\n') {
            line++;
        }
        return source[current++];
    }

    void addToken(TokenType type, int current_line){
        int len = current - start;
        string text = source.substr(start, len);
        tokens.push_back(Token(type, text, current_line));
    }

    void addToken(TokenType type, string _str_val, float decimal, int current_line){
        int len = current - start;
        string text = source.substr(start, len);
        tokens.push_back(Token(type, text, _str_val, decimal, current_line));
    }

    char peek(){
        if(isAtEnd()){
            return '\0';
        }
        return source[current];
    }

    char peekNext(){
        if(current + 1  >= source.size()) return '\0';
        return source[current+1];
    }

    bool isDigit(char c){
        return isdigit(static_cast<unsigned char>(c));
    }

    bool isAlpha(char c){
        return isalpha(static_cast<unsigned char>(c));
    }

    bool isAlphaNumeric(char c){
        return isalpha(static_cast<unsigned char>(c)) || isdigit(static_cast<unsigned char>(c)) || c == '_';
    }

    void error(int error_line, const string& message) {
        // For now, we continue scanning but could throw or exit if desired
        cerr << "Scanner error at line " << error_line << ": " << message << endl;
    }

    void scan_string(){
        while (peek()!='"' && !isAtEnd())
        {
            advance();
        }

        if (isAtEnd()) {
            error(line, "Unterminated string");
            return;
        }

        advance();

        int len = current - start;

        if (len < 2) {  // Minimal empty string
            error(line, "Invalid empty string");
            return;
        }

        string str_val = source.substr(start + 1, len - 2);
        addToken(STRING, str_val, 0.0f, line);
    }

    void scan_number(){
        while (isDigit(peek()))
        {
            advance();
        }
        // Check for .
        if (peek() == '.' && isDigit(peekNext())) {
            advance(); // Consume .
            while (isDigit(peek())) {
                advance();
            }
        }
        int len = current - start;
        string str_val = source.substr(start, len);
        try {
            float parsed_decimal = stof(str_val);
            addToken(NUMBER, "", parsed_decimal, line);
        } catch (const invalid_argument& e) {
            error(line, "Invalid number format: " + str_val);
        } catch (const out_of_range& e) {
            error(line, "Number out of range: " + str_val);
        }
    }

    void scan_identifier(){
        while (isAlphaNumeric(peek()))
        {
            advance();
        }
        int len = current - start;
        string str_val = source.substr(start, len);

        if (str_val == "true") {
            addToken(TRUE, line);
        } else if (str_val == "false") {
            addToken(FALSE, line);
        } else if (str_val == "null") {
            addToken(NULL_VALUE, line);
        } else {
            addToken(IDENTIFIER, line);
        }
    }

    void scanToken(){
        char c = advance();
        switch (c) {
            case ' ':
            case '\r':
            case '\t': break;
            case '\n': break; // Handled in advance()
            case '{': addToken(LEFT_BRACE, line); break;
            case '}': addToken(RIGHT_BRACE, line); break;
            case '[': addToken(LEFT_SQ, line); break;
            case ']': addToken(RIGHT_SQ, line); break;
            case ':': addToken(COLON, line); break;
            case ',': addToken(COMMA, line); break;
            case '"': 
                start = current - 1; // Adjust start to include the opening "
                scan_string(); 
                break;
            default:
                if (isDigit(c)) {
                    start = current - 1; // Adjust start to include the first digit
                    scan_number();
                } else if (isAlpha(c)) {
                    start = current - 1; // Adjust start to include the first alpha
                    scan_identifier();
                } else {
                    string msg = "Unexpected character: '" + string(1, c) + "'";
                    error(line, msg);
                    // Continue without adding token
                }
                break;
        }
    }

    vector<Token> scanTokens(){
        while(!isAtEnd()){
            // start is the start of lexeme, current it the current char of lexeme
            start = current;
            scanToken();
        }

        // tokens.push_back(Token(EOF_VALUE, "", line));
        return tokens;
    }
};

string getFileContent(string filePath){
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Error: cannot open file " << filePath << endl;
        // return 1;
        exit(EXIT_FAILURE);
    }

    string fileContent="";

    string line;
    while (getline(file, line)) {
        fileContent+=line+"\n";
    }
    file.close();

    return fileContent;
}

void run(string source){
    Scanner scanner(source);

    vector<Token> tokens = scanner.scanTokens();

    for(Token token: tokens){
        cout << token.lexeme << endl; 
    }

}

int main(){
    // Updated: Base path for tests directory
    string basePath = "./tests";
    
    // Traverse all sub-directories recursively for .json files
    try {
        for (const auto& entry : fs::recursive_directory_iterator(basePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                string filePath = entry.path().string();
                cout << "\n--- Processing file: " << filePath << " ---\n";
                
                string fileContent = getFileContent(filePath);
                
                run(fileContent);
                cout << "--- End of file ---\n";
            }
        }
    } catch (const filesystem::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    } catch (...) {
        cerr << "Unknown error during directory traversal" << endl;
    }
    return 0;
}