#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "mongoose.h"

static int s_signo;

void sighandler(int signum)
{
    s_signo = signum;
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
    const char* s_listening_address = "0.0.0.0:8080";
    struct mg_mgr mgr;
    struct mg_connection *c;

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    
    mg_log_set(4);
    mg_mgr_init(&mgr);
    if ((c = mg_http_listen(&mgr, s_listening_address, fn, &mgr)) == NULL)
    {
        MG_ERROR(("Cannot listen on %s. Use http://ADDR:PORT or :PORT",
                       s_listening_address));
        exit(EXIT_FAILURE);
    }
    c->is_hexdumping = 1;
    while (s_signo == 0)
        mg_mgr_poll(&mgr, 1000); // Block forever
    mg_mgr_free(&mgr);
    MG_INFO(("Exiting on signal %d", s_signo));
    return 0;
}