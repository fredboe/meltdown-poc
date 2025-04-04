//
// Created by fredboe on 04.04.25.
//

#ifndef MELTDOWN_H
#define MELTDOWN_H

#include <setjmp.h>

#include "fr.h"

typedef struct {
  FlushReload _channel;
} MeltdownUS;

MeltdownUS meltdown_init(void);

void meltdown_free(MeltdownUS *meltdown);

uint8_t meltdown_read_any(const MeltdownUS *meltdown, const uint8_t *addr);

#endif //MELTDOWN_H
