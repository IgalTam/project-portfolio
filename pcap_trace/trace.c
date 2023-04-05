#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/ether.h>
#include "smartalloc.h"
#include "trace.h"
#include "checksum.h"

void usage(char *funcName) {
    printf("Usage: %s filename", funcName);
}

char *config_out_file_name(char *filename) {
    /* configures the input .pcap extension into a .out.txt extension
    for the output file*/
    char *ret;
    if(! (ret = malloc(sizeof(char)*(strlen(filename) + 5) ) ) ) {
        perror(ret);
        exit(1);
    }
    snprintf(ret, strlen(filename)-3, "%s", filename);
    strncat(ret, "out.txt\0", 8);
    return ret;
}

enet_hdr *confEnetHdr() {
  /* initializes an enet_hdr instance*/
  enet_hdr *ret_h;
  /* allocate memory*/ 
  if(!(ret_h = malloc(sizeof(enet_hdr)))) {
  	perror("malloc");
        exit(1);
  } 
  return ret_h; 
}

arp_hdr *confArpHdr() {
  /* initializes an arp_hdr instance*/
  arp_hdr *ret_h;
  /* allocate memory*/ 
  if(!(ret_h = malloc(sizeof(arp_hdr)))) {
  	perror("malloc");
        exit(1);
  } 
  return ret_h;
}

ip_hdr *confIpHdr() {
  /* initializes an ip_hdr instance*/
  ip_hdr *ret_h;
  /* allocate memory*/ 
  if(!(ret_h = malloc(sizeof(ip_hdr)))) {
  	perror("malloc");
        exit(1);
  } 
  return ret_h;
}

tcp_hdr *confTcpHdr() {
  /* initializes an tcp_hdr instance*/
  tcp_hdr *ret_h;
  /* allocate memory*/ 
  if(!(ret_h = malloc(sizeof(tcp_hdr)))) {
  	perror("malloc");
        exit(1);
  } 
  return ret_h;
}

udp_hdr *confUdpHdr() {
  /* initializes an udp_hdr instance*/
  udp_hdr *ret_h;
  /* allocate memory*/ 
  if(!(ret_h = malloc(sizeof(udp_hdr)))) {
  	perror("malloc");
        exit(1);
  } 
  return ret_h;
}

char *getNamedPort(uint16_t *port_num) {
  /* returns string if special port name*/
  uint16_t port = (port_num[0] << 1) | port_num[1];

  if(port == 80) // HTTP
    return "HTTP";
  else if(port == 23) // Telnet
    return "TELNET";
  else if(port == 21) // FTP
    return "FTP";
  else if(port == 110) // POP3
    return "POP3";
  else if(port == 25) // SMTP
    return "SMTP";
  else // non-special port
    return NULL;
}

