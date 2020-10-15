/**
 * @brief     AESPL HTTP Client
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/socket.h>
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "http_parser.h"
#include "http_header.h"
#include "aespl_http_client.h"

#define HTTP_VERSION "HTTP/1.0"
#define USER_AGENT "aespl/1.0"

static const char *LOG_TAG = "aespl_http_client";

static int http_parser_on_headers_complete(http_parser *parser) {
    aespl_http_response *response = (aespl_http_response *)parser->data;

    if (response->tmp_header_field) {
        free(response->tmp_header_field);
        response->tmp_header_field = NULL;
    }

    if (response->tmp_header_value) {
        free(response->tmp_header_value);
        response->tmp_header_value = NULL;
    }

    return 0;
}

static int http_parser_on_header_field(http_parser *parser, const char *at, size_t length) {
    aespl_http_response *response = (aespl_http_response *)parser->data;

    if (response->tmp_header_field) {
        free(response->tmp_header_field);
    }

    response->tmp_header_field = malloc(length + 1);
    bzero(response->tmp_header_field, length + 1);
    strncpy(response->tmp_header_field, at, length);

    return 0;
}

static int http_parser_on_header_value(http_parser *parser, const char *at, size_t length) {
    aespl_http_response *response = (aespl_http_response *)parser->data;

    if (!response->tmp_header_field) {
        return 0;
    }

    if (response->tmp_header_value) {
        free(response->tmp_header_value);
    }

    response->tmp_header_value = malloc(length + 1);
    bzero(response->tmp_header_value, length + 1);
    strncpy(response->tmp_header_value, at, length);

    http_header_set(response->headers, response->tmp_header_field, response->tmp_header_value);

    return 0;
}

static int http_parser_on_body(http_parser *parser, const char *at, size_t length) {
    aespl_http_response *response = (aespl_http_response *)parser->data;

    response->body = malloc(length + 1);
    bzero(response->body, length + 1);
    strncpy(response->body, at, length);

    return 0;
}

esp_err_t aespl_http_client_request(aespl_http_response *response, enum http_method method, const char *url,
                                    http_header_handle_t headers, const char *body) {
    int err_i;

    // Parse the URL
    struct http_parser_url parsed_url;
    http_parser_url_init(&parsed_url);
    err_i = http_parser_parse_url(url, strlen(url), false, &parsed_url);
    if (err_i) {
        ESP_LOGE("Error parsing URL: %s", url);
        return ESP_FAIL;
    }

    // Parse hostname
    if (!(parsed_url.field_set & (1 << UF_HOST))) {
        ESP_LOGE("Host not found in URL: %s", url);
        return ESP_FAIL;
    }
    char *host = malloc(parsed_url.field_data[UF_HOST].len + 1);
    bzero(host, parsed_url.field_data[UF_HOST].len + 1);
    strncpy(host, url + parsed_url.field_data[UF_HOST].off, parsed_url.field_data[UF_HOST].len);

    // Parse port
    char *port;
    if (parsed_url.field_set & (1 << UF_PORT)) {
        port = malloc(parsed_url.field_data[UF_PORT].len + 1);
        bzero(port, parsed_url.field_data[UF_PORT].len + 1);
        strncpy(port, url + parsed_url.field_data[UF_PORT].off, parsed_url.field_data[UF_PORT].len);
    } else {
        port = malloc(3);
        strcpy(port, "80");
    }

    // Resolve address
    struct addrinfo *addr_info;
    const struct addrinfo addr_info_hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    err_i = getaddrinfo(host, port, &addr_info_hints, &addr_info);
    if (err_i != 0 || addr_info == NULL) {
        ESP_LOGE(LOG_TAG, "getaddrinfo(\"%s\", \"%s\") failed, error %d", host, port, err_i);
        free(host);
        free(port);
        return ESP_FAIL;
    }
    struct in_addr *addr = &((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
    ESP_LOGD(LOG_TAG, "IP address of \"%s\": %s", host, inet_ntoa(*addr));

    // Allocate socket
    int sock = socket(addr_info->ai_family, addr_info->ai_socktype, 0);
    if (sock < 0) {
        ESP_LOGE(LOG_TAG, "Failed to allocate socket");
        freeaddrinfo(addr_info);
        free(host);
        free(port);
        return ESP_FAIL;
    }

    // Open connection
    err_i = connect(sock, addr_info->ai_addr, addr_info->ai_addrlen);
    if (err_i != 0) {
        ESP_LOGE(LOG_TAG, "Socket connect failed, error %d", errno);
        close(sock);
        freeaddrinfo(addr_info);
        free(host);
        free(port);
        return ESP_FAIL;
    }
    freeaddrinfo(addr_info);

    // Parse path
    char *path;
    if (parsed_url.field_set & (1 << UF_PATH)) {
        path = malloc(parsed_url.field_data[UF_PATH].len + 1);
        bzero(path, parsed_url.field_data[UF_PATH].len + 1);
        strncpy(path, url + parsed_url.field_data[UF_PATH].off, parsed_url.field_data[UF_PATH].len);
    } else {
        path = malloc(2);
        strcpy(path, "/");
    }

    // Parse query
    char *query;
    if (parsed_url.field_set & (1 << UF_QUERY)) {
        query = malloc(parsed_url.field_data[UF_QUERY].len + 1);
        bzero(query, parsed_url.field_data[UF_QUERY].len + 1);
        strncpy(query, url + parsed_url.field_data[UF_QUERY].off, parsed_url.field_data[UF_QUERY].len);
    } else {
        query = malloc(1);
        strcpy(query, "");
    }

    // Build headers string
    char *headers_str = NULL;
    int headers_len = 512;
    if (headers) {
        headers_str = malloc(headers_len);
        http_header_generate_string(headers, 0, headers_str, &headers_len);
    }

    // Calculate request length
    uint16_t req_len = strlen(http_method_str(method)) + 1 + strlen(path) + 1 + strlen(HTTP_VERSION) + 2;
    if (query) {
        req_len += 1 + strlen(query);
    }
    req_len += strlen("Host: ") + strlen(host) + 2;
    req_len += strlen("User-Agent: ") + strlen(USER_AGENT) + 2;
    if (headers_str) {
        req_len += headers_len;
    }
    req_len += 2;  // "\r\n"
    if (method != HTTP_GET && body) {
        req_len += strlen(body);
    }
    req_len += 1;  // '\0'

    // Build request string
    char *req = malloc(req_len);
    char *offset = req;
    offset += sprintf(offset, "%s %s", http_method_str(method), path);
    if (strlen(query)) {
        offset += sprintf(offset, "?%s", query);
    }
    offset += sprintf(offset, " %s\r\n", HTTP_VERSION);
    offset += sprintf(offset, "Host: %s\r\n", host);
    offset += sprintf(offset, "User-Agent: %s\r\n", USER_AGENT);
    if (headers_str) {
        offset += sprintf(offset, "%s", headers_str);
    }
    offset += sprintf(offset, "\r\n");
    if (method != HTTP_GET && body) {
        strcat(req, body);
    }

    // Free URL
    free(host);
    free(port);
    free(path);
    free(query);
    free(headers_str);

    // Send request
    err_i = write(sock, req, req_len);
    if (err_i < 0) {
        ESP_LOGE(LOG_TAG, "Socket write failed");
        free(req);
        close(sock);
        return ESP_FAIL;
    }
    free(req);

    // Set socket receive timeout
    struct timeval rcv_timeout = {
        .tv_sec = 5,
        .tv_usec = 0,
    };
    err_i = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout));
    if (err_i < 0) {
        ESP_LOGE(LOG_TAG, "Failed to set socket receiving timeout");
        close(sock);
        return ESP_FAIL;
    }

    // Read HTTP response
    size_t resp_len = 0;
    char *resp_str = malloc(1);
    resp_str[0] = 0;
    ssize_t bytes_read;
    do {
        // Read a piece of data
        char buf[64];
        bytes_read = read(sock, buf, sizeof(buf) - 1);
        if (bytes_read < 0) {
            ESP_LOGW(LOG_TAG, "Error while reading from the socket: %d", bytes_read);
            free(resp_str);
            close(sock);
            return ESP_FAIL;
        }

        // Concatenate read bytes
        if (bytes_read > 0) {
            resp_len += bytes_read + 1;
            resp_str = realloc(resp_str, resp_len);
            if (!resp_str) {
                ESP_LOGE(LOG_TAG, "Error while allocating memory for the response");
                free(resp_str);
                close(sock);
                return ESP_ERR_NO_MEM;
            }
            buf[bytes_read] = 0;  // ensure end of the string marker
            strcat(resp_str, buf);
        }
    } while (bytes_read > 0);

    // Free socket
    close(sock);

    // Setup HTTP parser
    http_parser parser = {
        .data = (void *)response,
    };
    http_parser_settings parser_settings;
    http_parser_init(&parser, HTTP_RESPONSE);
    http_parser_settings_init(&parser_settings);

    // Setup parser callbacks
    parser_settings.on_header_field = &http_parser_on_header_field;
    parser_settings.on_header_value = &http_parser_on_header_value;
    parser_settings.on_headers_complete = &http_parser_on_headers_complete;
    parser_settings.on_body = &http_parser_on_body;

    // Initialize response struct
    memset(response, 0, sizeof(*response));
    response->headers = http_header_init();

    // Parse the response
    http_parser_execute(&parser, &parser_settings, resp_str, resp_len);

    // Free response string
    free(resp_str);

    response->status_code = parser.status_code;
    response->content_length = parser.content_length;

    return ESP_OK;
}

esp_err_t aespl_http_client_get(aespl_http_response *response, const char *url, http_header_handle_t headers) {
    return aespl_http_client_request(response, HTTP_GET, url, headers, NULL);
}

esp_err_t aespl_http_client_get_json(aespl_http_response *response, const char *url, http_header_handle_t headers) {
    esp_err_t err;
    bool own_headers = false;

    if (!headers) {
        own_headers = true;
        headers = http_header_init();
    }

    http_header_set(headers, "Accept", "application/json");

    err = aespl_http_client_get(response, url, headers);
    if (!err && response->status_code >= 200 && response->status_code < 300) {
        response->json = cJSON_Parse(response->body);
    }

    if (own_headers) {
        http_header_destroy(headers);
    }

    return err;
}

void aespl_http_client_free(aespl_http_response *response) {
    if (response->body) {
        free(response->body);
    }

    if (response->tmp_header_field) {
        free(response->tmp_header_field);
        response->tmp_header_field = NULL;
    }

    if (response->tmp_header_value) {
        free(response->tmp_header_value);
        response->tmp_header_value = NULL;
    }

    if (response->headers) {
        http_header_destroy(response->headers);
        response->headers = NULL;
    }

    if (response->json) {
        cJSON_Delete(response->json);
        response->json = NULL;
    }
}
