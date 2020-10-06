#ifndef DISTRIBUTION_SERVICE_H
#define DISTRIBUTION_SERVICE_H

#include <mira.h>

/**
 * @brief Definition of callback function called on incoming update
 */
typedef void (*distribution_service_callback_t)(
    uint32_t data_id,
    void *data,
    mira_size_t size,
    const mira_net_udp_callback_metadata_t *metadata,
    void *storage);

/**
 * @brief Register a new data set to distribute over the network
 *
 * @param data_id        Unique identifier for distributed data
 * @param data           Storage of data to distribute
 * @param size           Size of distributed data, max 230
 * @param rate           Mira Network rate to propagate data over
 * @param update_handler Function called on incoming update
 * @param storage        Generic storage of data which may be
 *                       accessed in the callback function
 *
 * @return Status of the operation
 * @retval 0 on success, -1 on failure
 */
int distribution_service_register(
    uint32_t data_id,
    void *data,
    mira_size_t size,
    uint8_t rate,
    distribution_service_callback_t update_handler,
    void *storage);

/**
 * @brief Update the distributed data with new content
 *
 * @param data_id Unique identifier for distributed data
 * @param data    Storage of distributed data
 * @param size    New size of distributed data
 *
 * @return Status of the operation
 * @retval 0 on success, -1 on failure
 */
int distribution_service_update(
    uint32_t data_id,
    void *data,
    mira_size_t size);

/**
 * @brief Pause a running distribution, preventing sending and receiving updates
 *
 * @param data_id Unique identifier fo distributed data
 *
 * @return Status of the operation
 * @retval 0 on success, -1 on failure
 */
int distribution_service_pause(
    uint32_t data_id);

/**
 * @brief Resume a paused distribution, preventing sending and receiving updates
 *
 * @param data_id Unique identifier for distributed data
 *
 * @return Status of the operation
 * @retval 0 on success, -1 on failure
 */
int distribution_service_resume(
    uint32_t data_id);

#endif