const u_char *wrEnetInfo(int fd, char *buf, const u_char *pkt_data, enet_hdr *ret_h) {
   /* extract Enet header info from given packet data*/
   uint16_t e_type_num[4], e_type_whl;
   int i=0;//, ret_f; 
   
   /* extract MACs and type*/
   while(i < MAC_LEN)
     ret_h->dst_mac[i++] = *(pkt_data++);
   i = 0;
   while(i < MAC_LEN)
     ret_h->src_mac[i++] = *(pkt_data++);
   i = 0;
   memcpy(&ret_h->enet_type_big, pkt_data, ENET_TYPE_LEN);
   ret_h->enet_type_big = ntohs(ret_h->enet_type_big);
   while(i < ENET_TYPE_LEN) {
     e_type_num[i++] = *(pkt_data++); 
   }

   /* get Enet type*/
   e_type_whl = (e_type_num[0] << 4) | (e_type_num[1]);
   if(e_type_whl == 0x86) { // ARP
     ret_h->enet_type = "ARP";
     ret_h->type_flag = 0;
   } else if(e_type_whl == 0x80) { // IP
     ret_h->enet_type = "IP";
     ret_h->type_flag = 1;
   } else {
     ret_h->enet_type = "Unknown";
     ret_h->type_flag = 2;
   }

   /* write Enet header to output*/
   write(fd, "\tEthernet Header\n", 17);
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tDest MAC: %x:%x:%x:%x:%x:%x\n", ret_h->dst_mac[0], 
	ret_h->dst_mac[1], ret_h->dst_mac[2], ret_h->dst_mac[3], 
	ret_h->dst_mac[4], ret_h->dst_mac[5]);   
   write(fd, buf, strlen(buf)); // dest MAC
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tSource MAC: %x:%x:%x:%x:%x:%x\n", ret_h->src_mac[0], 
	ret_h->src_mac[1], ret_h->src_mac[2], ret_h->src_mac[3], 
	ret_h->src_mac[4], ret_h->src_mac[5]);   
   write(fd, buf, strlen(buf)); // src MAC
   memset(buf,0,WRITE_BUF_LEN);
   if(!ret_h->type_flag || ret_h->type_flag == 1)
    snprintf(buf, WRITE_BUF_LEN, "\t\tType: %s\n\n", ret_h->enet_type);
  else
    snprintf(buf, WRITE_BUF_LEN, "\t\tType: 0x%x\n\n", ret_h->enet_type_big);
   write(fd, buf, strlen(buf)); // type
   memset(buf,0,WRITE_BUF_LEN);

   return pkt_data;  
}

void wrArpInfo(int fd, char *buf, const u_char *pkt_data, arp_hdr *ret_h) {
  /* extract ARP header info from given packet data*/
   uint16_t op_type_num[2], op_type_whl;
   int i=0; 
    
   /* extract opcode*/
   pkt_data = pkt_data + 20; // skip unnecessary info for output file
   op_type_num[0] = *(pkt_data++);
   op_type_num[1] = *(pkt_data++);

   /* extract sender MAC and IP addr*/
   while(i < MAC_LEN)
     ret_h->sndr_mac[i++] = *(pkt_data++);
   i = 0;
   while(i < IP_LEN) { 
     ret_h->sndr_ip[i++] = *(pkt_data++);
   }
   i = 0;
   
    /* extract target MAC and IP addr*/
   while(i < MAC_LEN)
     ret_h->rcvr_mac[i++] = *(pkt_data++);
   i = 0;
   while(i < IP_LEN)
     ret_h->rcvr_ip[i++] = *(pkt_data++);
   i = 0;
   
   /* get opcode*/
   op_type_whl = (op_type_num[0] << 4) | (op_type_num[1]);
   if(op_type_whl == 0x01) { // request
     ret_h->opcode = "Request";
   } else if(op_type_whl == 0x02) { // reply
     ret_h->opcode = "Reply";
   } else {
     ret_h->opcode = "Unknown";
   }

   /* write ARP header to output*/
   write(fd, "\tARP Header\n", 12);
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tOpcode: %s\n", ret_h->opcode);
   write(fd, buf, strlen(buf)); // opcode
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tSender MAC: %x:%x:%x:%x:%x:%x\n", 
        ret_h->sndr_mac[0], ret_h->sndr_mac[1], ret_h->sndr_mac[2], 
        ret_h->sndr_mac[3], ret_h->sndr_mac[4], ret_h->sndr_mac[5]);   
   write(fd, buf, strlen(buf)); // sender MAC
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tSender IP: %i.%i.%i.%i\n", 
        ret_h->sndr_ip[0], ret_h->sndr_ip[1], ret_h->sndr_ip[2], 
	ret_h->sndr_ip[3]);
   write(fd, buf, strlen(buf)); // sender IP
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tTarget MAC: %x:%x:%x:%x:%x:%x\n", 
	ret_h->rcvr_mac[0], ret_h->rcvr_mac[1], ret_h->rcvr_mac[2], 
	ret_h->rcvr_mac[3], ret_h->rcvr_mac[4], ret_h->rcvr_mac[5]);   
   write(fd, buf, strlen(buf)); // target MAC
   memset(buf,0,WRITE_BUF_LEN);
   snprintf(buf, WRITE_BUF_LEN, "\t\tTarget IP: %i.%i.%i.%i", 
        ret_h->rcvr_ip[0], ret_h->rcvr_ip[1], ret_h->rcvr_ip[2], 
	ret_h->rcvr_ip[3]);
   write(fd, buf, strlen(buf)); // sender IP
   memset(buf,0,WRITE_BUF_LEN); 
}

