/**
 * @file fb.c
 * 
 * @brief API implementation.
 *  
 * @copyright Copyright &copy; 2013 Fiberblaze A/S. All rights reserved. 
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "fb.h"

/// @cond PRIVATE_CODE
static const unsigned int RECV_BUFFER_SIZE = 4*1024*1024;
/// @endcond

int fb_init(struct fb_context **ctx, const char *devname ) {
	int rc = 0;

	*ctx = malloc(sizeof(struct fb_context));
	memset(*ctx, 0, sizeof(struct fb_context));

	if (!*ctx) {
		rc = -2;
		return rc;
	}

    if (!devname) {
        (*ctx)->fd = open("/dev/fiberblaze", O_RDWR | O_NONBLOCK);
    } else {
        (*ctx)->fd = open(devname, O_RDWR | O_NONBLOCK);
    }

	if ((*ctx)->fd<0) {
		rc = -1;
		goto errout;
	}

    (*ctx)->regs = mmap( NULL, REGISTERS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, (*ctx)->fd, 0 );
    if( !(*ctx)->regs )
    {
        return -2;
    }

	return 0;
errout: 
	free(*ctx);
	return rc;
}

int fb_destroy(struct fb_context *ctx)
{
	if (ctx) {
		close( ctx->fd );
		if (ctx->mem) {
  			munmap(ctx->mem, BUCKET_END+RECV_BUFFER_SIZE);
		}
		if (ctx->mem) {
  			munmap(ctx->regs, REGISTERS_SIZE);
		}
		free(ctx);
	}
	return 0;
}

/** Common code for allocating channels
 * 
 *  @param ctx Device context
 *  @param phy_interface Physical interface / port number
 *  @param chan Instruct FPGA to use specific TCP channel. Use the value
 *              -1 to let the driver assign the channel.
 *  @param channel_type Type of channel to allocate
 *  @param enableDma should the host memory dma be enabled 
 * 
 *  @return 0 on success, non-zero otherwise.
 */
static int fb_allocate_channel( struct fb_context *ctx, uint32_t phy_interface, int16_t chan, unsigned int channel_type, uint8_t enableDma ) 
{
    int i = 0;
    fb_alloc_info alloc_info;

    // Tell driver to allocate a channel
    alloc_info.type = channel_type;
    alloc_info.channel_no = chan;
    alloc_info.phy_interface_no = phy_interface;
	alloc_info.enableDma = enableDma;
    if( ioctl( ctx->fd, CHEMNITZ_ALLOC_CHANNEL, &alloc_info ) )
    {
        return -1;
    }
    ctx->phy_channel_no = alloc_info.channel_no;

    // Initialize transmit buckets
    for( i = 0; i < FB_NO_BUCKETS; ++i )
    {
        ctx->buckets_addr[ i ] = i;
        if( ioctl( ctx->fd, CHEMNITZ_GET_BUCKET_HWADDR, &ctx->buckets_addr[ i ] ) )
        {
            return -1;
        }
    }

    // Get pointer to send register
    if( ioctl( ctx->fd, CHEMNITZ_GET_BUCKET_REGISTER, &ctx->sendRegister ) )
    {
        return -1;
    }

    // Map shared memory
    ctx->mem = mmap( NULL, BUCKET_END + RECV_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, 0 );
    if( !ctx->mem )
    {
        return -2;
    }
    ctx->pl = (fb_pldma * )((uint8_t * )ctx->mem + REGISTERS_SIZE);
    ctx->st = (fb_status_dma * )((uint8_t * )ctx->mem + REGISTERS_SIZE + PAGE_SIZE);

    // Get receive hardware address
    if( ioctl( ctx->fd, CHEMNITZ_GET_RECV_HWADDR, &ctx->recvHwAddr ) )
    {
        return -1;
    }

    // First packet
    ctx->initial_packet = (uint64_t)(alloc_info.initial_packet ? alloc_info.initial_packet + (uint8_t*)ctx->mem + BUCKET_END - (uint8_t*)ctx->recvHwAddr : 0 );
    ctx->last_readmark = ctx->initial_packet ? (uint8_t*)ctx->initial_packet - ((uint8_t*)ctx->mem + BUCKET_END) : 0;

    // If we get here, all is well
    return 0; 
}

