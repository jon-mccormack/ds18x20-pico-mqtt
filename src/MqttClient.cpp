#include <iostream>
#include <sstream>
#include "MqttClient.h"
#include "lwip/arch.h"
#include "pico/cyw43_arch.h"
#include "lwip/arch.h"

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;
    LWIP_UNUSED_ARG(data);

    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id,
                        (int)len, (int)flags));
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;

    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id,
                        topic, (int)tot_len));
}

void mqtt_request_cb(void *arg, err_t err)
{
    const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;

    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", client_info->client_id, (int)err));
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;
    LWIP_UNUSED_ARG(client);

    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status));

    if (status != MQTT_CONNECT_ACCEPTED)
    {
        std::cerr << "Didn't connect!" << std::endl;
    }

    if (status == MQTT_CONNECT_ACCEPTED)
    {
        std::cout << "Mqtt connection accepted :)" << std::endl;
        // need to figure out what these are for, but if they're commented out the messages
        // timeout when publishing
        // mqtt_sub_unsub(client, "topic_qos1", 1, mqtt_request_cb, LWIP_CONST_CAST(void *, client_info), 1);
        // mqtt_sub_unsub(client, "topic_qos0", 0, mqtt_request_cb, LWIP_CONST_CAST(void *, client_info), 1);
    }
}

void mqtt_pub_request_cb(void *arg, err_t err)
{
    switch (err)
    {
    case ERR_OK:
        std::cout << "Publish request callback OK" << std::endl;
        break;
    case ERR_TIMEOUT:
        std::cout << "Publish request callback TIMEOUT" << std::endl;
        break;
    case ERR_ABRT:
        std::cout << "Publish request callback ABRT" << std::endl;
        break;
    default:
        std::cout << "Publish request callback UNKNOWN" << std::endl;
    };
}

ip_addr_t getAddress(const std::string &ip)
{
    std::stringstream s(ip);
    int a, b, c, d; // to store the 4 ints
    char ch;        // to temporarily store the '.'
    s >> a >> ch >> b >> ch >> c >> ch >> d;
    ip_addr_t broker;
    IP4_ADDR(&broker, a, b, c, d);
    return broker;
}

MqttClient::MqttClient(const std::string &address, uint16_t port)
{
    std::cout << "Mqtt broker ip: " << address << ", port: " << std::to_string(port) << std::endl;
    cyw43_arch_lwip_begin();
    client = mqtt_client_new();

    static const struct mqtt_connect_client_info_t mqtt_client_info =
        {
            "jon-boy",
            NULL, /* user */
            NULL, /* pass */
            5,    /* keep alive */
            NULL, /* will_topic */
            NULL, /* will_msg */
            0,    /* will_qos */
            0     /* will_retain */

        };

    ip_addr_t broker = getAddress(address);
    // IP4_ADDR(&broker, 192, 168, 1, 37);
    // IP4_ADDR(&broker, 127, 0, 0, 1);

    // mqtt_set_inpub_callback(client,
    //                         mqtt_incoming_publish_cb,
    //                         mqtt_incoming_data_cb,
    //                         LWIP_CONST_CAST(void *, &mqtt_client_info));

    mqtt_client_connect(
        client,
        &broker,
        port,
        mqtt_connection_cb,
        LWIP_CONST_CAST(void *, &mqtt_client_info),
        &mqtt_client_info);

    cyw43_arch_lwip_end();
}

void MqttClient::publish(const std::string &topic, const std::string &payload)
{
    cyw43_arch_lwip_begin();
    std::cout << "MqttClient::publish" << std::endl;
    u8_t qos = 1; /* 0 1 or 2, see MQTT specification.  AWS IoT does not support QoS 2 */
    u8_t retain = 0;
    err_t err = mqtt_publish(client, topic.c_str(), payload.c_str(), payload.length(), qos, retain, mqtt_pub_request_cb, nullptr);
    cyw43_arch_lwip_end();
    if (err != ERR_OK)
    {
        std::cout << "Publish err: " << std::to_string(err) << std::endl;
    }
}