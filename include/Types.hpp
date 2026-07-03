#ifndef TYPES_HPP
#define TYPES_HPP

#include <map>
#include <vector>

struct ServerConfig;

typedef std::vector<const ServerConfig*> ServersConfigVector;
typedef std::map<int, ServersConfigVector> ServersConfigMap;

#endif