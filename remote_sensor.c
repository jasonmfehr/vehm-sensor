#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "blink.h"
#include "admin_console.h"

#define POLL_TIME_S 2

#define BUF_SIZE 2048

#define WIFI_SSID "eclipse2"
#define WIFI_PASSWORD "jasonanddebbie07282007@normalil"

#define TEST_TCP_SERVER_IP "192.168.0.103"
#define TCP_PORT 8000

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    uint8_t buffer[BUF_SIZE];
    int buffer_len;
    int sent_len;
    bool complete;
    bool connected;
} TCP_CLIENT_T;

// callback functions
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;

    if (err != ERR_OK) {
      blink_error(7);
    }else{
      state->connected = true;
    }

    return err;
}

// init functions
static TCP_CLIENT_T* tcp_client_init(void) {
    TCP_CLIENT_T *state = calloc(1, sizeof(TCP_CLIENT_T));
    if (!state) {
        blink_error(6);
        return NULL;
    }
    ip4addr_aton(TEST_TCP_SERVER_IP, &state->remote_addr);
    return state;
}

static void tcp_client_err(void *arg, err_t err) {
  if (err != ERR_ABRT) {
    blink_error(9);
  }
}

static bool tcp_client_open(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    state->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&state->remote_addr));
    if (!state->tcp_pcb) {
        blink_error(7);
        return false;
    }

    tcp_arg(state->tcp_pcb, state);
    // tcp_poll(state->tcp_pcb, tcp_client_poll, POLL_TIME_S * 2);
    // tcp_sent(state->tcp_pcb, tcp_client_sent);
    // tcp_recv(state->tcp_pcb, tcp_client_recv);
    tcp_err(state->tcp_pcb, tcp_client_err);

    state->buffer_len = 0;

    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(state->tcp_pcb, &state->remote_addr, TCP_PORT, tcp_client_connected);
    cyw43_arch_lwip_end();

    return err == ERR_OK;
}

int main() {
  stdio_init_all();

  if (cyw43_arch_init()) {
    blink_error(4);
    return 1;
  }

  blink(3, 250);
  sleep_ms(2000);
  
  cyw43_arch_enable_sta_mode();
  
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
    blink_error(5);
    return 2;
  }

  blink(3, 250);
  sleep_ms(2000);

  TCP_CLIENT_T *state = tcp_client_init();
  if(!tcp_client_open(state)) {
    blink_error(8);
  }

  while(!state->connected) {
    sleep_ms(1000);
    blink(1, 500);
  }

  char* data = "hello picow world";
  size_t data_len = strlen(data);
  
  err_t err = tcp_write(state->tcp_pcb, data, data_len, TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK) {
    blink_error(9);
  }
  
  if (tcp_output(state->tcp_pcb) != ERR_OK){
    blink_error(10);
  }

  blink(10,150);

  err = tcp_close(state->tcp_pcb);
  if (err != ERR_OK) {
    tcp_abort(state->tcp_pcb);
  }

  blink_error(run_admin_server());
  
  cyw43_arch_deinit();
  return 0;
}