int fb_allocate_tcp_channel( struct fb_context *ctx, uint32_t phy_interface, int16_t chan) 
{
    return fb_allocate_channel( ctx, phy_interface, chan, FB_CHANNEL_TYPE_TCP, 1 /* enable dma */ );
}

int fb_allocate_udp_channel(struct fb_context *ctx, uint32_t phy_interface, int16_t chan, uint8_t enableDma) 
{
    return fb_allocate_channel( ctx, phy_interface, chan, FB_CHANNEL_TYPE_UDP, enableDma );
}

int fb_allocate_monitor_channel(struct fb_context *ctx, uint32_t phy_interface ) 
{
    return fb_allocate_channel( ctx, phy_interface, -1, FB_CHANNEL_TYPE_MON, 1 /* enable dma */ );
}

int fb_allocate_ulogic_channel(struct fb_context *ctx, int16_t chan ) 
{
    return fb_allocate_channel( ctx, 0 , chan, FB_CHANNEL_TYPE_ULOGIC, 1 /* enable dma */ );
}

int fb_set_local_addr( const struct fb_context *ctx, uint32_t phy_interface, struct in_addr local_ip, 
    struct in_addr netmask, struct in_addr gateway, uint8_t mac_addr[MAC_ADDR_LEN]) 
{

	fb_local_addr local;
	local.phy_interface = phy_interface;	
	local.local_ip = ntohl( local_ip.s_addr );
	local.netmask  = ntohl( netmask.s_addr );
	local.gateway  = ntohl( gateway.s_addr );

	if (mac_addr) {
		memcpy(local.mac_addr, mac_addr, MAC_ADDR_LEN);
	} else {
		memset(local.mac_addr, 0, MAC_ADDR_LEN);
	}

	if (ioctl(ctx->fd, CHEMNITZ_SET_LOCAL_ADDR, &local)) {
		return -1;
	}

	return 0;
}

int fb_get_local_addr( const struct fb_context *ctx, uint32_t phy_interface, struct in_addr *local_ip, 
    struct in_addr *netmask, struct in_addr *gateway, uint8_t mac_addr[MAC_ADDR_LEN]) 
{
	fb_local_addr local;

    memset(&local, 0, sizeof(local));

	local.phy_interface = phy_interface;	

	if (ioctl(ctx->fd, CHEMNITZ_GET_LOCAL_ADDR, &local)) {
		return -1;
	}

    local_ip->s_addr = htonl( local.local_ip );
    netmask->s_addr  = htonl( local.netmask );
    gateway->s_addr  = htonl( local.gateway );
    memcpy(mac_addr, local.mac_addr, MAC_ADDR_LEN);

    return 0;
}

int fb_get_link_status( const struct fb_context *ctx, uint64_t *linkStatus)
{
    if ( ioctl(ctx->fd, CHEMNITZ_LINK_STATUS, linkStatus ) ) {
        return -1;
    }
    return 0;
}

/// @cond PRIVATE_CODE
static float conv_temp(uint16_t int_val) {
    return int_val * 503.975 / 65536 - 273.15;
}
/// @endcond PRIVATE_CODE

int fb_get_fpga_temperature( const struct fb_context *ctx, float *minTemp, float *curTemp, float *maxTemp )
{
    fb_temperature temp;
    if ( ioctl(ctx->fd, CHEMNITZ_READ_TEMPERATURE, &temp ) ) {
        return -1;
    }
    *minTemp = conv_temp(temp.minTemp);
    *curTemp = conv_temp(temp.curTemp);
    *maxTemp = conv_temp(temp.maxTemp);

    return 0;
}

