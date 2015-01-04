// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_NET_UTIL_H_
#define NET_BASE_NET_UTIL_H_

#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#include <ws2tcpip.h>
#elif defined(OS_POSIX)
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/strings/string16.h"
#include "base/strings/utf_offset_string_conversions.h"
#include "net/base/address_family.h"
#include "net/base/escape.h"
#include "net/base/net_export.h"

namespace base {
class Time;
}

namespace net {

// IPAddressNumber is used to represent an IP address's numeric value as an
// array of bytes, from most significant to least significant. This is the
// network byte ordering.
//
// IPv4 addresses will have length 4, whereas IPv6 address will have length 16.
typedef std::vector<unsigned char> IPAddressNumber;
typedef std::vector<IPAddressNumber> IPAddressList;

static const size_t kIPv4AddressSize = 4;
static const size_t kIPv6AddressSize = 16;
#if defined(OS_WIN)
// Bluetooth address size. Windows Bluetooth is supported via winsock.
static const size_t kBluetoothAddressSize = 6;
#endif


// Returns the number of explicitly allowed ports; for testing.
NET_EXPORT_PRIVATE extern size_t GetCountOfExplicitlyAllowedPorts();


// Convenience struct for when you need a |struct sockaddr|.
struct SockaddrStorage {
  SockaddrStorage() : addr_len(sizeof(addr_storage)),
                      addr(reinterpret_cast<struct sockaddr*>(&addr_storage)) {}
  SockaddrStorage(const SockaddrStorage& other);
  void operator=(const SockaddrStorage& other);

  struct sockaddr_storage addr_storage;
  socklen_t addr_len;
  struct sockaddr* const addr;
};

// Extracts the IP address and port portions of a sockaddr. |port| is optional,
// and will not be filled in if NULL.
bool GetIPAddressFromSockAddr(const struct sockaddr* sock_addr,
                              socklen_t sock_addr_len,
                              const unsigned char** address,
                              size_t* address_len,
                              uint16* port);

// Returns the string representation of an IP address.
// For example: "192.168.0.1" or "::1".
NET_EXPORT std::string IPAddressToString(const uint8* address,
                                         size_t address_len);

// Returns the string representation of an IP address along with its port.
// For example: "192.168.0.1:99" or "[::1]:80".
NET_EXPORT std::string IPAddressToStringWithPort(const uint8* address,
                                                 size_t address_len,
                                                 uint16 port);

// Same as IPAddressToString() but for a sockaddr. This output will not include
// the IPv6 scope ID.
NET_EXPORT std::string NetAddressToString(const struct sockaddr* sa,
                                          socklen_t sock_addr_len);

// Same as IPAddressToStringWithPort() but for a sockaddr. This output will not
// include the IPv6 scope ID.
NET_EXPORT std::string NetAddressToStringWithPort(const struct sockaddr* sa,
                                                  socklen_t sock_addr_len);

// Same as IPAddressToString() but for an IPAddressNumber.
NET_EXPORT std::string IPAddressToString(const IPAddressNumber& addr);

// Same as IPAddressToStringWithPort() but for an IPAddressNumber.
NET_EXPORT std::string IPAddressToStringWithPort(
    const IPAddressNumber& addr, uint16 port);

// Returns the address as a sequence of bytes in network-byte-order.
NET_EXPORT std::string IPAddressToPackedString(const IPAddressNumber& addr);

// Returns the hostname of the current system. Returns empty string on failure.
NET_EXPORT std::string GetHostName();

// Return the value of the HTTP response header with name 'name'.  'headers'
// should be in the format that URLRequest::GetResponseHeaders() returns.
// Returns the empty string if the header is not found.
NET_EXPORT std::string GetSpecificHeader(const std::string& headers,
                                         const std::string& name);


// Set socket to non-blocking mode
NET_EXPORT int SetNonBlocking(int fd);

NET_EXPORT void SetExplicitlyAllowedPorts(const std::string& allowed_ports);

class NET_EXPORT ScopedPortException {
 public:
  explicit ScopedPortException(int port);
  ~ScopedPortException();

