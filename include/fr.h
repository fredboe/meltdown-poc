#ifndef FR_H
#define FR_H

#include "stdint.h"

#define N_BYTE_VALUES 256
#define PAGE_SIZE 4096

typedef struct {
  uint8_t *_buffer;
} FlushReload;

FlushReload fr_init(void);

void fr_free(FlushReload *fr);

void fr_reset(const FlushReload *fr);

void fr_leak(const FlushReload *fr, uint8_t secret);

uint8_t fr_get(const FlushReload *fr);


#endif //FR_H