int fb_get_fpga_version( const struct fb_context* ctx, uint64_t *fpga_version )
{
    fb_misc misc;
    misc.type = 0;
    misc.value = 0;

    if( ioctl( ctx->fd, CHEMNITZ_READ_VERSION, &misc ) ) 
    {
        return -1;
    }
    *fpga_version = misc.value;
    return 0;
}

int fb_get_board_serial( const struct fb_context* ctx, uint32_t *serial_no )
{
    fb_misc misc;
    misc.type = 1;
    misc.value = 0;
    
    if( ioctl( ctx->fd, CHEMNITZ_READ_VERSION, &misc ) ) 
    {
        return -1;
    }
    *serial_no = misc.value;
    return 0;
}

int fb_ping( const struct fb_context* ctx, unsigned int phy_interface, struct in_addr remote_ip,  uint8_t remote_mac_addr[MAC_ADDR_LEN], uint32_t *pingtime)
{
    int retval = 0;
    static unsigned int sequence = 0;

    fb_ping_info ping_info;
    ping_info.phy_interface  = phy_interface;
    ping_info.remote_ip      = ntohl(remote_ip.s_addr);
    ping_info.sequence       = sequence++;
    if (remote_mac_addr) {
        memcpy( ping_info.remote_mac_addr, remote_mac_addr, MAC_ADDR_LEN );
    } else {
        memset( ping_info.remote_mac_addr, 0, MAC_ADDR_LEN );
    }
    
    if( ( retval = ioctl( ctx->fd, CHEMNITZ_PING, &ping_info ) ) )
    {
        return retval;
    }
    
    // Update remote_mac_address
    if (remote_mac_addr && memcmp( remote_mac_addr, "\x00\x00\x00\x00\x00\x00", MAC_ADDR_LEN ) == 0 ) {
        memcpy( remote_mac_addr, ping_info.remote_mac_addr, MAC_ADDR_LEN );
    }

    *pingtime = ping_info.pingtime;

    return 0;
}

int fb_set_timestamping_mode( const struct fb_context *ctx, uint8_t mode )
{
    uint32_t timemode = mode;

    if ( mode != TIMECONTROL_HARDWARE || mode != TIMECONTROL_SOFTWARE ) {
        return -1;
    }
    
    return ioctl( ctx->fd, CHEMNITZ_TIMERMODE, &timemode );
}

int fb_get_statistic( struct fb_context* ctx, fb_stat_packet *packet ) {
    int rc = 0;
    if( (rc = ioctl( ctx->fd, CHEMNITZ_GET_STAT, packet )) ) 
    {
        return rc;
    }
    return rc;
}
 
int fb_connect( struct fb_context *ctx, struct in_addr remote_ip, uint16_t remote_port, uint16_t local_port, uint8_t mac_addr[MAC_ADDR_LEN])
{
	fb_connection_info connect_info;
	connect_info.remote_ip   = ntohl( remote_ip.s_addr );
	connect_info.remote_port = remote_port;
	connect_info.local_port  = local_port;
	if (mac_addr) {
		memcpy(connect_info.mac_addr, mac_addr, MAC_ADDR_LEN);
	} else {
		memset(connect_info.mac_addr, 0, MAC_ADDR_LEN);
	}

	if (ioctl(ctx->fd, CHEMNITZ_CONNECT, &connect_info)) {
		return -1;
	}

    ctx->lastPacketPa = 0;

	return 0;
}

int fb_listen( struct fb_context *ctx, uint16_t local_port, uint32_t timeout )
{
	fb_listen_info listen_info;
	listen_info.local_port = local_port;
    listen_info.timeout = timeout;
	if (ioctl(ctx->fd, CHEMNITZ_LISTEN, &listen_info)) {
		return -1;
	}

    ctx->lastPacketPa = 0;

	return 0;
}

