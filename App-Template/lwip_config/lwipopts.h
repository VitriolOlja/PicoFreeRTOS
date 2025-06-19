#ifndef _LWIPOPTS_EXAMPLE_COMMONH_H
#define _LWIPOPTS_EXAMPLE_COMMONH_H
// Common settings used in most of the pico_w examples
// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html for details)
// allow override in some examples


#include "pico/stdlib.h"

// === Core System Configuration ===

// NO_SYS=1 tells LWIP to operate without an OS (no threading or RTOS required)
#ifndef NO_SYS
#define NO_SYS 0
#endif

/*
  Max concurrent connections hint
  supports max 6 active tcp connections.
*/
#ifndef MAX_CONCURRENT_CX_HINT
#define MAX_CONCURRENT_CX_HINT      6
#endif

// at most 1 TCP server connection
#define NUM_SERVER_HINT             1


// Disable BSD-style socket API support unless overridden
#ifndef LWIP_SOCKET
#define LWIP_SOCKET 0
#endif

// If we're using polling mode for CYW43, allow use of malloc()
// Otherwise, must avoid it because background IRQs can't allocate safely
#if PICO_CYW43_ARCH_POLL
#define MEM_LIBC_MALLOC 1
#else

// MEM_LIBC_MALLOC is incompatible with non polling versions
#define MEM_LIBC_MALLOC 0
#endif

// Memory alignment for CPU (4 bytes = 32-bit alignment)
#define MEM_ALIGNMENT 4
#ifndef MEM_SIZE

// Default heap size for LWIP internal allocations (pbufs, TCP queues, etc.)
#define MEM_SIZE 4000
#endif

// === Memory Pool Configuration ===

// Number of TCP segments in memory
#define MEMP_NUM_TCP_SEG 32

// Number of queued ARP packets waiting for address resolution
#define MEMP_NUM_ARP_QUEUE 10

// Number of buffers in the pbuf pool
#define PBUF_POOL_SIZE 24

// === Protocol Options ===

// Enable ARP (Address Resolution Protocol)
#define LWIP_ARP 1

// Enable Ethernet support (required for Pico W over WiFi bridge)
// CYW43 driver presents itself to LWIP as an Ethernet device
// LWIP thinks it’s sending/receiving Ethernet frames, and the CYW43 library translates that to/from WiFi.
#define LWIP_ETHERNET 1


// Enable ICMP (used for ping)
#define LWIP_ICMP 1

// Enable RAW sockets (used by SNTP and custom protocols)
#define LWIP_RAW 1

// TCP maximum segment size (must fit within MTU - headers)
#define TCP_MSS 1460

// TCP window size: how much data can be unacknowledged at once
#define TCP_WND (8 * TCP_MSS)

// Send buffer size (how much data we can queue for transmission)
#define TCP_SND_BUF (8 * TCP_MSS)

// Number of TCP segments that can be queued
#define TCP_SND_QUEUELEN ((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))

// Enable network interface status change callback
#define LWIP_NETIF_STATUS_CALLBACK 1

// Enable link up/down callback
#define LWIP_NETIF_LINK_CALLBACK 1

// Allow setting hostname on network interfaces
#define LWIP_NETIF_HOSTNAME 1

// changed Disable netconn API (higher-level but not needed for NO_SYS mode)
//enabled
#define LWIP_NETCONN 1

// === Statistics (disabled to save memory) ===
#define MEM_STATS 0
#define SYS_STATS 0
#define MEMP_STATS 0
#define LINK_STATS 0


// #define ETH_PAD_SIZE 2 // Optional padding to align Ethernet frames

// Use fast checksum algorithm (LWIP_CHKSUM_ALGORITHM = 3 is hardware-optimized)
#define LWIP_CHKSUM_ALGORITHM 3

// Enable DHCP (dynamic IP addressing)
#define LWIP_DHCP 1

// Enable IPv4 support (LWIP is modular and can disable it)
#define LWIP_IPV4 1

// Enable TCP
#define LWIP_TCP 1

