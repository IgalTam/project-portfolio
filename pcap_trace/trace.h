#include <pcap/pcap.h>
#include <arpa/inet.h>

/* misc macros*/

#define WRITE_BUF_LEN 100
#define MAC_LEN 6
#define ENET_TYPE_LEN 2
#define ENET_PREAMBLE 8
#define IP_LEN 4
#define ENET_HDR_LEN 14
#define IP_HDR_LEN 20
#define SEQ_NUM_LEN 4
#define ACK_NUM_LEN 4
#define TCP_PS_HDR_LEN 12
#define PORT_LEN 2

/* Ethernet header properties*/

typedef struct rd_enet_hdr {
    uint16_t dst_mac[MAC_LEN];
    uint16_t src_mac[MAC_LEN];
    char *enet_type;
    uint16_t type_flag;
    uint16_t enet_type_big;
} enet_hdr;

/* ARP header properties*/

typedef struct rd_arp_hdr {
    char *opcode;
    uint16_t sndr_mac[MAC_LEN];
    uint16_t sndr_ip[IP_LEN];
    uint16_t rcvr_mac[MAC_LEN];
    uint16_t rcvr_ip[IP_LEN];
} arp_hdr;

/* IP header properties*/

typedef struct rd_ip_hdr {
    uint16_t ihl;
    uint16_t tos[2];
    uint8_t tos_big;
    uint16_t ttl;
    uint32_t tot_len;
    char *protocol;
    uint16_t prot_val;
    uint16_t chksm[2];
    char *chksm_stat;
    uint16_t sndr_ip[IP_LEN];
    uint32_t sndr_ip_big;
    uint16_t dst_ip[IP_LEN];
    uint32_t dst_ip_big;
    uint16_t prot_flag;
} ip_hdr;

/* TCP header properties*/

typedef struct rd_tcp_hdr {
    uint16_t src_port[PORT_LEN];
    char *src_port_named;
    uint16_t dst_port[PORT_LEN];
    char *dst_port_named;
    uint32_t seq_num_big;
    uint32_t ack_num_big;
    char *syn_flg;
    char *rst_flg;
    char *fin_flg;
    uint16_t win_size[2];
    uint16_t chksum[2];
    uint16_t chksum_big;
    char *chksum_stat;
} tcp_hdr;

/* UDP header properties*/

typedef struct rd_udp_hdr {
    uint16_t src_port[PORT_LEN];
    char *src_port_named;
    uint16_t dst_port[PORT_LEN];
    char *dst_port_named;
} udp_hdr;

/* function definitions*/

void usage(char *funcName);
char *config_out_file_name(char *filename);
enet_hdr *confEnetHdr();
arp_hdr *confArpHdr();
ip_hdr *confIpHdr();
tcp_hdr *confTcpHdr();
udp_hdr *confUdpHdr();
char *getNamedPort(uint16_t *port_num);
const u_char *wrEnetInfo(int fd, char *buf, const u_char *pkt_data, enet_hdr *ret_h);
void wrArpInfo(int fd, char *buf, const u_char *pkt_data, arp_hdr *ret_h);
const u_char *wrIpInfo(int fd, char *buf, const u_char *pkt_data, ip_hdr *ret_h);
void wrIcmpInfo(int fd, char *buf, const u_char *pkt_data);
void wrTcpInfo(int fd, char *buf, const u_char *pkt_data, ip_hdr *ip_h, tcp_hdr *ret_h);
void wrUdpInfo(int fd, char *buf, const u_char *pkt_data, udp_hdr *ret_h);