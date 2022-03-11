#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

char *compute_get_request(char *host, char *url, char *query_params, string token,
                          string cookie_session)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "GET %s%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (!token.empty())
    {
        sprintf(line, "Authorization: %s", token.c_str());
        compute_message(message, line);
    }

    if (!cookie_session.empty())
    {
        sprintf(line, "Cookie: %s", cookie_session.c_str());
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, string token,
                           json json_data, string cookie_session)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", json_data.dump().length());
    compute_message(message, line);

    // Step 4 (optional): add cookies, add headers
    if (!token.empty())
    {
        sprintf(line, "Authorization: %s", token.c_str());
        compute_message(message, line);
    }

    if (!cookie_session.empty())
    {
        sprintf(line, "Cookie: %s", cookie_session.c_str());
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, json_data.dump().c_str());

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params, string token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "DELETE %s%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    // Step 3 (optional): add headers and/or cookies, according to the protocol format	
    if (!token.empty())
    {
        sprintf(line, "Authorization: %s", token.c_str());
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}
