#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "cJSON.h"

static char *log_tag = NULL;
static httpd_config_t *server_config = NULL;
static httpd_handle_t server = NULL;

esp_err_t http_server_handle(httpd_method_t method, const char *uri, esp_err_t (*handler)(httpd_req_t *r)) {
    httpd_uri_t h = {
        .uri = uri,
        .method = method,
        .handler = handler,
    };

    esp_err_t err = httpd_register_uri_handler(server, &h);
    if (err == ESP_OK) {
        ESP_LOGI(log_tag, "URI handler registered: %s %s", http_method_str(method), uri);
    } else {
        ESP_LOGE(log_tag, "URI handler not registered (error %d): %s %s", err, http_method_str(method), uri);
    }

    return err;
}

esp_err_t http_server_send_response_json(httpd_req_t *req, uint16_t status, cJSON *json) {
    esp_err_t err;
    char *resp_str = NULL;

    switch (status) {
        default:
            httpd_resp_set_status(req, "200 OK");
            break;
    }

    err = httpd_resp_set_type(req, "application/json");
    if (err != ESP_OK) {
        return err;
    }

    resp_str = cJSON_PrintUnformatted(json);
    err = httpd_resp_send(req, resp_str, strlen(resp_str));
    free(resp_str);

    return err;
}

esp_err_t http_server_stop() {
    return httpd_stop(server);
}

esp_err_t http_server_start(const char *app_name, httpd_config_t *config) {
    log_tag = malloc(strlen(app_name) + strlen("_http_server") + 1);
    if (!log_tag) {
        ESP_LOGE("aespl_settings_init", "malloc() error");
        return ESP_ERR_NO_MEM;
    }
    strcpy(log_tag, app_name);
    strcat(log_tag, "_http_server");

    if (!config) {
        httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
        config = &cfg;
    }

    server_config = config;

    esp_err_t err = httpd_start(&server, server_config);
    if (err == ESP_OK) {
        ESP_LOGI(log_tag, "Server started on port %d", server_config->server_port);
    } else {
        ESP_LOGE(log_tag, "Error starting server: %d", err);
    }

    return ESP_OK;
}

