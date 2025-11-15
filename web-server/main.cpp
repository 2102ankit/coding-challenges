#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace filesystem;

string resolve_path(const string &uri)
{
    path root = current_path();
    path full = root / uri.substr(1); // -> "/img/pic.png" → "img/pic.png"

    if (full.lexically_normal().string().find(root.string()) != 0)
    {
        return "";
    }

    if (is_directory(full))
    {
        full /= "index.html";
    }
    return exists(full) ? full.string() : "";
}

string read_file(const path &p)
{
    if (!exists(p) || !is_regular_file(p))
        return {};

    ifstream f(p, ios::binary);
    if (!f.is_open())
        return {};

    string content;
    f.seekg(0, ios::end);
    content.resize(f.tellg());
    f.seekg(0, ios::beg);
    f.read(&content[0], content.size());
    return content;
}

bool send_file(int client_fd, const string &filepath, const string &content_type)
{
    string content = read_file(filepath);
    if (content.empty())
    {
        // 404 Not Found
        string not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 14\r\n"
            "\r\n"
            "File not found";
        send(client_fd, not_found.c_str(), not_found.size(), 0);
        return false;
    }

    // Build HTTP response
    string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " +
        content_type + "\r\n"
                       "Content-Length: " +
        to_string(content.size()) + "\r\n"
                                    "\r\n" +
        content;

    // Send all at once (or loop for large files)
    ssize_t sent = send(client_fd, response.c_str(), response.size(), 0);
    if (sent < 0)
    {
        perror("send");
        return false;
    }
    return true;
}

void handle_client_request(int client_fd)
{
    cout << "Thread is running with ID: " << this_thread::get_id() << "\n";
    char buffer[10240] = {0};
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    string request(buffer, n);

    // cout << request << endl;

    size_t eol = request.find("\r\n");
    if (eol == string::npos)
    {
        eol = request.find('\n');
        if (eol == string::npos)
        {
            cerr << "no line ending\n";
            return;
        }
    }

    string first_line = request.substr(0, eol);
    // cout << "first line : " << first_line << endl;

    istringstream line(first_line);
    string method, uri, version;
    if (!(line >> method >> uri >> version))
    {
        cerr << "Malformed Request" << endl;
        return;
    }

    if (method == "GET" && !uri.empty() && uri[0] == '/')
    {
        cout << "GET " << uri << endl;
    }
    else
    {
        cerr << "invalid request\n";
        return;
    }

    string response;
    string path = resolve_path(uri);
    cout << "\n\n> " << path << endl;
    if (path.empty())
    {
        string not_found = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 9\r\n\r\n"
                           "Not Found";
        send(client_fd, not_found.c_str(), not_found.size(), 0);
    }
    else
    {
        send_file(client_fd, path, "text/html");
    }

    send_file(client_fd, uri, "text/html");

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        cerr << "Socket creation failed\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    int bind_res = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_res < 0)
    {
        cerr << "Bind failed" << endl;
        return 1;
    }

    listen(server_fd, 10);

    cout << "server listening on port 9999...\n";

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        std::thread client_handler_thread(handle_client_request, client_fd);
        client_handler_thread.detach(); // Detach to allow independent execution
    }

    close(server_fd);
    return 0;
}