#include "admin_console.h"
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "error.h"
#include "blink.h"

#define ADMIN_TCP_PORT 4242
#define ADMIN_BUF_SIZE 2048
#define ADMIN_POLL_TIME_S 5

#define HTTP_RESPONSE_HEADERS "HTTP/1.1 %d OK\nContent-Length: %d\nContent-Type: text/html; charset=utf-8\nConnection: close\n\n"
#define HTTP_INDEX_PAGE "<HTML><HEAD><TITLE>Remote Sensor</TITLE></HEAD><BODY>Hello PicoW Admin World<br />LED is: %s</BODY></HTML>"

typedef struct ADMIN_TCP_SERVER_T_ {
  struct tcp_pcb *server_pcb;
  struct tcp_pcb *client_pcb;
  bool complete;
  uint8_t buffer_headers[ADMIN_BUF_SIZE];
  uint8_t buffer_body[ADMIN_BUF_SIZE];
  uint8_t buffer_recv[ADMIN_BUF_SIZE];
  int sent_len;
  int recv_len;
  int run_count;
} ADMIN_TCP_SERVER_T;

static ADMIN_TCP_SERVER_T* admin_tcp_server_init(void) {
  ADMIN_TCP_SERVER_T *state = calloc(1, sizeof(ADMIN_TCP_SERVER_T));
  if (!state) {
    return NULL;
  }
  return state;
}

err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb) {
  ADMIN_TCP_SERVER_T *state = (ADMIN_TCP_SERVER_T*)arg;
  err_t err;

  size_t admin_page_len;
  if (led_state()) {
    admin_page_len = sprintf(state->buffer_body, HTTP_INDEX_PAGE, LED_ON);
  }else{
    admin_page_len = sprintf(state->buffer_body, HTTP_INDEX_PAGE, LED_OFF);
  }
  size_t headers_len = sprintf(state->buffer_headers, HTTP_RESPONSE_HEADERS, "200", admin_page_len);

  cyw43_arch_lwip_check();

  err = tcp_write(tpcb, state->buffer_headers, headers_len, TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK) {
    return err;
  }

  err = tcp_write(tpcb, state->buffer_body, admin_page_len, TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK) {
    return err;
  }

  err = tcp_output(tpcb);
    if (err != ERR_OK) {
    return err;
  }

  return tcp_close(tpcb);
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
  ADMIN_TCP_SERVER_T *state = (ADMIN_TCP_SERVER_T*)arg;
  if (err != ERR_OK || client_pcb == NULL) {
    return ERR_VAL;
  }

  state->client_pcb = client_pcb;
  tcp_arg(client_pcb, state);
  // tcp_sent(client_pcb, tcp_server_sent);
  // tcp_recv(client_pcb, tcp_server_recv);
  // tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
  // tcp_err(client_pcb, admin_tcp_server_err);

  return tcp_server_send_data(arg, state->client_pcb);
}

static err admin_tcp_server_open(ADMIN_TCP_SERVER_T* state) {
  struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
  if (!pcb) {
    return RS_ADMIN_TCP_PCB_CREATE;
  }

  err_t err = tcp_bind(pcb, NULL, ADMIN_TCP_PORT);
  if (err != ERR_OK) {
    return RS_ADMIN_TCP_PCB_BIND;
  }

  state->server_pcb = tcp_listen_with_backlog(pcb, 1);
  if (!state->server_pcb) {
    if (pcb) {
        tcp_close(pcb);
    }
    return RS_ADMIN_TCP_LISTEN;
  }

  tcp_arg(state->server_pcb, state);
  tcp_accept(state->server_pcb, tcp_server_accept);

  return RS_OK;
}

err run_admin_server(void) {
  ADMIN_TCP_SERVER_T *state = admin_tcp_server_init();
  if (!state) {
    return RS_ADMIN_ALLOC;
  }

  err ret = admin_tcp_server_open(state);
  if (ret != RS_OK){
    return ret;
  }

  while (true) {
    blink(1,2000);
    sleep_ms(2000);
  }
}