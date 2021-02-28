#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

/*
   States:
   READ_FRAME
   PROCESS_FRAME
   WRITE_FRAME
 */

typedef enum state_enum {
    READ_FRAME,
    PROCESS_FRAME,
    WRITE_FRAME,
} state_t;

typedef enum set_state_response {
    STATE_SET_SUCCESS = 0,
    STATE_ALREADY_SET,
    STATE_SEQUENCE_ERROR,
    INVALID_STATE,
} set_state_response_t;

typedef enum check_state_response {
    STATE_MATCH = 0,
    STATE_NOT_MATCH
} check_state_response_t;

int set_state(state_t new_state);
int check_state(state_t asserted_state);

#endif /* STATE_MACHINE_H */