const u_char *wrIpInfo(int fd, char *buf, const u_char *pkt_data, ip_hdr *ret_h) {
  /* extract IP header info from given packet data*/
  unsigned short chksum_res;
  int i=0;
  
  // for use in in_cksum()
  short unsigned int *placeholder = (short unsigned int *) pkt_data; 
  
  /* extract TOS*/
  memcpy(&ret_h->ihl, pkt_data++, 1); //skip version number, extract IHL
  ret_h->ihl &= 0x0F;
  memcpy(&ret_h->tos_big, pkt_data++, 1);

  /* extract total length and TTL*/
  memcpy(&ret_h->tot_len, pkt_data, 2);
  ret_h->tot_len = ntohs(ret_h->tot_len);
  pkt_data += 6; //skip identif., fragment offset & flags
  ret_h->ttl = *(pkt_data++);

  /* extract protocol*/
  if(0x01 == (ret_h->prot_val = *(pkt_data++))) { //ICMP
    ret_h->protocol = "ICMP";
    ret_h->prot_flag = 0;
  } else if(0x06 == ret_h->prot_val) { //TCP
    ret_h->protocol = "TCP";
    ret_h->prot_flag = 1;
  } else if(0x11 == ret_h->prot_val) { //UDP
    ret_h->protocol = "UDP";
    ret_h->prot_flag = 2;
  } else {
    ret_h->protocol = "Unknown";
    ret_h->prot_flag = 3;
  }

  /* extract checksum*/
  ret_h->chksm[0] = *(pkt_data++);
  ret_h->chksm[1] = *(pkt_data++);

  /* evaluate checksum*/
  chksum_res = in_cksum(placeholder, ret_h->ihl * 4);
  if(!chksum_res)
    ret_h->chksm_stat = "Correct";
  else
    ret_h->chksm_stat = "Incorrect";
  
  /* extract sender and dest IP*/
  memcpy(&ret_h->sndr_ip_big, pkt_data, IP_LEN);
  ret_h->sndr_ip_big = ntohl(ret_h->sndr_ip_big);
  // dprintf(fd, "\nsndr ip test: %x\n", ret_h->sndr_ip_big);
  while(i < IP_LEN)
    ret_h->sndr_ip[i++] = *(pkt_data++);
   i = 0;
  memcpy(&ret_h->dst_ip_big, pkt_data, IP_LEN);
  ret_h->dst_ip_big = ntohl(ret_h->dst_ip_big);
  while(i < IP_LEN)
    ret_h->dst_ip[i++] = *(pkt_data++);
  i = 0;

  /* skip packet pointer past options, if they exist*/
  pkt_data += (ret_h->ihl * 4 - IP_HDR_LEN);

  /* write IP header to output*/
  write(fd, "\tIP Header\n", 11);
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tTOS: 0x%x\n", ret_h->tos_big);
  //(ret_h->tos[0] << 0) | 
  //       ret_h->tos[1]);
  write(fd, buf, strlen(buf)); // TOS
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tTTL: %i\n", ret_h->ttl);
  write(fd, buf, strlen(buf)); // TTL
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tProtocol: %s\n", ret_h->protocol);
  write(fd, buf, strlen(buf)); // protocol
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tChecksum: %s (0x%x)\n", ret_h->chksm_stat,
        (ret_h->chksm[0] << 8) | ret_h->chksm[1]);
  write(fd, buf, strlen(buf)); // checksum
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tSender IP: %i.%i.%i.%i\n", ret_h->sndr_ip[0],
        ret_h->sndr_ip[1], ret_h->sndr_ip[2], ret_h->sndr_ip[3]);
  write(fd, buf, strlen(buf)); // sender IP
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tDest IP: %i.%i.%i.%i", ret_h->dst_ip[0],
        ret_h->dst_ip[1], ret_h->dst_ip[2], ret_h->dst_ip[3]);
  write(fd, buf, strlen(buf)); // destination IP
  memset(buf,0,WRITE_BUF_LEN);

  if(ret_h->prot_flag != 3) // continue output if protocol is known
    write(fd, "\n\n", 2);

  return pkt_data;
}

