/* header to get prtraceroute to compile under linux-2.0.x.  If anyone has
	a more elegant way to get the __u32 / in_addr conflict with the 
	source/dest in ip packets solvded, I'd love to hear it.
			-- zab@grumblesmurf.net */

#ifndef __IP_LNX__
#define __IP_LNX__

extern "C" {
#include <asm/types.h>
#define __LITTLE_ENDIAN_BITFIELD
#include <linux/icmp.h>

#define ICMP_MINLEN 		8

#define ICMP_TIMXCEED 		ICMP_TIME_EXCEEDED
#define ICMP_UNREACH 		ICMP_DEST_UNREACH
#define ICMP_SOURCEQUENCH 	ICMP_SOURCE_QUENCH
#define ICMP_PARAMPROB 		ICMP_PARAMETERPROB
#define ICMP_TSTAMP 		ICMP_TIMESTAMP
#define ICMP_TSTAMPREPLY 	ICMP_TIMESTAMPREPLY
#define ICMP_IREQ		ICMP_INFO_REQUEST
#define ICMP_IREQREPLY		ICMP_INFO_REPLY
#define ICMP_MASKREQ		ICMP_ADDRESS
#define	ICMP_MASKREPLY		ICMP_ADDRESSREPLY

#define ICMP_UNREACH_PORT 	ICMP_PORT_UNREACH
#define ICMP_UNREACH_NET 	ICMP_NET_UNREACH
#define ICMP_UNREACH_HOST 	ICMP_HOST_UNREACH
#define ICMP_UNREACH_PROTOCOL 	ICMP_PROT_UNREACH
#define ICMP_UNREACH_NEEDFRAG 	ICMP_FRAG_NEEDED
#define ICMP_UNREACH_SRCFAIL 	ICMP_SR_FAILED

#define ICMP_TIMXCEED_INTRANS 	ICMP_EXC_TTL


#define icmp_type 	ic.type
#define icmp_code 	ic.code
#define icmp_cksum	ic.checksum

#define ip 		ouriphdr

#define ip_len	tot_len
#define ip_src	sun.s_addr
#define ip_dst	dun.d_addr
#define ip_ttl	ttl
#define ip_v	version
#define ip_hl	ihl
#define ip_tos	tos
#define ip_id	id
#define ip_off	frag_off
#define ip_p	protocol
#define ip_sum	check

struct ouriphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
        __u8    ihl:4,
                version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
        __u8    version:4,
                ihl:4;
#else
#error  "Please fix <asm/byteorder.h>"
#endif
        __u8    tos;
        __u16   tot_len;
        __u16   id;
        __u16   frag_off;
        __u8    ttl;
        __u8    protocol;
        __u16   check;
        union {
		__u32   saddr;
		in_addr s_addr;
	} sun;
	union {
	        __u32   daddr;
		in_addr d_addr;
	} dun;
		

        /*The options start here. */
};

/* linux doesn't include the ip header in the iphdr struct.. */
struct icmp {
	struct icmphdr ic;
	struct ouriphdr icmp_ip;
};

#define	uh_sport	source	/* source port */
#define uh_dport 	dest	/* destination port */
#define uh_ulen 	len	/* udp length */
#define uh_sum 		check	/* udp checksum */

}

#endif /* #ifndef __IP_LNX__ */
