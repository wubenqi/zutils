// Copyright 2010, www.lelewan.com
// All rights reserved
//
// Author: wubenqi<wubenqi@caitong.net>, 2009-02-04
//

#include "base2/config_file_util.h"

#include <iostream>

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"

#include "base2/config_file.h"

namespace base {
  
bool DBAddrInfo::ParseFromConnString(const base::StringPiece& conn_string) {
  bool result = true;
  const char* p = conn_string.data();
  const char* b = p;
  const char* s = p;
  for (; *p; ++p) {
    if (*p == ':') {
      s = p;
    }
    if (*p=='@') {
      if (s==b) {
        user.assign(b, p-b);
      } else {
        user.assign(b, s-b);
        passwd.assign(s+1, p-(s+1));
      }
      b = p+1;
      s= b;
    }
    if (*p=='/') {
      if (*b=='\0') {
        break;
      } else {
        if (host.empty()) {
          if (s==b) {
            host.assign(b, p-b);
          } else {
            host.assign(b, s-b);
            base::StringPiece tmp_port(s+1, p-(s+1));
            result = base::StringToInt(tmp_port, &port);
            if (!result) {
              LOG(ERROR) << "port is invalid number: " << tmp_port;
              break;
            }
          }
        } else {
          db_name.assign(b, p-b);
        }
      }
      b = p+1;
      s= b;
    }
  }
  
  return result;
}

void DBAddrInfo::PrintDebugString() {
  std::cout << user << ":" << passwd << "@" << host << ":" << port << "/" << db_name << "/" << std::endl;
}

bool DBAddrInfo::ParseFromConfigFile(const base::ConfigFile& config_file, const char* section_name) {
  if (!config_file.CheckBySection(section_name)) {
    LOG(ERROR) << "Not find section " << section_name;
    return false;
  }
  if (config_file.CheckBySectionAndKey(section_name, "host")) {
    host = config_file.GetString(section_name, "host");
  }
  if (config_file.CheckBySectionAndKey(section_name, "port")) {
    port = config_file.GetInt(section_name, "port");
  }
  if (config_file.CheckBySectionAndKey(section_name, "user")) {
    user = config_file.GetString(section_name, "user");
  }
  if (config_file.CheckBySectionAndKey(section_name, "passwd")) {
    passwd = config_file.GetString(section_name, "passwd");
  }
  if (config_file.CheckBySectionAndKey(section_name, "dbname")) {
    db_name = config_file.GetString(section_name, "dbname");
  }
  if (config_file.CheckBySectionAndKey(section_name, "dbtype")) {
    db_type = config_file.GetString(section_name, "dbtype");
  }
  if (config_file.CheckBySectionAndKey(section_name, "min_conn_count")) {
    min_conn_count = config_file.GetInt(section_name, "min_conn_count");
  }
  if (config_file.CheckBySectionAndKey(section_name, "max_conn_count")) {
    max_conn_count = config_file.GetInt(section_name, "max_conn_count");
  }
  return true;
}


bool IpAddrInfo::ParseFromConfigFile(const base::ConfigFile& config_file, const char* section_name) {
  if (!config_file.CheckBySection(section_name)) {
    LOG(ERROR) << "Not find section " << section_name;
    return false;
  }

  if (config_file.CheckBySectionAndKey(section_name, "host")) {
    addr = config_file.GetString(section_name, "host");
  }
  if (config_file.CheckBySectionAndKey(section_name, "port")) {
    port = config_file.GetInt(section_name, "port");
  }

  return true;
}

bool IpAddrInfo2::ParseFromConfigFile(const base::ConfigFile& config_file, const char* section_name) {
  if (!config_file.CheckBySection(section_name)) {
    LOG(ERROR) << "Not find section " << section_name;
    return false;
  }

  if (config_file.CheckBySectionAndKey(section_name, "host")) {
    addr = config_file.GetString(section_name, "host");
  } else {
    return false;
  }
  if (config_file.CheckBySectionAndKey(section_name, "port")) {
    port = config_file.GetString(section_name, "port");
  } else {
    return false;
  }

  return true;
}

}

