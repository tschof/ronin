#ifndef FB_H
#define FB_H

/**
 * @file fb.h 
 *  
 * @brief API declaration and inline function definition.
 */

#ifdef __cplusplus
extern "C" {
#endif

// #include <unistd.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "ctls.h"
#include <stdio.h>

/** **************************************************************************
 * @defgroup ConstantsMacros Constants and macros. 
 * @{ 
 */ 

/// Size of memory pages
#define PAGE_SIZE   4096

/// Invalid port number
#define PORT_INVALID    (-1)

/// TCP port number
#define PORT_TCP        0
#define PORT_0          0

/// UDP port number
#define PORT_UDP        1
#define PORT_1          1

#define PORT_2          2
#define PORT_3          3
#define PORT_4          4
#define PORT_5          5
#define PORT_6          6
#define PORT_7          7

/// @} ConstantsMacros

/// @cond PRIVATE_CODE
// The data structure and macros in the PRIVATE_CODE section need to be
// defined here such that the inline functions in this file can work. You must 
// under no circumstances access its contents directly but instead use the 
// access functions provided.

/// Offset to a numbered bucket from start of bucket area
#define BUCKET_OFFSET(n)   (REGISTERS_SIZE + (2 * PAGE_SIZE) + (n * PAGE_SIZE))

/// Offset to end of bucket area
#define BUCKET_END          BUCKET_OFFSET(FB_NO_BUCKETS)

/// Indicate that this is the most likely code path taken
#define likely(x)       __builtin_expect((x),1)

/// Indicate that this is the most unlikely code path taken
#define unlikely(x)     __builtin_expect((x),0)

/** Context for a device.
 */
struct fb_context
{
    int fd;                                 ///< File descriptor
    const uint32_t sendRegister;            ///< Send register number
    uint32_t buckets_addr[FB_NO_BUCKETS];   ///< Table of bucket addresses
    void* mem;                              ///< Pointer to location in user space that the FPGA
    void* regs;                             ///< Pointer to the registers
    const uint64_t recvHwAddr;              ///< Physical address as seen by the FPGA
    fb_pldma* pl;                           ///< Pointer to pointer list DMA
    fb_status_dma* st;                      ///< Pointer to status DMA
    uint64_t lastPacketPa;                  ///< Last cached pointer to head of ring buffer - HW address
    uint64_t initial_packet;                ///< First packet in ring buffer
    unsigned int phy_channel_no;            ///< physical channel no
    uint32_t last_readmark;                 ///< Last advance of the read marker
};
/// @endcond

/** **************************************************************************
 * @defgroup InitialisationCleanup Initialisation and cleanup. 
 * @{ 
 * @brief Create and destroy a device context, configure IP adresses. 
 */ 

/** Obtain a device context
 * 
 *  @param ctx Pointer to where the new device context shall be put if
 *             allocation succeeds.
 *  @param devname Name to device node in '/dev' for the card. If NULL then
 *  "/dev/fiberblaze/" is used.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_init( struct fb_context** ctx, const char *devname );

/** Destroy the device context
 * 
 *  @param ctx Device context that shall be destroyed.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_destroy( struct fb_context* ctx );

/** Set the local parameters (IP data, MAC address) for the
 *  physical interface.
 * 
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 *  @param local_ip Local IP address
 *  @param netmask Netmask for the interface
 *  @param gateway Gateway address
 *  @param mac_addr MAC address of the physical interface. If NULL the card 
 *                  use its factory MAC address for the interface.
 *   
 *  @note Only IPv4 addresses are supported. 
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_set_local_addr( const struct fb_context* ctx, uint32_t phy_interface, struct in_addr local_ip, struct in_addr netmask, struct in_addr gateway, uint8_t mac_addr[MAC_ADDR_LEN] );

/** Get the local IP parameters for a physical interface.
 * 
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 *  @param local_ip Pointer to where the local IP address is
 *                  delivered.
 *  @param netmask Pointer to where the netmask for the
 *                 interface is delivered.
 *  @param gateway Pointer to where the gateway address is
 *                 delivered.
 *  @param mac_addr Pointer to where the MAC address of the
 *                  physical interface is delivered.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_get_local_addr( const struct fb_context* ctx, uint32_t phy_interface, struct in_addr *local_ip, struct in_addr *netmask, struct in_addr *gateway, uint8_t mac_addr[MAC_ADDR_LEN] );

/// @} InitialisationCleanup

/** **************************************************************************
 * @defgroup SystemInfo System information. 
 * @{ 
 * @brief Get various pieces of information about system state. 
 */ 

/** Get FPGA temperatures.
 *  
 *  Temperatures are reported in degrees C
 * 
 *  @param ctx Device context
 *  @param minTemp Minimum FPGA temperature
 *  @param curTemp Current FPGA temperature
 *  @param maxTemp Maximum FPGA temparature
 *  
 *  @return 0 on success, non-zero otherwise.
 */
int fb_get_fpga_temperature( const struct fb_context *ctx, float *minTemp, float *curTemp, float *maxTemp );

/** @{
 *  @name Link status bit masks.
 *  @brief Used when decoding the value returned by @ref
 *         fb_get_link_status
 */
#define PORT_0_LINK_STATUS  (0x1LL << 0)         ///< Port 0 link status bit
#define PORT_1_LINK_STATUS  (0x1LL << 7)         ///< Port 1 link status bit
#define PORT_2_LINK_STATUS  (0x1LL << 15)        ///< Port 2 link status bit
#define PORT_3_LINK_STATUS  (0x1LL << 23)        ///< Port 3 link status bit
#define PORT_4_LINK_STATUS  (0x1LL << (0+32))    ///< Port 0 link status bit
#define PORT_5_LINK_STATUS  (0x1LL << (7+32))    ///< Port 1 link status bit
#define PORT_6_LINK_STATUS  (0x1LL << (15+32))   ///< Port 2 link status bit
#define PORT_7_LINK_STATUS  (0x1LL << (23+32))   ///< Port 3 link status bit
#define PORT_TCP_LINK_STATUS PORT_0_LINK_STATUS ///< TCP port link status bit
#define PORT_UDP_LINK_STATUS PORT_1_LINK_STATUS ///< UDP port link status bit

/// @}

/** Get link status for network ports.
 *
 *  @param ctx Device context.
 *  @param linkStatus Pointer to where bitmask for link status shall be
 *                    returned.
 *
 *  @return 0 on sucess, non-zero otherwise.
 */
int fb_get_link_status( const struct fb_context* ctx, uint64_t *linkStatus );

/** Get FPGA version number and type.
 *  
 *  @param ctx Device context
 *  @param fpga_version Pointer to where the FPGA version shall
 *                      be delivered.
 *  
 *  @return 0 on success, non-zero otherwise.
 */
int fb_get_fpga_version( const struct fb_context* ctx, uint64_t *fpga_version );

/** Get board serial number 
 *  
 *  @param ctx Device context
 *  @param serial_no  Pointer to where the board searal number shall 
 *                      be delivered.
 *  
 *  @return 0 on success, non-zero otherwise.
 */
int fb_get_board_serial( const struct fb_context* ctx, uint32_t *serial_no );


/** Set timestamping mode.
 *
 *  Card timer can either be regulated by software or a by hardware via a pulse per second signal.
 *
 *  @param ctx Device context
 *  @param mode Mode @ref TIMECONTROL_SOFTWARE (default), or @ref TIMECONTROL_HARDWARE.
 *
 *  @return 0 on success, non-zero otherwice.
 */
int fb_set_timestamping_mode( const struct fb_context *ctx, uint8_t mode );

/** Ping remote address.
 *  
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 *  @param remote_ip Ip address to ping
 *  @param remote_mac_addr Mac address of remote end or NULL to make device
 *  make arp request for address, if given an pointer to an array of zeors, the
 *  array will be updated with the remote mac address.  
 *  @param time updated with the number of microseconds. 
 *  
 *  @return 0 on success, non-zero otherwise.
 */
int fb_ping( const struct fb_context* ctx, unsigned int phy_interface, struct in_addr remote_ip,  uint8_t remote_mac_addr[MAC_ADDR_LEN], uint32_t *time);


/** Get port statistics. 
 *
 *  @param ctx Device context
 *  @param packet Pointer to data structure where statistics data shall be
 *                delivered.
 *
 *  @return 0 on success, non-zero otherwize.
 */
int fb_get_statistic( struct fb_context *ctx, fb_stat_packet *packet);


/// @} SystemInfo

/** **************************************************************************
 * @defgroup ChannelSetup Initialisation of TCP, UDP and monitoring channels. 
 * @{ 
 */ 

/** Allocate a TCP channel.
 * 
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 *  @param chan Instruct FPGA to use specific TCP channel. Use the value
 *              -1 to let the driver assign the channel.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_allocate_tcp_channel( struct fb_context* ctx, uint32_t phy_interface, int16_t chan );

/** Allocate a UDP channel.
 * 
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 *  @param chan Instruct FPGA to use specific UDP channel. Use the value
 *              -1 to let the driver assign the channel.
    @param enableDma enabling sending the stream to host ring buffer
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_allocate_udp_channel( struct fb_context* ctx, uint32_t phy_interface, int16_t chan, uint8_t enableDma );

/** Allocate a monitoring channel.
 * 
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_allocate_monitor_channel( struct fb_context* ctx, uint32_t phy_interface );

/** Allocate a user logic channel.
 * 
 *  @param ctx Device context
 *  @param chan - ulogic channel number 0 .. 7 
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_allocate_ulogic_channel(struct fb_context *ctx, int16_t chan );

/// @} ChannelSetup

/** **************************************************************************
 * @defgroup TCPConnections Handle TCP connections. 
 * @{ 
 * @brief Make and break connections, listen on ports and get TCP status. 
 */ 

/** Listen for incoming connection. 
 * 
 *  @note This call blocks until a connection is obtained.
 * 
 *  @param ctx Device context
 *  @param local_port Port we listen on
 *  @param timeout Timeout in seconds - If the value of is zero we don't get any timeout
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_listen( struct fb_context* ctx, uint16_t local_port, uint32_t timeout );

/** Connect to remote machine. 
 * 
 *  @param ctx Device context.
 *  @param remote_ip Remote IP address.
 *  @param remote_port Port on remote to connect to.
 *  @param local_port Local port used for connection. Set to 0 to let the
 *                    driver assign a port number.
 *  @param mac_addr Override the destination MAC address. If NULL the MAC
 *                  address is resolved using ARP.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_connect( struct fb_context* ctx, struct in_addr remote_ip, uint16_t remote_port, uint16_t local_port, uint8_t mac_addr[MAC_ADDR_LEN] );

/** Disconnect the TCP connection
 * 
 *  @param ctx Device context
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_disconnect( struct fb_context* ctx );


/** Get status of TCP channel
 * 
 *  @param ctx Device context.
 * 
 *  @return int The TCP channel status or -1 on error.
 */
static inline int fb_get_tcp_status( const struct fb_context* ctx )
{
#ifndef FB_GET_TCP_STATUS_BY_IOCTL
    // There are 4 bits per channel
    return (ctx->st->tcp_connection_status >> (ctx->phy_channel_no * 4)) & 0x0F;
#else
    uint32_t status = 0 ;
    if( ioctl( ctx->fd, CHEMNITZ_GET_TCP_STATUS, &status ) )
    {
        return -1;
    }
    return status;
#endif
}

/** Get last acknowledge number
 * 
 *  @param ctx Device context.
 *  @param last_ack_num Pointer to where last acknowledge number shall be
 *                      delivered.
 *  
 *  @return 0 on success, non-zero otherwise.
 */
int fb_get_last_ack_num( const struct fb_context* ctx, uint32_t* last_ack_num );

/// @} TCPConnections

/** **************************************************************************
 * @defgroup PacketReceptionFd Handle incoming packets, file descriptor.  
 * @{ 
 * @brief Use the file descriptor and standard system calls to get incoming 
 * packets. 
 *  
 * Due to the use of system calls that rely on interrupts and this method is 
 * probably slower than the @ref PacketReceptionRaw "raw memory access method". 
 */ 

/** Return the file descriptor for the the current connection.
 *  
 *  The file descriptor that this function provides is used with
 *  standard system calls.
 * 
 *  @param ctx Device context
 * 
 *  @return File descriptor.
 */
static inline int fb_get_fd( const struct fb_context* ctx )
{
    return ctx->fd;
}

/// @} PacketReceptionFd

/** **************************************************************************
 * @defgroup PacketReceptionRaw Handle incoming packets, raw memory access.  
 * @{ 
 * @brief Functions for accessing incoming packets directly. 
 *  
 * @warning You must have allocated a TCP channel using @ref fb_allocate_tcp_channel, 
 * a UDP channel using @ref fb_allocate_udp_channel or a monitor channel using @ref 
 * fb_allocate_monitor_channel before calling any of the functions in this module.
 *  
 * @note The functions and definitions in this section are only used when 
 *       the receive buffer is handled in user space.
 */ 

/// @cond PRIVATE_CODE
/** Compute and return the fill level of the ring buffer in bytes.
 * 
 *  @param ctx Device context
 *  @param current Pointer to last packet received
 *  
 */
static inline uint32_t fb_get_fill_level_bytes( struct fb_context* ctx )
{
    uint32_t fill_level, wr_offset;
    ctx->lastPacketPa = ctx->pl->lastPacket[ ctx->phy_channel_no ]; // Update cache value of last packet
    wr_offset = ctx->lastPacketPa - (uint64_t)ctx->recvHwAddr;
    fill_level = (wr_offset - ctx->last_readmark) % RECV_DMA_SIZE;
    return fill_level;
}

/** Get the raw pointer to the next packet 
 * 
 *  @param ctx Device context
 *  @param current Pointer to last packet received
 *  
 *  @note This function is for <b>INTERNAL USE ONLY</b>. It must not be
 *        used by user space programs.
 * 
 *  @return pointer to fb_packet
 */
static inline fb_packet* fb_get_raw_next_packet( const struct fb_context* ctx, const fb_packet* current )
{
    const unsigned int headerSize = (sizeof(fb_packet) - 2);
    if( unlikely( !current || current->status & FB_PKT_STATUS_END_OF_BUFFER ) )
    {
        if ( ( current && ( current->status & FB_PKT_STATUS_END_OF_BUFFER ) ) || ! ctx->initial_packet ) 
            return (fb_packet*)((uint8_t*)ctx->mem + BUCKET_END);

        current = (fb_packet*)((uint8_t*)ctx->mem + BUCKET_END + ( ctx->initial_packet - (uint64_t)ctx->recvHwAddr ));
    }

    return (fb_packet*)((uint8_t*)current + (((headerSize + current->len) & 0x3fU) ?
                ((headerSize + current->len) & ~0x3fU) + 64 :
                (headerSize + current->len)));
}
/// @endcond

/** Get the next incoming packet based on previous incoming packet.
 * 
 *  This function always returns immediately but it may not return a packet.
 * 
 *  @param ctx Device context
 *  @param current Pointer to previous packet or NULL if this is the first call to this function.
 * 
 *  @return pointer to received packet or NULL if no new packets are available.
 */
static inline fb_packet* fb_get_next_packet( struct fb_context* ctx, const fb_packet* current )
{
    fb_packet* nextPacket;

    // Our cached lastPacketPa is not set - then update it
    if( unlikely( !ctx->lastPacketPa ) ) // Only true for the first packet
    {
        // Register that we will manually handle incoming traffic
        int ret = ioctl( ctx->fd, CHEMNITZ_ULREAD , NULL );
        if( ret ) {
            fprintf(stderr, "Failed to register traffic ownership on phys chan %d\n", ctx->phy_channel_no );
        }

        ctx->lastPacketPa = ctx->pl->lastPacket[ ctx->phy_channel_no ]; // Initialize the cache pldma.
        if( unlikely( !ctx->lastPacketPa ) )
            return 0; // No packet yet.
    }

    if( unlikely( !current ) ) // If lastPacketPa and current == 0 then the first packet is placed in the start of the buffer.
    {
        if (! ctx->initial_packet ) {
            return (fb_packet*)((uint8_t*)ctx->mem + BUCKET_END );
        } else {
            current = (fb_packet*)(ctx->initial_packet);
        }
    }

    // From here the variable nextPacket is expected location of next packet and should be check is not above the current the cache version of the next pointer
    nextPacket = fb_get_raw_next_packet( ctx, current );

    if( (uint64_t)current - (uint64_t)((uint8_t*)ctx->mem + BUCKET_END) + (uint64_t)ctx->recvHwAddr == ctx->lastPacketPa )
    {
        ctx->lastPacketPa = ctx->pl->lastPacket[ ctx->phy_channel_no ]; // Update cache value of last packet
        if( (uint64_t)current - (uint64_t)((uint8_t*)ctx->mem + BUCKET_END) + (uint64_t)ctx->recvHwAddr == ctx->lastPacketPa )  // recheck
        {
            return 0;
        }
    }

    return nextPacket;
}

/** Enter a busy-poll loop while waiting for the next packet.
 * 
 *  @param ctx Device context
 *  @param current Pointer to last packet received
 *  
 *  This function does not return until a packet has been
 *  received.
 * 
 *  @return pointer to received packet
 */
static inline fb_packet* fb_busy_poll_for_next_packet( struct fb_context* ctx, const fb_packet* current )
{
    fb_packet* nextPacket = fb_get_raw_next_packet( ctx, current );
    const uint64_t curLastPacketPa = ctx->lastPacketPa;
    while( curLastPacketPa == (ctx->lastPacketPa = ctx->pl->lastPacket[ ctx->phy_channel_no ]) )
    {
        __asm__ __volatile__( "pause" );
    }
    return nextPacket;
}

/** Enter a busy-poll loop while waiting for the next packet, with timeout.
 * 
 *  @param ctx Device context
 *  @param current Pointer to last packet received
 *  @param maxSpinCount number of busy poll loops to perform (timeout)
 *  
 *  This function spins until a new packet has arrived or the number of busyloop iterations has been reached,
 *  whichever come first.
 * 
 *  @return the pointer to received packet, NULL otherwise.
 */
static inline fb_packet* fb_busy_poll_for_next_packet_timeout(struct fb_context* ctx, const fb_packet* current, uint32_t maxSpinCount)
{
    fb_packet* nextPacket;
    uint32_t i;

    // Our cached lastPacketPa is not set - then update it
    if( unlikely( !ctx->lastPacketPa ) ) // Only true for the first packet
    {
        // Register that we will manually handle incoming traffic
        int ret = ioctl( ctx->fd, CHEMNITZ_ULREAD , NULL );
        if( ret )
        {
            fprintf(stderr, "Failed to register traffic ownership on phys chan %d\n", ctx->phy_channel_no );
        }

        ctx->lastPacketPa = ctx->pl->lastPacket[ ctx->phy_channel_no ]; // Initialize the cache pldma.
        if( unlikely( !ctx->lastPacketPa ) )
            return 0; // No packet yet.
    }

    if( unlikely( !current ) ) // If lastPacketPa and current == 0 then the first packet is placed in the start of the buffer.
    {
        if (! ctx->initial_packet ) {
            return (fb_packet*)((uint8_t*)ctx->mem + BUCKET_END );
        }
        else {
            current = (fb_packet*)(ctx->initial_packet);
        }
    }

    // From here the variable nextPacket is expected location of next packet and should be check is not above the current the cache version of the next pointer
    nextPacket = fb_get_raw_next_packet( ctx, current );

    for(i = 0; i < maxSpinCount; ++i)
    {
        if( (uint64_t)current - (uint64_t)((uint8_t*)ctx->mem + BUCKET_END) + (uint64_t)ctx->recvHwAddr != ctx->lastPacketPa )
            return nextPacket;

        ctx->lastPacketPa = ctx->pl->lastPacket[ ctx->phy_channel_no ]; // Update cache value of last packet
    }

    return NULL;
}

/** Update read marker in FPGA receive buffer.
 * 
 *  Call this function once in a while to free up space in the
 *  receive ring buffer. It is not necessary nor efficient to
 *  call it for every incoming packet.
 * 
 *  @param ctx Device context
 *  @param packet Pointer to most recently processed packet.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
static inline int fb_update_recv_ptr( struct fb_context* ctx, const fb_packet* packet )
{
    uint32_t offset = (uint8_t*)packet - ((uint8_t*)ctx->mem + BUCKET_END);
    ctx->last_readmark = offset;

    if( ioctl( ctx->fd, CHEMNITZ_SET_READMARK, &offset ) )
    {
        return -1;
    }
    return 0;
}

/// @} PacketReceptionRaw

/** **************************************************************************
 * @defgroup PacketTransmission Handle outgoing packets. 
 * @{ 
 * @brief Functions for administrating transmission buckets. 
 *  
 * @warning You must call @ref fb_allocate_tcp_channel for the device context in 
 * question before calling any of the functions in this module.
 */ 

/** Get the send register for the current connection.
 * 
 *  @param ctx Device context
 * 
 *  @return Current connection send register address
 */
static inline volatile uint32_t* fb_send_register( const struct fb_context* ctx )
{
    return (volatile uint32_t*)((uint8_t*)ctx->mem + ctx->sendRegister);
}

/** Returns the numbered bucket for the device context.
 * 
 *  @note Only used when the receive buffer is handled in user space.
 * 
 *  @param ctx Device context
 *  @param bucket_no Number of desired bucket
 * 
 *  @return pointer to fb_bucket
 */
static inline fb_bucket* fb_get_bucket( const struct fb_context* ctx, const uint8_t bucket_no )
{
    return (fb_bucket*)((uint8_t*)ctx->mem + BUCKET_OFFSET( bucket_no ));
}

/** Get the bucket hardware address. 
 *   
 *  This is the value that shall be written to the send register for 
 *  sending the payload in the bucket. 
 * 
 *  @param ctx Device context
 *  @param bucket_no Number of bucket
 * 
 *  @return HW address for the bucket 
 */
static inline uint32_t fb_get_bucket_hwaddr( const struct fb_context* ctx, const uint8_t bucket_no )
{
    return ctx->buckets_addr[ bucket_no ];
}

/// @} PacketTransmission


/** **************************************************************************
 * @defgroup DmaBuffer DMA-able buffers.
 * @{ 
 * @brief Functions for allocating and freeing DMA-able buffers.
 */ 

/** Allocate a buffer suitable to DMA transfer to and from the SmartNIC firmware.
 *
 *  @param ctx Device context
 *  @param size Size in bytes of the buffer
 *  @param hw_addr The HW address to be used by the device for transfer
 * 
 *  @return the user space address of the buffer, for accessing in software.
 */
static inline void *fb_allocate_dma_buffer( struct fb_context* ctx, int size, uint64_t *hw_addr )
{
    fb_dmabuf dmabuf;
    dmabuf.sz = size;

    if( ioctl( ctx->fd, CHEMNITZ_ALLOC_DMABUF, &dmabuf ) )
    {
        return NULL;
    }

    *hw_addr = dmabuf.dma;
    return dmabuf.data;
}

/** Deallocate a buffer previously allocated by fb_allocate_dma_buffer().
 *
 *  @param ctx Device context
 *  @param ptr the user space address of the buffer, as returned by fb_allocate_dma_buffer()
 */
static inline void  fb_deallocate_dma_buffer( struct fb_context* ctx, void *ptr )
{
    ioctl( ctx->fd, CHEMNITZ_DEALLOC_DMABUF, &ptr );
}

/// @} DmaBuffer


/** **************************************************************************
 * @defgroup MultiCast IGMP setup. 
 * @{ 
 * @brief Functions for joining and leaving a multicast group. 
 */ 

/** Join a multicast group.
 * 
 *  @param ctx Device context
 *  @param phy_interface_no Physical interface / port number to operate on
 *  @param group Group to join
 *  @param port Select port number that shall receive multicast traffic.
 *              Calling this function again with a different port number
 *              but otherwise same parameters adds that port number to the
 *              list of ports to receive IGMP traffic on. Port number 0
 *              means receive on all ports.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_igmp_join( const struct fb_context* ctx, unsigned int phy_interface_no, struct in_addr group, uint16_t port );

/** Leave a multicast group.
 * 
 *  @param ctx Device context
 *  @param phy_interface_no Physical interface / port number to operate on
 *  @param group Group to leave
 *  @param port Select port number that shall no longer receive multicast
 *              traffic. Port number 0 means stop all reception from this
 *              IGMP group.
 * 
 *  @return 0 on success, non-zero otherwise.
 */
int fb_igmp_leave( const struct fb_context* ctx, unsigned int phy_interface_no, struct in_addr group, uint16_t port );

/// @} MultiCast

/** **************************************************************************
 * @defgroup ULogicFunc Access to user logic register
 * @{ 
 * @brief Function to read and write user logic registers. There is 32 registers of 64 bits
 */ 

/** Write to user logic register.
  * 
  * @param ctx Device context
  * @param addr The register addr a value between 0 and 31 
  * @param value The 64 bit value to be written
  *
 */
static inline void fb_write_ulogic_register( const struct fb_context *ctx, unsigned int addr, uint64_t value)
{
    *(volatile uint64_t*)((uint64_t*)ctx->regs + ULOGICREGWRITE/8 + addr) = value;
}

/** Read from user logic register.
  *
  * @param ctx Device context
  * @param addr The register addr a value between 0 and 31
  * @param value a pointer to a 64 bits value where the result will be returned
  *
  * @return 0 on success.
 */
static inline int fb_read_ulogic_register( const struct fb_context *ctx, unsigned int addr, uint64_t *value) 
{
    uint64_t temp_value = addr;

    if ( ioctl( ctx->fd, CHEMNITZ_ULOGICREG, &temp_value ) )
    {
        return -1;
    }
    
    *value = temp_value;

    return 0;
}

/// @} ULogicFunc

/** **************************************************************************
 * @defgroup DemoDesignFunc Demo design functions
 * @{ 
 * @brief Functions to activate the demo design in the fpga
 */ 

/** Activate the demo design.
 *
 *  Default the demo design is deactivated.
 *  
 *  @param ctx Device context
 *  @param enabled enable/disable demo design. 
 */
int fb_activate_demo_design( const struct fb_context *ctx, uint32_t enabled );

/// @} DemoDesignFunc


/** **************************************************************************
 * @defgroup DebugFunc Debug functionality. 
 * @{ 
 * @brief Functions for debugging the system. Not used for normal operation.
 */ 

/** Dump a received packet into a file.
 *  
 *  Writes out two files:
 *  - A file containing a dump of the packet pointed to by @a lastpacket
 *    parameter. This file has the extension '.dump'.
 *  - A text file containing various information and the description
 *    pointed to by @a infostr parameter. This file has the extension
 *    '.txt'.
 *  
 *  The name of the file is &lt;basename&gt;:&lt;pid&gt;.ext where '.ext'
 *  is one of the extensions mentioned above.
 * 
 *  @param ctx Device context
 *  @param basefilename Base of file name for the two files written.
 *  @param infostr Pointer to user defined message that will be included
 *                 in the text file
 *  @param lastpacket Pointer to the received packet that shall be dumped
 *                    in the dump file
 *
 *  @return 0 on success, non-zero otherwise.
 */
int fb_dump_receive_info( const struct fb_context *ctx, const char *basefilename, const char *infostr, fb_packet *lastpacket );

/// @} DebugFunc
#ifdef __cplusplus
}
#endif

#endif /* FB_H */
