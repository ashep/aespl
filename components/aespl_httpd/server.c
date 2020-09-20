#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "aespl_httpd.h"

static char *log_tag = "aespl_http_server";

static httpd_config_t *server_config = NULL;
static httpd_handle_t server = NULL;

esp_err_t aespl_httpd_handle(httpd_method_t method, const char *uri, esp_err_t (*handler)(httpd_req_t *r)) {
    if (!server) {
        ESP_LOGE(log_tag, "server is not started");
        return AESPL_ERR_HTTPD_NOT_STARTED;
    }

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

esp_err_t aespl_httpd_send(httpd_req_t *req, const char *status, const char *body) {
    if (!server) {
        ESP_LOGE(log_tag, "server is not started");
        return AESPL_ERR_HTTPD_NOT_STARTED;
    }

    esp_err_t err = httpd_resp_send(req, body, strlen(body));

    if (err != ESP_OK) {
        ESP_LOGE(log_tag, "unable to send response");
        return err;
    } else {
        ESP_LOGI(log_tag, "%s %s %s", http_method_str(req->method), req->uri, status);
    }

    return err;
}

esp_err_t aespl_httpd_send_json(httpd_req_t *req, const char *status, cJSON *json) {
    if (!server) {
        ESP_LOGE(log_tag, "server is not started");
        return AESPL_ERR_HTTPD_NOT_STARTED;
    }

    esp_err_t err = httpd_resp_set_type(req, HTTPD_TYPE_JSON);
    if (err != ESP_OK) {
        ESP_LOGE(log_tag, "unable to set response content type");
        return err;
    }

    err = httpd_resp_set_status(req, status);
    if (err != ESP_OK) {
        ESP_LOGE(log_tag, "unable to set response status");
        return err;
    }

    char *body = cJSON_PrintUnformatted(json);

    err = aespl_httpd_send(req, status, body);
    free(body);

    return err;
}

esp_err_t aespl_httpd_stop() {
    if (!server) {
        ESP_LOGE(log_tag, "server is not started");
        return AESPL_ERR_HTTPD_NOT_STARTED;
    }

    esp_err_t err = httpd_stop(server);
    if (err) {
        return err;
    }

    server = NULL;
    server_config = NULL;

    return ESP_OK;
}

esp_err_t aespl_httpd_start(const char *app_name, httpd_config_t *config) {
    if (server) {
        ESP_LOGE(log_tag, "server is already started");
        return AESPL_ERR_HTTPD_ALREADY_STARTED;
    }

    if (!config) {
        httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
        config = &cfg;
    }

    server_config = config;

    esp_err_t err = httpd_start(&server, server_config);
    if (err == ESP_OK) {
        ESP_LOGI(log_tag, "server started on port %d", server_config->server_port);
    } else {
        ESP_LOGE(log_tag, "error starting server: %d", err);
    }

    return ESP_OK;
}
