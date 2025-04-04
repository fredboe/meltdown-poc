#include "meltdown.h"
#include "fr.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void flush_reload_demo() {
    FlushReload channel = fr_init();
    fr_reset(&channel);

    uint8_t secret = 42;
    fr_leak(&channel, secret);
    uint8_t guessed_secret = fr_get(&channel);

    printf("What was the guess after the leakage of %d? %d\n", secret, guessed_secret);
    printf("The retrieval worked? %d\n", secret == guessed_secret);

    fr_free(&channel);
}

void meltdown_demo() {
    MeltdownUS meltdown_attack = meltdown_init();

    uint8_t secret = 42;
    uint8_t read_secret = meltdown_read_any(&meltdown_attack, &secret);

    printf("The result of the meltdown attack was %d and %d was expected.\n", secret, read_secret);

    meltdown_free(&meltdown_attack);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "flush+reload") == 0) {
        flush_reload_demo();
    } else if (argc > 1 && strcmp(argv[1], "meltdown") == 0) {
        meltdown_demo();
    } else {
        printf("Either input 'flush+reload' or 'meltdown' to see a demo.\n");
    }
}
