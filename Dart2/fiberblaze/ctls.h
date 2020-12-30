#ifndef CTLS_H
#define CTLS_H

/**
 * @file ctls.h 
 *  
 * @brief Shared data structures. 
 */

#ifdef __cplusplus
extern "C" {
#endif

// Make sure the compiler does not add its own padding to our data structures
#pragma pack(push,1)

/** 
 * @addtogroup PacketTransmission 
 * @{ 
 */  
/** 
 * @{ 
 * @name Transmit bucket definitions.
 */
#define FB_NO_BUCKETS   32          ///< Number of available transmit buckets

#define FB_BUCKET_SIZE  (2048-4)     ///< Size of a transmit bucket

#define RECV_DMA_SIZE (PAGE_SIZE * (1<<10)) 


#define REGISTERS_SIZE      (0x8000)

/* Userlogic register access*/
#define ULOGICREGWRITE      (0x4000)
#define ULOGICREGBASE       (0x600)



/// Use IO Control call to get TCP status
// #define FB_GET_TCP_STATUS_BY_IOCTL

/// @}

/// Transmit bucket data structure
typedef struct fb_bucket_t { 
#if defined(CHEMNITZ_FB2022) | defined(CHEMNITZ_ANCONA)
    uint16_t flags;                 ///< Flags
    uint16_t len;                   ///< Length of data
#endif
    uint8_t data[FB_BUCKET_SIZE];   ///< Data to be transmitted
} fb_bucket;

/// @} PacketTransmission

/**
 * @addtogroup PacketReceptionRaw 
 * @{ 
 */


/** Incoming packet data structure. 
  *
  * @note @ref fb_packet.offset_ip "Offsets" are number of double words.
  */
typedef struct fb_packet_t {
#if 0
#ifndef PLDMA
    uint64_t    next;               ///< Pointer to next packet. Only used for older FPGA architechtures.
#endif
#endif
    uint16_t    len;                ///< Length of payload
    uint16_t    status;             ///< Packet status.
#if defined(CHEMNITZ_FB2022) || defined(CHEMNITZ_ANCONA)
    uint32_t    padding[4];         ///< Not used
#else
    uint32_t    ts_seconds;         ///< Timestamp, whole seconds part.
    uint32_t    ts_quants;          ///< Timestamp, quants part. One 'quant' is 400 picoseconds.
    uint8_t     offset_ip;          ///< Offset to IP part of packet.
    uint8_t     offset_layer_4;     ///< Offset to layer 4 part of packet.
    uint8_t     offset_layer_5;     ///< Offset to layer 5 part of packet.
    uint8_t     padding2;           ///< Not used
#endif
    uint16_t    layer_5_len;        ///< Length of TCP or UDP payload. For other packet types, this field is undefined.
    uint8_t     pkt_payload[];      ///< Packet payload including layer 2 and upwards.
} fb_packet;


/** Get pointer to TCP payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint8_t* Pointer to payload
 */
static inline const uint8_t* fb_get_tcp_payload( const fb_packet* pkt )
{
    return pkt->pkt_payload + pkt->offset_layer_5 * 4 - 2;
}

/** Get pointer to raw payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint8_t* Pointer to payload
 */
static inline const uint8_t* fb_get_raw_payload( const fb_packet* pkt )
{
    return pkt->pkt_payload;
}

/** Get pointer to UDP payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint8_t* Pointer to payload
 */
static inline const uint8_t* fb_get_udp_payload( const fb_packet* pkt )
{
    return pkt->pkt_payload + pkt->offset_layer_5 * 4 - 2;
}

/** Get pointer to user logic payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint8_t* Pointer to payload
 */
static inline const uint8_t* fb_get_ulogic_payload( const fb_packet* pkt )
{
    return pkt->pkt_payload - 2;
}

/** Get length of TCP payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint16_t Length of payload
 */
static inline uint16_t fb_get_tcp_payload_length( const fb_packet* pkt )
{
    return pkt->layer_5_len;
}

/** Get length of UDP payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint16_t Length of payload
 */
static inline uint16_t fb_get_udp_payload_length( const fb_packet* pkt )
{
    return pkt->layer_5_len;
}

/** Get length of user logic payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint16_t Length of payload
 */
static inline uint16_t fb_get_ulogic_payload_length( const fb_packet* pkt )
{
    return pkt->len;
}

/** Get length of raw payload
 * 
 *  @param pkt Pointer to packet
 * 
 *  @return uint16_t Length of payload
 */
static inline uint16_t fb_get_raw_payload_length( const fb_packet* pkt )
{
    return pkt->len - 2;
}

/**
 * @{ 
 * @name Packet status flags.
 * @brief Bit mask definitions for the @ref fb_packet_t.status 
 *        field.
 */
#define FB_PKT_STATUS_END_OF_BUFFER     (1U << 12)  ///< Packet is at end of buffer - next packet will be at start.

/// @}

/// @} PacketReceptionRaw

/**
 * @addtogroup SystemInfo 
 * @{ 
 */

/** Statistics for a single port.
 */
typedef struct
{
    uint32_t okFrames;              ///< Number of OK frames
    uint32_t errorFrames;           ///< Number of error frames
    uint32_t size_1_63;             ///< Frames with length less than 64 bytes
    uint32_t size_64;               ///< Frames with length equal to 64 bytes
    uint32_t size_65_127;           ///< Frames with length between 65 and 127 bytes
    uint32_t size_128_255;          ///< Frames with length between 128 and 255 bytes
    uint32_t size_256_511;          ///< Frames with length between 256 and 511 bytes
    uint32_t size_512_1023;         ///< Frames with length between 512 and 1023 bytes
    uint32_t size_1024_1517;        ///< Frames with length between 1024 and 1517 bytes
    uint32_t size_above_1518;       ///< Frames with length of more than 1518 bytes
    uint64_t okBytes;               ///< Number of OK bytes
    uint64_t errorBytes;            ///< Number of error bytes
} fb_stat_port;

/** Statistics for all ports
 */
typedef struct
{
    uint32_t    ts_seconds;         ///< Timestamp, whole seconds part.
    uint32_t    ts_quants;          ///< Timestamp, quants part. One 'quant' is 400 picoseconds.
    fb_stat_port rx_stat[ 4 ];      ///< Receive statistics for ports 0-3
    uint32_t reserved[ 4 ];         ///< Reserved for future use
    fb_stat_port rx_stat_ext[ 4 ];  ///< Receive statistics for ports 4-7
    uint32_t reserved1[ 4 ];        ///< Reserved for future used
    uint32_t padding[8];
    fb_stat_port tx_stat[ 4 ];      ///< Trasmit statistics for ports 0-3
    uint32_t reserved2[ 4 ];        ///< Reserved for furture use
    fb_stat_port tx_stat_ext[ 4 ];  ///< Transmit statistics for ports 0-3
    uint32_t reserved3[ 4 ];        ///< Reserved for future used
} fb_stat_packet;

/// @} SystemInfo


/**
 * @addtogroup TCPConnections 
 * @{ 
 */

/**
 * @{ 
 * @name TCP status codes.
 * @brief Possible values returned by @ref fb_get_tcp_status.
 */

#define FB_TCP_CLOSED                   0x0 ///< Socket is closed
#define FB_TCP_LISTEN                   0x1 ///< Listen for incoming connection requests.
#define FB_TCP_SYN_SENT                 0x2 ///< Sent a connection request, waiting for ack.
#define FB_TCP_SYN_RECV                 0x3 ///< Received a connection request, sent ack, waiting for final ack in three-way handshake.
#define FB_TCP_ESTABLISHED              0x4 ///< Connection established.
#define FB_TCP_FIN_WAIT1                0x5 ///< Our side has shutdown.
#define FB_TCP_FIN_WAIT2                0x6 ///< Waiting for remote to shutdown.
#define FB_TCP_CLOSING                  0x7 ///< Both sides have shutdown but we still wait for ack.
#define FB_TCP_CLOSE_WAIT               0x8 ///< Remote side has shutdown and is waiting for us to finish writing our data and to shutdown (we have to close() to move on to LAST_ACK).
#define FB_TCP_LAST_ACK                 0x9 ///< Our side has shutdown after remote has shutdown.
#define FB_TCP_TIME_WAIT                0xA ///< Waiting for enough time to pass to be sure the remote end has received the ack of the connection termination request.
#define FB_TCP_ARP_SENT                 0xD ///< We have sent an arp packet

/// @}

/// @} TCPConnections

/** Time control selection modes
 */
#define TIMECONTROL_INVALID     0   ///< Invalid setting for time control mode
#define TIMECONTROL_HARDWARE    1   ///< Time controlled through HW (the default)
#define TIMECONTROL_SOFTWARE    2   ///< Time controlled throug SW

/// @cond PRIVATE_CODE
// These data structure and defintions are internal and are therefore not documented.

#define MAC_ADDR_LEN 6

typedef struct _fb_connection_info {
    uint32_t remote_ip;
    uint16_t remote_port;
    uint16_t local_port;
    uint8_t  mac_addr[MAC_ADDR_LEN];
} fb_connection_info;

typedef struct _fb_listen_info {
    uint16_t local_port;
    uint32_t timeout;
} fb_listen_info;

typedef struct _fb_local_addr {
    uint32_t phy_interface;
    uint32_t local_ip;
    uint32_t netmask;
    uint32_t gateway;
    uint8_t mac_addr[MAC_ADDR_LEN];
} fb_local_addr;

typedef struct _fb_ping_info {
    uint32_t phy_interface;
    uint32_t remote_ip;
    uint16_t sequence;
    uint8_t  remote_mac_addr[MAC_ADDR_LEN];
    uint32_t pingtime;
} fb_ping_info;

#define IGMP_JOIN  0x1
#define IGMP_LEAVE 0x2

typedef struct fb_igmp_info_t {
    uint32_t action;
    uint32_t group;
    uint16_t port;
} fb_igmp_info;

#define FB_CHANNEL_TYPE_TCP        0
#define FB_CHANNEL_TYPE_UDP        1
#define FB_CHANNEL_TYPE_MON        2
#define FB_CHANNEL_TYPE_ULOGIC     3
#define FB_CHANNEL_TYPE_OOB        4
#define FB_CHANNEL_TYPE_UNKNOWN    5

typedef struct _fb_alloc_info_ {
  uint16_t type;
  uint16_t channel_no;
  uint32_t phy_interface_no;
  uint8_t  enableDma;
  uint64_t initial_packet; 
} fb_alloc_info;

typedef struct _fb_pldma_t {
    volatile uint64_t lastPacket[64];
} fb_pldma;

typedef struct _fb_filling_t {
    uint16_t    filling;
    uint16_t    peak_filling;
} fb_filling;

typedef struct _fb_status_dma_t {
    volatile uint32_t    input_stat;
    uint32_t    reserved_1[ 4 ];
    volatile uint32_t    input_stat_ext;
    volatile uint32_t    tcp_connection_status;
    uint32_t    reserved_2;
    volatile fb_filling  dma_filling[ 64 ];
} fb_status_dma;

typedef struct _fb_temperature_t {
    uint16_t minTemp;
    uint16_t curTemp;
    uint16_t maxTemp;
} fb_temperature;

typedef struct _fb_misc_t {
    uint16_t type;
    uint64_t value;
} fb_misc;

// Used internal in the driver
typedef struct {
    volatile uint64_t next;
    uint32_t status;
    fb_stat_packet stat;
} fb_statistics_packet;

typedef struct _fb_dmabuf_t {
    void       *data;       // User addressable pointer
    uint64_t    dma;        // DMA address (usually physical address)
    uint64_t    sz;         // size of the buffer
} fb_dmabuf;

#pragma pack(pop)

#define CHEMNITZ_IOC_MAGIC 'k'
#define CHEMNITZ_LINK_STATUS          _IOR( CHEMNITZ_IOC_MAGIC, 1,  uint64_t )
#define CHEMNITZ_ALLOC_CHANNEL        _IOWR(CHEMNITZ_IOC_MAGIC, 2,  fb_alloc_info )
#define CHEMNITZ_DEALLOC_CHANNEL      _IO(  CHEMNITZ_IOC_MAGIC, 3 )
#define CHEMNITZ_CONNECT              _IOW( CHEMNITZ_IOC_MAGIC, 4,  fb_connection_info )
#define CHEMNITZ_DISCONNECT           _IO(  CHEMNITZ_IOC_MAGIC, 5 )
// #define CHEMNITZ_BUSY_POLL            _IOR(CHEMNITZ_IOC_MAGIC,6, uint32_t)
#define CHEMNITZ_GET_BUCKET_HWADDR    _IOWR(CHEMNITZ_IOC_MAGIC, 7,  uint32_t )
#define CHEMNITZ_GET_BUCKET_REGISTER  _IOR( CHEMNITZ_IOC_MAGIC, 8,  uint32_t ) 
#define CHEMNITZ_SET_LOCAL_ADDR       _IOW( CHEMNITZ_IOC_MAGIC, 9,  fb_local_addr ) 
#define CHEMNITZ_GET_LOCAL_ADDR       _IOWR(CHEMNITZ_IOC_MAGIC, 10, fb_local_addr ) 
#define CHEMNITZ_GET_NO_BUCKETS       _IOR( CHEMNITZ_IOC_MAGIC, 11, uint32_t ) 
#define CHEMNITZ_SET_READMARK         _IOW( CHEMNITZ_IOC_MAGIC, 12, uint32_t ) 
#define CHEMNITZ_GET_RECV_HWADDR      _IOR( CHEMNITZ_IOC_MAGIC, 13, uint32_t ) 
#define CHEMNITZ_ULOGICREG            _IOWR( CHEMNITZ_IOC_MAGIC, 14, uint64_t )
#define CHEMNITZ_LISTEN               _IOW( CHEMNITZ_IOC_MAGIC, 15, fb_listen_info )
#define CHEMNITZ_IGMPINFO             _IOW( CHEMNITZ_IOC_MAGIC, 16, fb_igmp_info )
// #define CHEMNITZ_MEMORY_MAPPINGS      _IOW( CHEMNITZ_IOC_MAGIC, 17, fb_memory_mappings )
#define CHEMNITZ_GET_TCP_STATUS       _IOR( CHEMNITZ_IOC_MAGIC, 18, uint32_t )
#define CHEMNITZ_READ_TEMPERATURE     _IOR( CHEMNITZ_IOC_MAGIC, 19, fb_temperature )
#define CHEMNITZ_READ_VERSION         _IOWR( CHEMNITZ_IOC_MAGIC, 20, fb_misc )
#define CHEMNITZ_GET_STAT             _IOR( CHEMNITZ_IOC_MAGIC, 21, fb_stat_packet )
#define CHEMNITZ_LAST_ACK_NO          _IOR( CHEMNITZ_IOC_MAGIC, 22, uint32_t )
#define CHEMNITZ_PING                 _IOWR( CHEMNITZ_IOC_MAGIC, 23, fb_ping_info )
#define CHEMNITZ_TIMERMODE            _IOW( CHEMNITZ_IOC_MAGIC, 24, uint32_t )
#define CHEMNITZ_ULREAD               _IOW( CHEMNITZ_IOC_MAGIC, 25, uint32_t )
#define CHEMNITZ_ALLOC_DMABUF         _IOWR(CHEMNITZ_IOC_MAGIC, 26, fb_dmabuf )
#define CHEMNITZ_DEALLOC_DMABUF       _IOW( CHEMNITZ_IOC_MAGIC, 27, void* )

/// @endcond

#ifdef __cplusplus
}
#endif

#endif /* CTLS_H */
