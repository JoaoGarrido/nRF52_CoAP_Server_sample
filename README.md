
# CoAP 

# Example 1: Non confirmable request and non confirmable response

```text
Client  Server
    |      |
    |      |
    +----->|     Header: GET (T=NON, Code=0.01, MID=0x7d40)
    | GET  |      Token: 0x75
    |      |   Uri-Path: "temperature"
    |      |
    |      |
    |<-----+     Header: 2.05 Content (T=NON, Code=2.05, MID=0xad7d)
    | 2.05 |      Token: 0x75
    |      |    Payload: "22.3 C"
    |      |

    Figure 22: Non-confirmable Request; Non-confirmable Response
```

Name:
not_safe

Request_handler:
* Message type  -> Non Confirmable
* Message code  -> GET (0.1)
* Message ID    -> Any
* Uri-Path      -> "temperature"
* Token         -> VALUE_A

Response:
* Message type  -> Non Confirmable
* Message code  -> Content (2.05)
* Token         -> VALUE_A
* Payload       -> MeshLocalID

Test with libcoap:
```console
coap-client -m get -N coap://[$ot ipaddr mleid]/temperature -v 9
```

# Example 2: Confirmable request and non confirmable piggyback response
```text
Client              Server       Client              Server
    |                  |             |                  |
    |   CON [0xbc90]   |             |   CON [0xbc91]   |
    | GET /temperature |             | GET /temperature |
    |   (Token 0x71)   |             |   (Token 0x72)   |
    +----------------->|             +----------------->|
    |                  |             |                  |
    |   ACK [0xbc90]   |             |   ACK [0xbc91]   |
    |   2.05 Content   |             |  4.04 Not Found  |
    |   (Token 0x71)   |             |   (Token 0x72)   |
    |     "22.5 C"     |             |   "Not found"    |
    |<-----------------+             |<-----------------+
    |                  |             |                  |

    Figure 4: Two GET Requests with Piggybacked Responses
```

Name: dodgy_pig

Request_handler:
* Message type  -> Confirmable
* Message code  -> GET (0.1)
* Message ID    -> VALUE_MSG_ID
* Uri-Path      -> "luminosity"
* Token         -> VALUE_TOKEN

Response:
* Message type  -> Acknowledgment
* Message code  -> Content (2.05)
* Message ID    -> VALUE_MSG_ID
* Token         -> VALUE_TOKEN
* Payload       -> MeshLocalID

Test with libcoap:
```console
coap-client -m get coap://[$ot ipaddr mleid]/luminosity -v 9
```

# Example 3: Confirmable request and confirmable separate response

Name: divorce_confirmation

Request_handler(confirmable):
* Message type  -> Confirmable
* Message code  -> GET (0.1)
* Message ID    -> VALUE_MSG_ID_A
* Uri-Path      -> "luminosity"
* Token         -> VALUE_TOKEN_A

Response(ack):
* Message type  -> Acknowledgment
* Message code  -> Empty (0.00)
* Message ID    -> VALUE_MSG_ID_A

Response(confirmable):
* Message type  -> Confirmable
* Message code  -> Content (2.05)
* Message ID    -> VALUE_MSG_ID_B
* Token         -> VALUE_TOKEN_A
* Payload       -> MeshLocalID

Request_handler(ack):
* Message type  -> Aknowledgment
* Message code  -> Empty (0.00)
* Message ID    -> VALUE_MSG_ID_B

Test with libcoap:
```console
coap-client -m get coap://[$ot ipaddr mleid]/air_quality -v 9
```

## Normal scenario

```text
Client  Server
    |      |
    |      |
    +----->|     Header: GET (T=CON, Code=0.01, MID=0x7d38)
    | GET  |      Token: 0x53
    |      |   Uri-Path: "temperature"
    |      |
    |      |
    |<- - -+     Header: (T=ACK, Code=0.00, MID=0x7d38)
    |      |
    |      |
    |<-----+     Header: 2.05 Content (T=CON, Code=2.05, MID=0xad7b)
    | 2.05 |      Token: 0x53
    |      |    Payload: "22.3 C"
    |      |
    |      |
    +- - ->|     Header: (T=ACK, Code=0.00, MID=0xad7b)
    |      |

            Figure 20: Confirmable Request; Separate Response
```

## Error

```text
Client  Server
    |      |
    |      |
    +----->|     Header: GET (T=CON, Code=0.01, MID=0x7d39)
    | GET  |      Token: 0x64
    |      |   Uri-Path: "temperature"
CRASH    |
    |      |
    |<- - -+     Header: (T=ACK, Code=0.00, MID=0x7d39)
    |      |
    |      |
    |<-----+     Header: 2.05 Content (T=CON, Code=2.05, MID=0xad7c)
    | 2.05 |      Token: 0x64
    |      |    Payload: "22.3 C"
    |      |
    |      |
    +- - ->|     Header: (T=RST, Code=0.00, MID=0xad7c)
    |      |

    Figure 21: Confirmable Request; Separate Response (Unexpected)
```

# Testing

## Requirements
In order to test this examples you should:
1. Install the nrf-5x SDK (tested in version 1.4.0)
2. Pull this repository to <name_of_folder> out of the SDK path 
3. Copy to samples/openthread/
    ```console
    $ cp -r <name_of_folder> ${ZEPHYR_BASE}/../nrf/samples/openthread/
    ```
4. Open terminal in the SDK folder
    ```console
    $ cd ${ZEPHYR_BASE}/../nrf/samples/openthread/<name_of_folder>
    ```

## Build
* For nrf52840dk
    ```console
    $ west build --b nrf52840dk_nrf52840
    ```
* For nrf52833dk
    ```console
    $ west build --b nrf52833dk_nrf52833
    ```

## Flash
There are 2 different devices:
1. Open JLinkExe
    * nrf52833dk binary
        ```console
        $ sudo JLinkExe -device NRF52833_XXAA -if swd -speed 1000
        ```
    * nrf52833dk binary
        ```console
        $ sudo JLinkExe -device NRF52840_XXAA -if swd -speed 1000
        ```
2. Connect to device
    ```console
    $ connect
    ```
3. Erase previous flash content
    ```console
    $ erase
    ```
4. Flash .hex file
    ```console
    $ loadfile bin/<name_of_file>
    ```

## Debug

Devices:
* /dev/ttyACM0 -> nrf52840dk
* /dev/ttyACM1 -> nrf52840 dongle (ot-br)
* /dev/ttyACM2 -> nrf52833dk

Connect to ot-br:
```console
$ sudo ot-ctl
```

Connect to other devices:
```console
$ screen <device_name> 115200
```