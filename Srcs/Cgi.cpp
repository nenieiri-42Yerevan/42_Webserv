#include "Cgi.hpp"
#include <fcntl.h>
#include "Client.hpp"

Cgi::Cgi(Client  *other)
{
    this->header = other->getHeader();
    this->cont = other;
    /*std::map<std::string, std::string>::iterator it = this->header.begin();
    while (it != this->header.end())
    {
        std::cout << it->first << "  " << it->second << std::endl;
        it++;
    }*/
    /*std::cout << this->cont->getBody() << std::endl;*/
}

Cgi::Cgi()
{
}

const std::string Cgi::findscript(std::string uri)
{
    size_t found;
    std::string ret;

    found = uri.find("?");
    if (found != std::string::npos)
        ret = uri.substr(0, found);
    else
        ret = uri;    
    return ret;
}

std::string Cgi::findquery(std::string uri)
{
    size_t found;

    found = uri.find("?");
    if (found != std::string::npos)
        return (uri.substr(found + 1, uri.length() - found));
    return ("");
}
void Cgi::initenv()
{
    char *pwd;

    pwd = getcwd(NULL, 0);
    env["UPLOAD_DIR"] = pwd + (std::string)"/" + this->cont->getUploadDir();
    env["CONTENT_LENGTH"] = this->header["content-length"];
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["CONTENT_TYPE"] = this->header["content-type"];
    env["PATH_INFO"] = this->cont->getFile();
    env["REQUEST_METHOD"] = this->header["method"];
    env["QUERY_STRING"] = findquery(this->header["uri"]);
    env["REMOTE_ADDR"] = this->header["host"];
    env["SCRIPT_NAME"] = findscript(this->header["uri"]);
    env["SCRIPT_FILENAME"] = std::string(pwd) + "/" + this->cont->getFile();
    env["SERVER_NAME"] = "webserv";
    env["SERVER_PORT"] = this->cont->getServerPort();
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["SERVER_SOFTWARE"] = "Webserv";
    env["REDIRECT_STATUS"] = "true";
    env["ORIGIN"] = this->header["origin"];
    if (BONUS == 1)
        env["HTTP_COOKIE"] = this->header["cookie"];
	free(pwd);
}

Cgi::Cgi(const Cgi &other)
{
    *this = other;
}

void Cgi::tofile(std::string path)
{
    std::stringstream ss;
    std::ifstream ifs(path, std::ifstream::in);
    std::string response;
    std::string str;

    ss << ifs.rdbuf();
    str = ss.str();
    ss.clear();
    ss.str("");
    std::string res = str;
    size_t pos = str.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        res = str.substr(pos + 4, str.length() - (pos + 4));
    }
    ss << res.length();
	response += "HTTP/1.1 " + (std::string)"200" + " " + "ok" + "\r\n";
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Server: webserv\r\n";
    if (this->cont->getCgi().first == "py")
        response += "Content-Type: text/html\r\n\r\n";
	response += str;
    ifs.close();
    this->cont->setResponse(response);
}

Cgi::~Cgi(){}
Cgi &Cgi::operator=(Cgi const &other)
{
    if (this != &other)
    {
        this->env = other.env;
        this->header = other.header;
        this->cont = other.cont;
    }
    return (*this);
}

void Cgi::cgi_run()
{
    int pid;
    char *args[3];
    char *envc[20];
    int i, status;

    i = 0;
    initenv();
    char *pwd = getcwd(NULL, 0);
    this->path = this->cont->getCgi().second;
    if (this->path[0] == '.' && this->path[1] == '/')
        this->path = this->path.substr(2, this->path.length() - 2);
    if (this->path[0] != '/')
        this->path = std::string(pwd) + (std::string)"/" + this->path;
    free(pwd);
    std::map<std::string, std::string>::iterator it = env.begin();
    while (it != env.end())
    {
        envc[i++] = strdup((it->first + "=" + it->second).c_str());
        it++;
    }
    envc[i] = NULL;
    int fd = open("temp", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
    pid = fork();
    if (pid == 0)
    {
        dup2(fd, 1);
        close(fd);
        if (this->header["method"] == "POST")
        {
            int pipefd[2];
            pipe(pipefd);
            dup2(pipefd[0], 0);
            write(pipefd[1], this->cont->getBody().c_str(), this->cont->getBody().length());
            close(pipefd[0]);
            close(pipefd[1]);
        }
        args[0] = strdup(this->path.c_str());
        args[1] = strdup(this->cont->getFile().c_str());
        args[2] = NULL;
        if (execve(args[0], args, envc) == -1)
        {
            free(args[0]);
            free(args[1]);
            perror("Error");
            exit(errno);
        }
    }
    if (pid < 0)
    {
        std::string response;
        response = "HTTP/1.1 500 Internal Server Error\r\n";
        response += "Content-Length : 96\r\n";
        response += "Server : webserv\r\n";
        response += "\r\n";
        response += "<html></html><head></head><body><h1><center>500<br>\
        Internal Server Error</h1></center></body></html>";
        this->cont->setResponse(response);
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
    {
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0)       
        {
            std::string response;
            response = "HTTP/1.1 500 Internal Server Error\r\n";
            response += "Content-Length : 96\r\n";
            response += "Server : webserv\r\n";
            response += "\r\n";
            response += "<html></html><head></head><body><h1><center>500<br>\
            Internal Server Error</h1></center></body></html>";
            this->cont->setResponse(response);
        }
    }
    close(fd);
    tofile("temp");
    i = 0;
    while (envc[i])
    {
        free(envc[i]);
        i++;
    }
    unlink("temp");
}