// Enable UDP
#define LWIP_UDP 1

// Enable DNS resolver
#define LWIP_DNS 1

// Enable TCP keep-alive messages (detect broken connections)
#define LWIP_TCP_KEEPALIVE 1

// Improve performance by sending packets in a single pbuf (optimization)
#define LWIP_NETIF_TX_SINGLE_PBUF 1

// Disable ARP check during DHCP (may interfere with CYW43 behavior)
#define DHCP_DOES_ARP_CHECK 0

// Disable Address Conflict Detection check during DHCP (used in some setups)
#define LWIP_DHCP_DOES_ACD_CHECK 0

// === Debug and Logging ===
#ifndef NDEBUG
#define LWIP_DEBUG 1
#define LWIP_STATS 1
#define LWIP_STATS_DISPLAY 1
#endif

// Define the LWIP debug flags
#define LWIP_DBG_ON      0x80U
#define LWIP_DBG_OFF     0x00U
#define LWIP_DBG_TRACE   0x40U
#define LWIP_DBG_STATE   0x20U
#define LWIP_DBG_FRESH   0x10U

// A global "all debug types on" flag; not quite sure how this is used yet
#define LWIP_DBG_TYPES_ON (LWIP_DBG_ON | LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH)

// Set debug level for various modules (OFF = 0, ON = LWIP_DBG_ON)
// These can be set to LWIP_DBG_ON to see verbose logs
#define ETHARP_DEBUG LWIP_DBG_OFF
#define NETIF_DEBUG LWIP_DBG_OFF
#define PBUF_DEBUG LWIP_DBG_OFF
#define API_LIB_DEBUG LWIP_DBG_OFF
#define API_MSG_DEBUG LWIP_DBG_OFF
#define SOCKETS_DEBUG LWIP_DBG_OFF
#define ICMP_DEBUG LWIP_DBG_OFF
#define INET_DEBUG LWIP_DBG_OFF
#define IP_DEBUG LWIP_DBG_OFF
#define IP_REASS_DEBUG LWIP_DBG_OFF
#define RAW_DEBUG LWIP_DBG_OFF
#define MEM_DEBUG LWIP_DBG_OFF
#define MEMP_DEBUG LWIP_DBG_OFF
#define SYS_DEBUG LWIP_DBG_OFF
#define TCP_DEBUG LWIP_DBG_OFF
#define TCP_INPUT_DEBUG LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF
#define TCP_RTO_DEBUG LWIP_DBG_OFF
#define TCP_CWND_DEBUG LWIP_DBG_OFF
#define TCP_WND_DEBUG LWIP_DBG_OFF
#define TCP_FR_DEBUG LWIP_DBG_OFF
#define TCP_QLEN_DEBUG LWIP_DBG_OFF
#define TCP_RST_DEBUG LWIP_DBG_OFF
#define UDP_DEBUG LWIP_DBG_OFF
#define TCPIP_DEBUG LWIP_DBG_OFF
#define PPP_DEBUG LWIP_DBG_OFF
#define SLIP_DEBUG LWIP_DBG_OFF
#define DHCP_DEBUG LWIP_DBG_OFF


#define DEFAULT_TCP_RECVMBOX_SIZE 128

#define SNTP_SUPPORT      1
#define SNTP_SERVER_DNS   1
void sntpSetTimeSec(uint32_t sec);
#define SNTP_SET_SYSTEM_TIME(sec) sntpSetTimeSec(sec)
//MEMP_NUM_SYS_TIMEOUTS Needs to be one larger than default for SNTP
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1)

// SNTP updates automatically using a timer managed internall by the LWIP stack
#define SNTP_UPDATE_DELAY 60*60*1000

// #define CYW43_VDEBUG(...) printf(__VA_ARGS__)
#define CYW43_VERBOSE_DEBUG 1
#define CYW43_DEBUG
#define CYW43_INFO
#define PICO_CYW43_ARCH_DEBUG_ENABLED 1


#endif /* __LWIPOPTS_H__ */