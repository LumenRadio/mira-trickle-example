// #include "debug.h"
#include "mira.h"
#include "distribution_service.h"
#include "distribution_service_worker.h"


#define DISTRIBUTION_SERVICE_NUM_CTX    4

static distribution_service_worker_t distribution_service_ctx[
    DISTRIBUTION_SERVICE_NUM_CTX];
static int distribution_service_num_ctx = 0;

int distribution_service_register(
    uint32_t data_id,
    void *data,
    mira_size_t size,
    uint8_t rate,
    distribution_service_callback_t update_handler,
    void *storage)
{
    distribution_service_worker_t *ctx;
    if (distribution_service_num_ctx >= DISTRIBUTION_SERVICE_NUM_CTX) {
        return -1;
    }

    ctx = &distribution_service_ctx[distribution_service_num_ctx];
    distribution_service_num_ctx++;

    return distribution_service_worker_register(
        ctx,
        data_id,
        data,
        size,
        rate,
        update_handler,
        storage);
}

int distribution_service_update(
    uint32_t data_id,
    void *data,
    mira_size_t size)
{
    int i;
    distribution_service_worker_t *ctx;

    for (i = 0; i < distribution_service_num_ctx; i++) {
        ctx = &distribution_service_ctx[i];
        if (ctx->id == data_id) {
            return distribution_service_worker_update(ctx, data, size);
        }
    }

    return -1;
}

int distribution_service_pause(
    uint32_t data_id)
{
    int i;
    distribution_service_worker_t *ctx;

    for (i = 0; i < distribution_service_num_ctx; i++) {
        ctx = &distribution_service_ctx[i];
        if (ctx->id == data_id) {
            return distribution_service_worker_pause(ctx);
        }
    }

    return -1;
}

int distribution_service_resume(
    uint32_t data_id)
{
    int i;
    distribution_service_worker_t *ctx;

    for (i = 0; i < distribution_service_num_ctx; i++) {
        ctx = &distribution_service_ctx[i];
        if (ctx->id == data_id) {
            return distribution_service_worker_resume(ctx);
        }
    }

    return -1;
}