void wrIcmpInfo(int fd, char *buf, const u_char *pkt_data) {
  /* extract type from ICMP header*/
  uint16_t type;

  /* write ICMP header to output*/
  write(fd, "\tICMP Header\n", 13);
  memset(buf,0,WRITE_BUF_LEN);
  if(0x0 == (type = *pkt_data))
    write(fd, "\t\tType: Reply", 13);
  else if(0x8 == type)
    write(fd, "\t\tType: Request", 15);
  else
    write(fd, "\t\tType: Unknown", 15);
  memset(buf,0,WRITE_BUF_LEN);
}

void wrTcpInfo(int fd, char *buf, const u_char *pkt_data, ip_hdr *ip_h, 
    tcp_hdr *ret_h) {
  /* extract TCP header info from given data packet*/
  uint16_t tcp_len=0, data_off, tcp_flgs;
  int i=0;
  uint32_t temp;

  // for use in in_cksum()
  unsigned short chksum_res;
  const u_char *hdr_plchldr = pkt_data;
  uint8_t *tcp_ps_hdr;

  /* extract source port*/
  while(i < PORT_LEN) {
    ret_h->src_port[i++] = *(pkt_data++);
  }
  ret_h->src_port_named = getNamedPort(ret_h->src_port);
  i=0;

  /* extract destination port*/
   while(i < PORT_LEN) {
    ret_h->dst_port[i++] = *(pkt_data++);
  }
  ret_h->dst_port_named = getNamedPort(ret_h->dst_port);
  i = 0;

  /* extract sequence number*/
  memcpy(&ret_h->seq_num_big, pkt_data, SEQ_NUM_LEN);
  pkt_data += SEQ_NUM_LEN;

  /* extract acknowledgement number*/
  memcpy(&ret_h->ack_num_big, pkt_data, ACK_NUM_LEN);
  pkt_data += ACK_NUM_LEN;

  /* store data offset and reserved byte in local vars*/
  memcpy(&data_off, pkt_data, 1);
  pkt_data++;

  /* extract flags, store in local var*/
  tcp_flgs = *(pkt_data++);
  if(!(tcp_flgs & 0x04)) {
    ret_h->rst_flg = "No";
  } else {
    ret_h->rst_flg = "Yes";
  }
  if(!(tcp_flgs & 0x02)) {
    ret_h->syn_flg = "No";
  } else {
    ret_h->syn_flg = "Yes";
  }
  if(!(tcp_flgs & 0x01)) {
    ret_h->fin_flg = "No";
  } else {
    ret_h->fin_flg = "Yes";
  }

  /* extract window size*/
  ret_h->win_size[0] = *(pkt_data++);
  ret_h->win_size[1] = *(pkt_data++);

  /* extract checksum*/
  memcpy(&ret_h->chksum_big, pkt_data, 2);
  ret_h->chksum_big = ntohs(ret_h->chksum_big);
  pkt_data += 2;

  /* construct TCP pseudo-header for checksum eval*/
  tcp_len = ip_h->tot_len - 4*ip_h->ihl;
  if(!(tcp_ps_hdr = malloc(sizeof(uint16_t) * (TCP_PS_HDR_LEN + tcp_len)))) {
    perror("malloc");
    exit(1);
  }
  temp = htonl(ip_h->sndr_ip_big);
  memcpy(tcp_ps_hdr, &temp, IP_LEN);
  tcp_ps_hdr += IP_LEN;
  temp = htonl(ip_h->dst_ip_big);
  memcpy(tcp_ps_hdr, &temp, IP_LEN);
  tcp_ps_hdr += IP_LEN;
  memset(tcp_ps_hdr, 0, 1);
  tcp_ps_hdr++;
  memcpy(tcp_ps_hdr++, &ip_h->prot_val, 1);
  uint16_t tcp_len_2 = htons(tcp_len);
  memcpy(tcp_ps_hdr, &tcp_len_2, 2);
  tcp_ps_hdr += 2;
  memcpy(tcp_ps_hdr, hdr_plchldr, tcp_len);
  tcp_ps_hdr -= TCP_PS_HDR_LEN; // reset pseudoheader pointer to beginning

  /* evaluate checksum*/
  chksum_res = in_cksum((short unsigned int *) tcp_ps_hdr, 
    TCP_PS_HDR_LEN + tcp_len);
  if(chksum_res == 0)
    ret_h->chksum_stat = "Correct";
  else
    ret_h->chksum_stat = "Incorrect";

  /* clean up pseudo header*/
  free(tcp_ps_hdr);

  /* write TCP header to output*/
  write(fd, "\tTCP Header\n", 12);
  memset(buf,0,WRITE_BUF_LEN);
  if(ret_h->src_port_named) {
    snprintf(buf, WRITE_BUF_LEN, "\t\tSource Port:  %s\n", 
      ret_h->src_port_named);
    write(fd, buf, strlen(buf));
  } else {
    snprintf(buf, WRITE_BUF_LEN, "\t\tSource Port:  %i\n", 
      (ret_h->src_port[0] << 8) | ret_h->src_port[1]);
    write(fd, buf, strlen(buf));
  }
  memset(buf,0,WRITE_BUF_LEN);
  if(ret_h->dst_port_named) {
    snprintf(buf, WRITE_BUF_LEN, "\t\tDest Port:  %s\n", 
      ret_h->dst_port_named);
    write(fd, buf, strlen(buf));
  } else {
    snprintf(buf, WRITE_BUF_LEN, "\t\tDest Port:  %i\n", 
      (ret_h->dst_port[0] << 8) | ret_h->dst_port[1]);
    write(fd, buf, strlen(buf));
  }
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tSequence Number: %u\n", 
    htonl(ret_h->seq_num_big));
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tACK Number: %u\n", 
    htonl(ret_h->ack_num_big));
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tSYN Flag: %s\n", ret_h->syn_flg);
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tRST Flag: %s\n", ret_h->rst_flg);
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tFIN Flag: %s\n", ret_h->fin_flg);
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tWindow Size: %i\n",
    (ret_h->win_size[0] << 8) | ret_h->win_size[1]);
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
  snprintf(buf, WRITE_BUF_LEN, "\t\tChecksum: %s (0x%x)", ret_h->chksum_stat,
      ret_h->chksum_big);
  write(fd, buf, strlen(buf));
  memset(buf,0,WRITE_BUF_LEN);
}

