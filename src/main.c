#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "mongoose.h"

static int s_signo;
static char* s_static_path;

void sighandler(int signum)
{
    s_signo = signum;
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    struct mg_http_serve_opts opts = {0}; // Serve local dir
    opts.root_dir = s_static_path; //"./static";
    opts.mime_types = "wasm=application/wasm";
    opts.extra_headers = "Access-Control-Allow-Origin: *\r\n";
    if (ev == MG_EV_HTTP_MSG)
    {
        MG_INFO(("New request to: [%.*s], body size: %lu", (int)hm->uri.len,
                 hm->uri.ptr, (unsigned long)hm->body.len));
        mg_http_serve_dir(c, ev_data, &opts);
    }
}

static void usage(const char *prog) {
  fprintf(stderr, "Usage: OPTIONS\n");
  fprintf(stderr, "\t-p [port] - set port\n");
  fprintf(stderr, "\t-s [path] - set static path\n");
  fprintf(stderr, "\t-l [logging level] - set logging level\n");
  fprintf(stderr, "\t\t0 - Disable logging\n");
  fprintf(stderr, "\t\t1 - Log errors only\n");
  fprintf(stderr, "\t\t2 - Log errors and info messages\n");
  fprintf(stderr, "\t\t3 - Log errors, info and debug messages\n");
  fprintf(stderr, "\t\t4 - Log everything\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char s_listening_address[13] = "0.0.0.0:";
    struct mg_mgr mgr;
    struct mg_connection *c;
    int i;
    int logLevel = 0;
    // Parse command-line flags
    if(argc == 1)
    {
        usage(argv[0]);
    }
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            strcat(s_listening_address, argv[++i]);
        }
		else if(strcmp(argv[i], "-s") == 0)
		{
			s_static_path = argv[++i];
		}
        else if(strcmp(argv[i], "-l") == 0)
		{
			logLevel = strtol(argv[++i], NULL, 10);
		}
        else
        {
            usage(argv[0]);
        }
    }
    

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    mg_log_set(logLevel);
    mg_mgr_init(&mgr);
    if ((c = mg_http_listen(&mgr, s_listening_address, fn, &mgr)) == NULL)
    {
        MG_ERROR(("Cannot listen on %s. Use http://ADDR:PORT or :PORT",
                  s_listening_address));
        exit(EXIT_FAILURE);
    }
    c->is_hexdumping = (logLevel == 4);
    while (s_signo == 0)
        mg_mgr_poll(&mgr, 1000); // Block forever
    mg_mgr_free(&mgr);
    MG_INFO(("Exiting on signal %d", s_signo));
    return 0;
}
