#ifndef _REQUESTS_
#define _REQUESTS_
#include <string>
#include "json.hpp"

using namespace std;

using json = nlohmann::json;


// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params, string token,
                            string cookie_session);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char *content_type, string token,
                           json json_data, string cookie_session);
                           
// computes and returns a DELETE request string
char *compute_delete_request(char *host, char *url, char *query_params, string token);

#endif
