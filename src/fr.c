#include "fr.h"

#include <xmmintrin.h>
#include <stdlib.h>

/// Utils
void flush(const uint8_t *addr) {
    _mm_mfence();
    _mm_clflush(addr);
    _mm_mfence();
}

void access_unserialized(const uint8_t *addr) {
    volatile uint8_t tmp = *(volatile uint8_t *) addr;
}

void access_serialized(const uint8_t *addr) {
    volatile uint8_t tmp = *(volatile uint8_t *) addr;
    _mm_mfence();
}


uint64_t get_time() {
    uint32_t hi, lo, aux;
    __asm__ __volatile__ (
        "rdtscp"
        : "=a"(lo), "=d"(hi), "=c"(aux)
        :
        :
    );
    return ((uint64_t) hi << 32) | lo;
}

uint64_t measure_access_time(const uint8_t *addr) {
    uint64_t start = get_time();
    access_serialized(addr);
    uint64_t end = get_time();
    return end - start;
}


/// Flush+Reload implementation
FlushReload fr_init(void) {
    uint8_t *buffer = malloc(N_BYTE_VALUES * PAGE_SIZE * sizeof(uint8_t));
    for (int i = 0; i < N_BYTE_VALUES * PAGE_SIZE; i++) buffer[i] = 219;
    return (FlushReload){._buffer = buffer};
}

void fr_free(FlushReload *fr) {
    free(fr->_buffer);
    fr->_buffer = NULL;
}

void fr_reset(const FlushReload *fr) {
    for (int page_start = 0; page_start < N_BYTE_VALUES * PAGE_SIZE; page_start += PAGE_SIZE) {
        flush(fr->_buffer + page_start);
    }
}

void fr_leak(const FlushReload *fr, const uint8_t secret) {
    access_unserialized(fr->_buffer + secret * PAGE_SIZE);
}

uint8_t fr_get(const FlushReload *fr) {
    uint8_t byte_index_with_min_access_time = 0;
    uint64_t min_access_time = 1 << 31;

    for (int i = 0; i < N_BYTE_VALUES; i++) {
        const uint8_t byte_idx = (113 * i + 1) % N_BYTE_VALUES; // pseudo-randomness

        const uint64_t access_time = measure_access_time(fr->_buffer + byte_idx * PAGE_SIZE);
        if (access_time < min_access_time) {
            min_access_time = access_time;
            byte_index_with_min_access_time = byte_idx;
        }
    }

    return byte_index_with_min_access_time;
}