void wrUdpInfo(int fd, char *buf, const u_char *pkt_data, udp_hdr *ret_h) {
  /* extract UDP header info from given data packet*/
  int i=0;

  /* extract source port*/
  while(i < PORT_LEN)
    ret_h->src_port[i++] = *(pkt_data++);
  ret_h->src_port_named = getNamedPort(ret_h->src_port);
  i=0;

  /* extract destination port*/
  while(i < PORT_LEN)
    ret_h->dst_port[i++] = *(pkt_data++);
  ret_h->dst_port_named = getNamedPort(ret_h->dst_port);
  
  /* write UDP header to output*/
  write(fd, "\tUDP Header\n", 12);
  memset(buf,0,WRITE_BUF_LEN);
  if(ret_h->src_port_named) {
    snprintf(buf, WRITE_BUF_LEN, "\t\tSource Port:  %s\n", 
      ret_h->src_port_named);
    write(fd, buf, strlen(buf));
  } else {
    snprintf(buf, WRITE_BUF_LEN, "\t\tSource Port:  %i\n", 
      (ret_h->src_port[0] << 8) | ret_h->src_port[1]);
    write(fd, buf, strlen(buf));
  } 
  memset(buf,0,WRITE_BUF_LEN);
  if(ret_h->dst_port_named) {
    snprintf(buf, WRITE_BUF_LEN, "\t\tDest Port:  %s", 
      ret_h->dst_port_named);
    write(fd, buf, strlen(buf));
  } else {
    snprintf(buf, WRITE_BUF_LEN, "\t\tDest Port:  %i", 
      (ret_h->dst_port[0] << 8) | ret_h->dst_port[1]);
    write(fd, buf, strlen(buf));
  }
  memset(buf,0,WRITE_BUF_LEN);
}

