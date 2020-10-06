#ifndef DISTRIBUTION_SERVICE_WORKER_H
#define DISTRIBUTION_SERVICE_WORKER_H

#include <mira.h>
#include "trickle_timer.h"

typedef void (*distribution_service_worker_callback_t)(
    uint32_t data_id,
    void *data,
    uint32_t size,
    const mira_net_udp_callback_metadata_t *metadata,
    void *storage);

typedef struct distribution_service_worker {
    struct distribution_service_worker *next;

    uint32_t id;
    uint32_t version;
    uint8_t rate;

    void *data;
    uint32_t size;

    struct trickle_timer timer;

    distribution_service_worker_callback_t update_handler;
    void *storage;
} distribution_service_worker_t;

int distribution_service_worker_register(
    distribution_service_worker_t *ctx,
    uint32_t id,
    void *data,
    uint32_t size,
    uint8_t rate,
    distribution_service_worker_callback_t update_handler,
    void *storage);

int distribution_service_worker_update(
    distribution_service_worker_t *ctx,
    void *data,
    uint32_t size);

int distribution_service_worker_pause(
    distribution_service_worker_t *ctx);

int distribution_service_worker_resume(
    distribution_service_worker_t *ctx);

#endif
