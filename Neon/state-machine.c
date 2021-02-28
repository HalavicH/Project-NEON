#include "state-machine.h"

static volatile state_t current_state = READ_FRAME;

static inline
int is_valid_state(state_t state)
{
    switch (state) {
    case READ_FRAME:
    case PROCESS_FRAME:
    case WRITE_FRAME:
        return 1;
    default:
        return 0;
    }
}

int set_state(state_t new_state)
{
    if (!is_valid_state(new_state)) {
        return INVALID_STATE;
    }

    if (new_state == current_state) {
        return STATE_ALREADY_SET;
    }

    /* TODO: Add sequence check */

    current_state = new_state;

    return STATE_SET_SUCCESS;
}

int check_state(state_t asserted_state)
{
    if (!is_valid_state(asserted_state)) {
        return INVALID_STATE;
    }

    if (asserted_state != current_state) {
        return STATE_NOT_MATCH;
    }

    return STATE_MATCH;
}

