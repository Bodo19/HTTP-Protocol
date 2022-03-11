#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"

using namespace std;

using json = nlohmann::json;
//Includ librari din C si C++ si extensia pentru json

bool check_number(string str)
{
    for (int i = 0; i < str.length(); i++)
        if (isdigit(str[i]) == false)
            return false;
    return true;
}
//In aceasta functie verific daca variabila pe care o primeste este forma integer sau string

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    string input, session_cookie, token_jtw;
    //Imi declar variabila in care imi creez mesajul pentru server(message), variabila in care
    //primesc mesajul de la server(response), variabila in care deschid socketul(sockfd) si
    //variabilele pe care le folosesc pentru a citi comenzile(input) si pentru a retine le
    //folosesc pentru a retine session_cookie si token_jtw

    while (1)
    {
        //Fac un while care merge pana primeste "exit"
        cin >> input;
        //Citesc comenzile primite
        if (input == "exit")
        {
            //Daca primesc "exit" dau break pentru a iesi din while
            break;
        }

        if (input == "register")
        {
            //Inregistrez un nou utilizator daca primesc register
            cin.ignore(); //Ma asigur ca buffer-ul este liber
            string username, password;
            cout << "username=";
            getline(cin, username, '\n');
            cout << "password=";
            getline(cin, password, '\n');
            //Citesc username-ul si parola

            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            json register_json = {
                {"username", username},
                {"password", password}};
            //Fac fisierul json conform cerintei

            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/register",
                                           "application/json", "", register_json, "");
            //Apelez cererea de post la server, in header-ul de la post este format din
            //host, url, content-type, string, json, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "login")
        {
            //Autentific un utilizator daca este deja inregistrat
            cin.ignore(); //Ma asigur ca buffer-ul este liber
            string username, password;
            cout << "username=";
            getline(cin, username, '\n');
            cout << "password=";
            getline(cin, password, '\n');
            //Citesc username-ul si parola

            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            json register_json = {
                {"username", username},
                {"password", password}};
            //Fac fisierul json conform cerintei

            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/login",
                                           "application/json", "", register_json, "");
            //Apelez cererea de post la server, in header-ul de la post este format din
            //host, url, content-type, string, json, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            string response_string(response);
            //Castez response la string

            if (response_string.find("Set-Cookie: "))
            {
                //Ma folosesc de find ca sa verific daca primesc cookie
                response_string.erase(0, response_string.find("Set-Cookie: "));
                //Dau erase pana la "Set-Cookie: "de la inceputul stringului
                response_string.erase(0, strlen("Set-Cookie: "));
                //Dau erase la "Set-Cookie" pentru a ajunge la cookie-ul primit de sesiune
                response_string = response_string.substr(0, response_string.find(';'));
                //Substrag pana la ";" care este pus la finalul cookieului
                session_cookie = response_string;
                //Salvez in cookie ul sesiunii cookie ul gasit
            }

            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "enter_library")
        {
            //Intru in librarie pentru a obtine tokenul de a lucra cu books
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access",
                                          NULL, token_jtw, session_cookie);
            //Apelez cererea de get la server, in header-ul de la get este format din
            //host, url, query_params, string, string
            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            string json_string = basic_extract_json_response(response);
            json json_val = json::parse(json_string);
            //Parsez stringul extras din response

            if (json_val.dump().find("token"))
            //Dau dump pentru a obtine un string apoi daau find ca sa verific daca am token
            {
                //Construiesc token-ul cu prefixul "Bearer" urmat de valoare lui token sau tokenUndefined din json.
                token_jtw = "Bearer " + json_val.value("token", "TokenUndefined");
            }

            //cout << token_jtw << "\n";

            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "get_books")
        {
            //Primesc cuprinsul din librarie
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books",
                                          NULL, token_jtw, session_cookie);
            //Apelez cererea de get la server, in header-ul de la get este format din
            //host, url, query_params, string, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            char *json_response = basic_extract_json_response(response);
            //Extrag din response lista de carti

            if (!json_response)
            {
                //Daca e goala afisez eroarea
                cout << "No books found in your library.\n";
            }

            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "get_book")
        {
            //Primesc informatii despre o carte anume in functie de id-ul primit
            string id;
            cout << "id=";
            cin >> id;
            //Citesc id-ul

            if (!check_number(id))
            {
                cout << "It's not an int.\n";
                continue;
            }
            //Verific daca este litera sau cifra, daca e litera dau eroare si ies din comanda
            //dar raman in while, daca e cifra merg mai departe

            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            string book_route_id = "/api/v1/tema/library/books/" + id;
            char *book_route = (char *)calloc(book_route_id.length() + 1, sizeof(char));
            strcpy(book_route, book_route_id.c_str());
            //Adaug id-ul primit la url folosind un char si un string

            message = compute_get_request("34.118.48.238", book_route, NULL, token_jtw, session_cookie);
            //Apelez cererea de get la server, in header-ul de la get este format din
            //host, url, query_params, string, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            free(book_route);
            //Eliberez memoria
            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "add_book")
        {
            string title, author, genre, publisher, page_count;
            int page_count_int;
            cin.ignore();
            cout << "titel=";
            getline(cin, title, '\n');
            cout << "author=";
            getline(cin, author, '\n');
            cout << "genre=";
            getline(cin, genre, '\n');
            cout << "publisher=";
            getline(cin, publisher, '\n');
            cout << "page_count= ";
            cin >> page_count;
            //Citesc datele pe care le primesc de la tastatura

            page_count_int = stoi(page_count);
            //Convertesc numarul de pagini la int

            if (page_count_int < 1)
            {
                cout << "The page count should be positive.\n";
                continue;
            }
            //Verific daca numarul de pagini este peste 0, daca nu ies din instructiune,
            //daca da, merg mai departe

            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            json new_book_json = {
                {"title", title},
                {"author", author},
                {"genre", genre},
                {"publisher", publisher},
                {"page_count", page_count_int}};
            //Fac fisierul json conform cerintei

            message = compute_post_request("34.118.48.238", "/api/v1/tema/library/books", "application/json",
                                           token_jtw, new_book_json, session_cookie);
            //Apelez cererea de post la server, in header-ul de la post este format din
            //host, url, content-type, string, json, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "delete_book")
        {
            //Sterg o carte anume in functie de id-ul primit
            string id;
            cout << "id=";
            cin >> id;
            //Citesc id-ul

            if (!check_number(id))
            {
                cout << "It's not an int.\n";
                continue;
            }
            //Verific daca este litera sau cifra, daca e litera dau eroare si ies din comanda
            //dar raman in while, daca e cifra merg mai departe

            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            string book_route_id = "/api/v1/tema/library/books/" + id;
            char *book_route = (char *)calloc(book_route_id.length() + 1, sizeof(char));
            strcpy(book_route, book_route_id.c_str());
            //Adaug id-ul primit la url folosind un char si un string

            message = compute_delete_request("34.118.48.238", book_route, NULL, token_jtw);
            //Apelez cererea de delete la server, in header-ul de la delete este format din
            //host, url, query_params, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            free(book_route);
            //Elibere memoria
            close_connection(sockfd);
            //Inchid socketul
        }
        if (input == "logout")
        {

            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            //Deschid socket-ul pe ip-ul local

            message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout", NULL, token_jtw, session_cookie);
            //Apelez cererea de get la server, in header-ul de la get este format din
            //host, url, query_params, string, string

            puts(message);
            //Afisez mesajul pe care il trimit serverului

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //Trimit mesajul serverului si apoi astept raspunsul

            puts(response);
            //Afisez raspunsul

            token_jtw = "";
            session_cookie = "";
            //Eliberez token-ul si cookie-ul

            close_connection(sockfd);
            //Inchid socketul
        }
    }
    return 0;
}