int fb_disconnect( struct fb_context *ctx) 
{
	if (ioctl(ctx->fd, CHEMNITZ_DISCONNECT, NULL)) {
		return -1;
	}

    ctx->lastPacketPa = 0;

	return 0;
}

int fb_get_last_ack_num( const struct fb_context* ctx, uint32_t* last_ack_num )
{
    if (ioctl(ctx->fd, CHEMNITZ_LAST_ACK_NO, last_ack_num)) {
        return -1;
    }

    return 0;
}


int fb_igmp_join( const struct fb_context *ctx, unsigned int phy_interface_no, struct in_addr group, uint16_t port )
{
    fb_igmp_info info;

    info.action = IGMP_JOIN;
    info.group  = ntohl( group.s_addr );
    info.port = port;

	if (ioctl(ctx->fd, CHEMNITZ_IGMPINFO, &info)) {
		return -1;
	}

    return 0;
}

int fb_igmp_leave( const struct fb_context *ctx, unsigned int phy_interface_no, struct in_addr group, uint16_t port )
{
    fb_igmp_info info;

    info.action = IGMP_LEAVE;
    info.group  = ntohl( group.s_addr );
    info.port = port;

	if (ioctl(ctx->fd, CHEMNITZ_IGMPINFO, &info)) {
		return -1;
	}

    return 0;
}

int fb_dump_receive_info( const struct fb_context *ctx, const char *basefilename, const char *infostr, fb_packet *lastpacket ) 
{
    
    FILE *file;
    char outstr[256];
    
    snprintf(outstr, 256, "%s.%d.dump", basefilename, getpid());

    if ( ! ( file = fopen(outstr, "wb") ) ) {
        fprintf(stderr, "Cannot open '%s' for writing in %s:%d\n", outstr, __FILE__, __LINE__ );
        return -1;
    };

    if ( 1 != fwrite((uint8_t*)ctx->mem+BUCKET_END, RECV_BUFFER_SIZE, 1, file)) {
        fprintf(stderr, "Cannot write the full dump of the receive buffer in %s:%d\n", __FILE__, __LINE__ );
        fclose(file);
        return -1;
    }
    
    fclose(file);

    // Get time of day
    time_t t;
    struct tm *tmp;

    t = time(NULL);
    if ( ! ( tmp = localtime(&t) ) ) {
        fprintf(stderr, "Cannot get localtime in %s:%d\n", __FILE__, __LINE__ ); 
        return -1;
    }

    snprintf(outstr, 256, "%s.%d.txt", basefilename, getpid());
    if ( ! ( file = fopen(outstr, "w") ) ) {
        fprintf(stderr, "Can't open '%s' for writing in %s:%d\n", outstr, __FILE__, __LINE__ );
        return -1;
    };
    

    fprintf( file, "Filename: %s\n", outstr );

    if (strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
    }
    
    fprintf( file, "Time: %s\n", outstr);
    fprintf( file, "Info: %s\n", infostr );
    fprintf( file, "Dma no ( phy_channel_no ): %d\n", ctx->phy_channel_no );
    fprintf( file, "Receive buffer size: 0x%x\n", RECV_BUFFER_SIZE );
    fprintf( file, "LastPacket cached pldma: 0x%016lx offset: 0x%lx\n", ctx->lastPacketPa, ctx->lastPacketPa - ctx->recvHwAddr );
    uint64_t lastPacket = ctx->pl->lastPacket[ctx->phy_channel_no];
    fprintf( file, "LastPacket according to pldma: 0x%016lx offset: 0x%lx\n", lastPacket, lastPacket - ctx->recvHwAddr );
    fprintf( file, "Last packet offset according to argument: offset: 0x%lx\n", (uint8_t*)lastpacket - ((uint8_t*)ctx->mem + BUCKET_END) );

    fclose(file);

    return 0;
}

int fb_activate_demo_design( const struct fb_context *ctx, uint32_t enabled )
{
    fb_write_ulogic_register( ctx, 0, enabled );

    return 0;
}
