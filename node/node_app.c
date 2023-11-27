/*----------------------------------------------------------------------------
Copyright (c) 2020 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.

This example is provided as is, without warranty.
----------------------------------------------------------------------------*/

#include <mira.h>
#include <stdio.h>
#include <string.h>

#include "../distribution_service.h"

#define UDP_PORT 456
#define SEND_INTERVAL 60
#define CHECK_NET_INTERVAL 1
#define DISTRIBUTION_ID_STATE 0x10011001

/*
 * Identifies as a node.
 * Sends data to the root.
 */
/* clang-format off */
static const mira_net_config_t net_config = {
    .pan_id = 0x6cf9eb32,
    .key = {
        0x11, 0x12, 0x13, 0x14,
        0x21, 0x22, 0x23, 0x24,
        0x31, 0x32, 0x33, 0x34,
        0x41, 0x42, 0x43, 0x44
    },
    .mode = MIRA_NET_MODE_MESH,
    .rate = MIRA_NET_RATE_FAST,
    .antenna = 0,
    .prefix = NULL /* default prefix */
};
/* clang-format on */

MIRA_IODEFS(MIRA_IODEF_NONE,    /* fd 0: stdin */
            MIRA_IODEF_UART(0), /* fd 1: stdout */
            MIRA_IODEF_NONE     /* fd 2: stderr */
                                /* More file descriptors can be added, for use with dprintf(); */
);

static void udp_listen_callback(mira_net_udp_connection_t* connection,
                                const void* data,
                                uint16_t data_len,
                                const mira_net_udp_callback_metadata_t* metadata,
                                void* storage)
{
    char buffer[MIRA_NET_MAX_ADDRESS_STR_LEN];
    uint16_t i;

    printf("Received message from [%s]:%u: ",
           mira_net_toolkit_format_address(buffer, metadata->source_address),
           metadata->source_port);
    for (i = 0; i < data_len - 1; i++) {
        printf("%c", ((char*)data)[i]);
    }
    printf("\n");
}

PROCESS(main_proc, "Main process");

int distribution_init(void);
static uint32_t global_state;

void mira_setup(void)
{
    mira_status_t uart_ret;
    mira_uart_config_t uart_config = { .baudrate = 115200,
                                       .tx_pin = MIRA_GPIO_PIN(0, 6),
                                       .rx_pin = MIRA_GPIO_PIN(0, 8) };

    MIRA_MEM_SET_BUFFER(12288);

    uart_ret = mira_uart_init(0, &uart_config);
    if (uart_ret != MIRA_SUCCESS) {
        /* Nowhere to send an error message */
    }

    global_state = 0;
    process_start(&main_proc, NULL);
}

PROCESS_THREAD(main_proc, ev, data)
{
    static struct etimer timer;

    static mira_net_udp_connection_t* udp_connection;

    static mira_net_address_t net_address;
    static char buffer[MIRA_NET_MAX_ADDRESS_STR_LEN];
    static mira_status_t res;
    static const char* message = "Hello Network";

    PROCESS_BEGIN();
    /* Pause once, so we don't run anything before finish of startup */
    PROCESS_PAUSE();

    printf("Starting Node (Sender).\n");
    printf("Sending one packet every %d seconds\n", SEND_INTERVAL);

    mira_status_t result = mira_net_init(&net_config);
    if (result) {
        printf("FAILURE: mira_net_init returned %d\n", result);
        while (1)
            ;
    }

    /* Initialize the distribution service after the network is started */

    res = distribution_init();
    if (res) {
        printf("ERROR: distrubition_init() failed. (%d)\n", res);
    }

    /*
     * Open a connection, but don't specify target address yet, which means
     * only mira_net_udp_send_to() can be used to send packets later.
     */
    udp_connection = mira_net_udp_connect(NULL, 0, udp_listen_callback, NULL);

    while (1) {
        mira_net_state_t net_state = mira_net_get_state();

        if (net_state != MIRA_NET_STATE_JOINED) {
            printf("Waiting for network (state is %s)\n",
                   net_state == MIRA_NET_STATE_NOT_ASSOCIATED ? "not associated"
                   : net_state == MIRA_NET_STATE_ASSOCIATED   ? "associated"
                   : net_state == MIRA_NET_STATE_JOINED       ? "joined"
                                                              : "UNKNOWN");
            etimer_set(&timer, CHECK_NET_INTERVAL * CLOCK_SECOND);
        } else {
            /* Try to retrieve the root address. */
            res = mira_net_get_root_address(&net_address);

            if (res != MIRA_SUCCESS) {
                printf("Waiting for root address (res: %d)\n", res);
                etimer_set(&timer, CHECK_NET_INTERVAL * CLOCK_SECOND);
            } else {
                /*
                 * Root address is successfully retrieved, send a message to the
                 * root node on the given UDP Port.
                 */
                printf("Sending to address: %s\n",
                       mira_net_toolkit_format_address(buffer, &net_address));
                mira_net_udp_send_to(
                  udp_connection, &net_address, UDP_PORT, message, strlen(message));
                etimer_set(&timer, SEND_INTERVAL * CLOCK_SECOND);
            }
        }
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    }

    mira_net_udp_close(udp_connection);

    PROCESS_END();
}

void global_state_update_cb(uint32_t data_id,
                            void* data,
                            mira_size_t size,
                            const mira_net_udp_callback_metadata_t* metadata,
                            void* storage)
{
    memcpy(&global_state, data, size);
    printf("New state received, %ld, [ID: 0x%08lx]\n", global_state, data_id);
}

int distribution_init()
{
    mira_status_t status;

    status = distribution_service_register(DISTRIBUTION_ID_STATE,
                                           &global_state,        /* state message */
                                           sizeof(global_state), /* size */
                                           0,                    /* rate */
                                           global_state_update_cb,
                                           NULL);

    return status;
}
