#ifndef __error
#define __error
typedef enum {
  /* No error, everything OK. */
  RS_OK                   = 0,
  
  /* Cannot allocate admin TCP server */
  RS_ADMIN_ALLOC          = 11,
  
  /* cannot open admin TCP server for listening */
  RS_ADMIN_OPEN           = 12,

  /* cannot create tcp_pcb for admin server */
  RS_ADMIN_TCP_PCB_CREATE = 13,

  /* cannot bind tcp_pcb for admin server to tcp port */
  RS_ADMIN_TCP_PCB_BIND   = 14,

  /* admin server failed to listen */
  RS_ADMIN_TCP_LISTEN     = 15,
} err_enum_rs_;

typedef err_enum_rs_ err;
#endif