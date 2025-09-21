#include <bits/stdc++.h>

using namespace std;

// Function to count bytes from an input stream
int countBytesInput(istream &in) {
    int byte_count = 0;
    char ch;
    while (in.get(ch)) {
        ++byte_count;
    }
    in.clear();
    return byte_count;
}

// Function to count bytes from a file using seekg/tellg
int countBytesFile(ifstream &file) {
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.clear();           
    file.seekg(0, ios::beg);
    return size;
}

// Function to count lines from an input stream
int countLines(istream &in) {
    int line_cnt = 0;
    char ch;
    bool last_char_newline = false;

    // Check if the stream is empty, if so, return 0 lines
    if (in.peek() == EOF) {
        in.clear();
        return 0;
    }

    while (in.get(ch)) {
        if (ch == '\n') {
            line_cnt++;
            last_char_newline = true;
        } else {
            last_char_newline = false;
        }
    }

    // If the file is not empty and the last character wasn't a newline, count the last line
    if (!last_char_newline && in.gcount() > 0) {
        line_cnt++;
    }

    in.clear(); 
    return line_cnt;
}

// Function to count words from an input stream
int countWords(istream &in) {
    int word_cnt = 0;
    char ch;
    bool in_word = false;

    while (in.get(ch)) {
        if (std::isspace(ch)) {
            in_word = false;
        } else if (!in_word) {
            ++word_cnt;
            in_word = true;
        }
    }
    in.clear();
    return word_cnt;
}

int main(int argc, char *argv[]) {
    istream *input = nullptr;
    ifstream file;

    if (argc == 1) { // No arguments, read from stdin for all three counts
        input = &cin;

        stringstream ss;
        char ch;
        while (cin.get(ch)) {
            ss.put(ch);
        }
        cout << countBytesInput(ss) << " " << countWords(ss) << " "
             << countLines(ss) << endl;

    } else if (argc >= 2) {
        string argument = argv[1];

        if (argument == "-c" || argument == "-w" ||
            argument == "-l") { // Flag provided
            string flag = argv[1];

            if (argc == 3) { // Flag and file path provided
                string filePath = argv[2];
                file.open(filePath, ios::binary);
                if (!file.is_open()) {
                    cerr << "Error: cannot open file " << filePath << endl;
                    return 1;
                }

                
                if (flag == "-c") {
                    cout << countBytesFile(file) << " " << filePath << endl;
                } else if (flag == "-w") {
                    cout << countWords(file) << " " << filePath << endl;
                } else if (flag == "-l") {
                    cout << countLines(file) << " " << filePath << endl;
                }
                file.close(); // Close the file
            } else {
                // Flag provided, but no file path, read from stdin
                stringstream ss;
                char ch;
                while (cin.get(ch)) {
                    ss.put(ch);
                }

                if (flag == "-c") {
                    cout << countBytesInput(ss) << endl;
                } else if (flag == "-w") {
                    cout << countWords(ss) << endl;
                } else if (flag == "-l") {
                    cout << countLines(ss) << endl;
                }
            }
        } else { // No flag, just a file path
            string filePath = argv[1];

            file.open(filePath, ios::binary);
            if (!file.is_open()) {
                cerr << "Error: cannot open file " << filePath << endl;
                return 1;
            }
            cout << countBytesFile(file) << " " << countWords(file) << " "
                 << countLines(file) << " " << filePath << endl;
            file.close(); // Close the file
        }
    }

    return 0;
}