 private:
  int port_;

  DISALLOW_COPY_AND_ASSIGN(ScopedPortException);
};

// Returns true if it can determine that only loopback addresses are configured.
// i.e. if only 127.0.0.1 and ::1 are routable.
// Also returns false if it cannot determine this.
bool HaveOnlyLoopbackAddresses();

// Returns AddressFamily of the address.
NET_EXPORT_PRIVATE AddressFamily GetAddressFamily(
    const IPAddressNumber& address);

// Maps the given AddressFamily to either AF_INET, AF_INET6 or AF_UNSPEC.
NET_EXPORT_PRIVATE int ConvertAddressFamily(AddressFamily address_family);

// Parses a URL-safe IP literal (see RFC 3986, Sec 3.2.2) to its numeric value.
// Returns true on success, and fills |ip_number| with the numeric value
NET_EXPORT bool ParseURLHostnameToNumber(const std::string& hostname,
                                         IPAddressNumber* ip_number);

// Parses an IP address literal (either IPv4 or IPv6) to its numeric value.
// Returns true on success and fills |ip_number| with the numeric value.
NET_EXPORT bool ParseIPLiteralToNumber(const std::string& ip_literal,
                                       IPAddressNumber* ip_number);

// Converts an IPv4 address to an IPv4-mapped IPv6 address.
// For example 192.168.0.1 would be converted to ::ffff:192.168.0.1.
NET_EXPORT_PRIVATE IPAddressNumber ConvertIPv4NumberToIPv6Number(
    const IPAddressNumber& ipv4_number);

// Returns true iff |address| is an IPv4-mapped IPv6 address.
NET_EXPORT_PRIVATE bool IsIPv4Mapped(const IPAddressNumber& address);

// Converts an IPv4-mapped IPv6 address to IPv4 address. Should only be called
// on IPv4-mapped IPv6 addresses.
NET_EXPORT_PRIVATE IPAddressNumber ConvertIPv4MappedToIPv4(
    const IPAddressNumber& address);

// Parses an IP block specifier from CIDR notation to an
// (IP address, prefix length) pair. Returns true on success and fills
// |*ip_number| with the numeric value of the IP address and sets
// |*prefix_length_in_bits| with the length of the prefix.
//
// CIDR notation literals can use either IPv4 or IPv6 literals. Some examples:
//
//    10.10.3.1/20
//    a:b:c::/46
//    ::1/128
NET_EXPORT bool ParseCIDRBlock(const std::string& cidr_literal,
                               IPAddressNumber* ip_number,
                               size_t* prefix_length_in_bits);

// Compares an IP address to see if it falls within the specified IP block.
// Returns true if it does, false otherwise.
//
// The IP block is given by (|ip_prefix|, |prefix_length_in_bits|) -- any
// IP address whose |prefix_length_in_bits| most significant bits match
// |ip_prefix| will be matched.
//
// In cases when an IPv4 address is being compared to an IPv6 address prefix
// and vice versa, the IPv4 addresses will be converted to IPv4-mapped
// (IPv6) addresses.
NET_EXPORT_PRIVATE bool IPNumberMatchesPrefix(const IPAddressNumber& ip_number,
                                              const IPAddressNumber& ip_prefix,
                                              size_t prefix_length_in_bits);

// Retuns the port field of the |sockaddr|.
const uint16* GetPortFieldFromSockaddr(const struct sockaddr* address,
                                       socklen_t address_len);
// Returns the value of port in |sockaddr| (in host byte ordering).
NET_EXPORT_PRIVATE int GetPortFromSockaddr(const struct sockaddr* address,
                                           socklen_t address_len);

}  // namespace net

#endif  // NET_BASE_NET_UTIL_H_
