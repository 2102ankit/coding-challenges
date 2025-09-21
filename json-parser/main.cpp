#include<bits/stdc++.h>
#include <filesystem>

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

    // Constructor for the Token class
    Token(TokenType _type, string _lexeme, string _str_val, float _decimal){
        type = _type;
        lexeme = _lexeme;
        str_val = _str_val;
        decimal = _decimal;
    }

    Token(TokenType _type, string _lexeme){
        type = _type;
        lexeme = _lexeme;
    }
};

class Scanner{
public:
    string source;
    vector<Token> tokens;

    int start = 0;
    int current = 0;

    Scanner(string input_source){
        source = input_source;
    }    

    bool isAtEnd(){
        return current >= source.size();
    }

    char advance(){
        return source[current++];
    }

    void addToken(TokenType type){
        int len = current - start;
        string text = source.substr(start,len);
        tokens.push_back(Token(type,text));
    }

    void addToken(TokenType type, string _str_val, float decimal){
        int len = current - start;
        string text = source.substr(start,len);
        tokens.push_back(Token(type,text,_str_val,decimal));
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

    void scan_string(){
        while (peek()!='"' && !isAtEnd())
        {
            advance();
        }

        if(isAtEnd()){
            cerr << "Unterminated string\n";
            return;
        }

        advance();

        int len = current - start;

        if (len < 2) {  // Minimal empty string
            cerr << "Unterminated string\n";
            return;
        }

        string str_val = source.substr(start+1, len-2);
        addToken(STRING,str_val,0.0f);
    }

    void scan_number(){
        while (isDigit(peek()))
        {
            advance();
        }
        //check for .
        if(peek()=='.' && isDigit(peekNext())){
            do
            {
                advance();
            } while (isDigit(peek()));
            
        }
        int len = current - start;
        string str_val = source.substr(start, len);
        float parsed_decimal = stof(str_val);
        
        addToken(NUMBER,"",parsed_decimal);
    }

    void scan_identifier(){
        while (isAlphaNumeric(peek()))
        {
            advance();
        }
        int len = current - start;
        string str_val = source.substr(start, len);

        if(str_val=="true") addToken(TRUE);
        else if(str_val=="false") addToken(FALSE);
        else if(str_val=="null") addToken(NULL_VALUE);
        else addToken(IDENTIFIER);
        
    }

    void scanToken(){
        char c = advance();
        switch (c){
            case ' ':
            case '\n':
            case '\r':
            case '\t': break;
            case '{': addToken(LEFT_BRACE); break;
            case '}': addToken(RIGHT_BRACE); break;
            case '[': addToken(LEFT_SQ); break;
            case ']': addToken(RIGHT_SQ); break;
            case ':': addToken(COLON); break;
            case ',': addToken(COMMA); break;
            case '"': scan_string(); break;
            
            default:
                if(isDigit(c)){
                    scan_number();
                }else if(isAlpha(c)){
                    scan_identifier();
                }
                else{
                    cerr << "Unexpected character " << c << endl;
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

        // tokens.push_back(Token(EOF_VALUE, ""));
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
    
    // string filePath = "/mnt/c/Users/ankit/OneDrive/Desktop/Learn/json-parser/tests/step1/valid.json";
    // string fileContent = getFileContent(filePath);

    // run(fileContent);

    // Updated: Base path for tests directory
    string basePath = "/mnt/c/Users/ankit/OneDrive/Desktop/Learn/json-parser/tests";
    
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
    } catch (...) {
        cerr << "Unknown error during directory traversal" << endl;
    }
    return 0;
}