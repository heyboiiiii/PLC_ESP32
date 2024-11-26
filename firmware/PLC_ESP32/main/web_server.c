#include "esp_http_server.h"
#include <string.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h> 
#include <stdio.h>
#include "esp_log.h"
//#include "plc.c"

extern uint8_t LD_I1;
extern uint8_t LD_I2;
extern uint8_t LD_I3;
extern uint8_t LD_I4;
extern uint8_t LD_I5;
extern uint8_t LD_I6;

extern uint8_t LD_Q1;
extern uint8_t LD_Q2;
extern uint8_t LD_Q3;
extern uint8_t LD_Q4;

//#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req)
{
    /* Send a simple response */
    extern unsigned char view_start[] asm("_binary_webpage_html_start");
    extern unsigned char view_end[] asm("_binary_webpage_html_end");
    size_t view_len = view_end - view_start;
    //memcpy(viewHtml, view_start, view_len);
    /*
    char viewHtml[view_len];
    memcpy(viewHtml, view_start, view_len);
    ESP_LOGI("index", "URI: %s", req->uri);
    */
    //ESP_LOGI("index", "URI: %s", viewHtml);
    
    //const char resp[] = "hola";
    httpd_resp_send_chunk(req, (const char *)view_start, view_len);
    return ESP_OK;
}

esp_err_t Iodata(httpd_req_t *req)
{
    //update_values(LD_I1,LD_I2,LD_I3,LD_I4,LD_I5,LD_I6,LD_Q1,LD_Q2,LD_Q3,LD_Q4);
    //extern unsigned char view_start[] asm("_binary_Iodata_json_start");
    //extern unsigned char view_end[] asm("_binary_Iodata_json_end");
    //size_t view_len = view_end - view_start;
    char viewJson[] = "{ \"I1\":%d,\"I2\":%d,\"I3\":%d,\"I4\":%d,\"I5\":%d,\"I6\":%d,\"O1\":%d,\"O2\":%d,\"O3\":%d,\"O4\":%d}";
    //memcpy(viewJson, view_start, view_len);


    char *viewJsonUpdated;
    asprintf(&viewJsonUpdated, viewJson, LD_I1,LD_I2,LD_I3,LD_I4,LD_I5,LD_I6,LD_Q1,LD_Q2,LD_Q3,LD_Q4);

    ESP_LOGI("json", "%s", viewJsonUpdated);
    httpd_resp_set_type(req,"application/json");
    httpd_resp_sendstr(req, viewJsonUpdated);
    free(viewJsonUpdated);
    return ESP_OK;
}




/* Our URI handler function to be called during POST /uri request */
esp_err_t post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_infovalues = {
    .uri      = "/info_values",
    .method   = HTTP_GET,
    .handler  = Iodata,
    .user_ctx = NULL
};



/* URI handler structure for POST /uri */
httpd_uri_t uri_post = {
    .uri      = "/uri",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_infovalues);

        httpd_register_uri_handler(server, &uri_post);

    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}


















