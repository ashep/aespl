#include "esp_http_server.h"
#include "cJSON.h"

cJSON *http_util_parse_req_body_json(httpd_req_t *req) {
    int body_len = 0;
    char *body = malloc(req->content_len);
    cJSON *json = NULL;

    body_len = httpd_req_recv(req, body, req->content_len);
    if (body_len > 0)
        json = cJSON_Parse(body);

    free(body);

    return json;
}
