#ifndef COMM_IPCONVERTTOOLS_H_
#define COMM_IPCONVERTTOOLS_H_

//#include <dtls/dtls.h>
#include <framework/tcpip/tcpip.h>			// IP_MULTI_ADDRESS
#include <framework/tcpip/berkeley_api.h>	// SOCKADDR

//=================================================================================================

// estructura intermedia que nos permitirá convertir de "session" a "IP_MULTI_ADDRESS / puerto" y viceversa
typedef union {
	struct {
		IP_MULTI_ADDRESS ipAddress;
		WORD port;
	} ip;
	
    //session_t session;
} __attribute__ ((packed)) IP_TO_SESSION_MAPPER;

//=================================================================================================

/**
 * Pasa de sesión a IP/puerto
 * 
 * @param session			La sesión
 * @param mapper			El par IP/puerto
 */
//void IpConvertTools_mapSessionToIpPort(session_t *session, IP_TO_SESSION_MAPPER *mapper);

/**
 * Pasa de IP/puerto a sesión
 * 
 * @param ipType			El tipo de IP
 * @param ip				La IP
 * @param puerto			El puerto
 * @param mapper			La sesión
 */
void IpConvertTools_mapIpPortToSession(IP_ADDRESS_TYPE ipType, IP_MULTI_ADDRESS *ip, WORD puerto, IP_TO_SESSION_MAPPER *mapper);

/**
 * Pasa de SOCKADDR (IPv4/IPv6) a IP/puerto
 * 
 * @param ipType			El tipo de IP
 * @param sockAddr			La SOCKADDR
 * @param mapper			El par IP/puerto
 */
void IpConvertTools_mapSockAddrToIpPort(IP_ADDRESS_TYPE ipType, SOCKADDR *sockAddr, IP_TO_SESSION_MAPPER *mapper);

/**
 * Pasa de IP/puerto a SOCKADDR (IPv4/IPv6)
 * 
 * @param ipType			El tipo de IP
 * @param ip				La IP
 * @param puerto			El puerto
 * @param sockAddr			La SOCKADDR
 */
void IpConvertTools_mapIpPortToSockAddr(IP_ADDRESS_TYPE ipType, IP_MULTI_ADDRESS *ip, WORD puerto, SOCKADDR *sockAddr);

/**
 * Pasa de SOCKADDR (IPv4/IPv6) a sesión
 * 
 * @param ipType			El tipo de IP
 * @param sockAddr			La SOCKADDR
 * @param mapper			La sesión
 */
void IpConvertTools_mapSockAddrToSession(IP_ADDRESS_TYPE ipType, SOCKADDR *sockAddr, IP_TO_SESSION_MAPPER *mapper);

/**
 * Pasa de sesión a SOCKADDR (IPv4/IPv6)
 * 
 * @param ipType			El tipo de IP
 * @param session			La sesión
 * @param sockAddr			La SOCKADDR
 */
//void IpConvertTools_mapSessionToSockAddr(IP_ADDRESS_TYPE ipType, session_t *session, SOCKADDR *sockAddr);

//=================================================================================================

#endif
