#pragma once
#include "descriptor_data.h"

#include <sys/time.h>

#define MAX_HOSTNAME 256

void init_mother(int nPort);
void descriptor_close(descriptor_data *d, int bSendClose = static_cast<int>(TRUE));
void MplexSendSetup(descriptor_data *d);

#if defined(SUNOS4)
void bzero(char *b, int length);
#endif

auto any_event(int port, struct multi_type *m) -> int;
auto multi_any_connect(int port) -> int; /* test for event on socket port */
auto multi_any_freaky(struct multi_type *m) -> int;
auto multi_new(int mother, struct multi_type *m) -> int;
void multi_close_all();

auto any_input(int fd) -> int;  /* test for input         */
auto any_output(int fd) -> int; /* test for output        */

auto multi_process_input(struct multi_element *pm) -> int;

auto write_to_descriptor(int desc, char *txt) -> int;
auto read_from_descriptor(int desc, char *txt) -> int;
void init_char(unit_data *ch);

#define SNOOP_PROMPT "% " /* probably not very nice to have here, but hey! */