int main(int argc, char *argv[]) {
  /* main function*/
  int res, f_out=STDOUT_FILENO, pkt_count=0; //, enet_res, ip_res;
  char *f_out_name, *wr_buf, errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *trace_ptr;
  struct pcap_pkthdr *pkt_hdr;
  const u_char *pkt_data, *pkt_data_old;
  //struct ether_header *eptr;

  /* parse arguments*/
  if(argc != 2) {
    usage(argv[0]);
    exit(1);
  }

  /* open trace file for reading*/
  trace_ptr = pcap_open_offline(argv[1], errbuf);

  /* configure structs*/
  enet_hdr *enet_h = confEnetHdr();
  arp_hdr *arp_h = confArpHdr();
  ip_hdr *ip_h = confIpHdr();
  tcp_hdr *tcp_h = confTcpHdr();
  udp_hdr *udp_h = confUdpHdr();      

  /* open output file*/
  f_out_name = config_out_file_name(argv[1]); //use this section for non-stdout
  /*if( -1 == (f_out = open(f_out_name, O_WRONLY, 0577))) {
    perror("open");
    exit(1);
  }*/

  /* set up buffer */
  if(! (wr_buf = malloc(sizeof(char) * WRITE_BUF_LEN))) {
    perror("malloc");
    exit(1);
  }

  /* retrieve packets*/
  while((res = pcap_next_ex(trace_ptr, &pkt_hdr, &pkt_data)) >= 0) {
    pkt_count++;
    write(f_out, "\n", 1);
    /* write general packet info*/
    snprintf(wr_buf, WRITE_BUF_LEN, "Packet number: %i  Packet Len: %i\n\n",
    pkt_count, pkt_hdr->len);
    write(f_out, wr_buf, strlen(wr_buf));
    /* get and write Enet header info*/
    pkt_data_old = pkt_data;
    pkt_data = wrEnetInfo(f_out, wr_buf, pkt_data, enet_h);
    if(!enet_h->type_flag)
      wrArpInfo(f_out, wr_buf, pkt_data_old, arp_h);
    else if(enet_h->type_flag == 1) {
      pkt_data = wrIpInfo(f_out, wr_buf, pkt_data, ip_h);
      if(!ip_h->prot_flag) //ICMP
        wrIcmpInfo(f_out, wr_buf, pkt_data);
      else if(ip_h->prot_flag == 1) //TCP
        wrTcpInfo(f_out, wr_buf, pkt_data, ip_h, tcp_h);
      else if(ip_h->prot_flag == 2) //UDP
        wrUdpInfo(f_out, wr_buf, pkt_data, udp_h);
    }
    else {
      snprintf(wr_buf, WRITE_BUF_LEN, "\tUnknown PDU");
      write(f_out, wr_buf, strlen(wr_buf));
      memset(wr_buf,0,WRITE_BUF_LEN);
    }
    write(f_out, "\n", 1);
  }

  /* close files and cleanup*/
  pcap_close(trace_ptr);
  close(f_out);
  free(wr_buf);
  free(f_out_name);
  free(enet_h);
  free(arp_h);
  free(ip_h);
  free(tcp_h);
  free(udp_h);

  return 0;
}
