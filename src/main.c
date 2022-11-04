#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "mongoose.h"

void sighandler(int signum)
{
    printf("Caught signal %d, coming out...\n", signum);
    exit(0);
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    struct mg_http_serve_opts opts = {0}; // Serve local dir
    opts.root_dir = "./static";
    opts.mime_types = "wasm=application/wasm";
    if (ev == MG_EV_HTTP_MSG)
    {
        MG_INFO(("New request to: [%.*s], body size: %lu", (int)hm->uri.len,
                 hm->uri.ptr, (unsigned long)hm->body.len));
        mg_http_serve_dir(c, ev_data, &opts);
    }
}

int main(void)
{
    signal(SIGINT, sighandler);
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "0.0.0.0:8080", fn, NULL); // Create listening connection
    for (;;)
        mg_mgr_poll(&mgr, 1000); // Block forever
    return 0;
}