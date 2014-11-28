/**
 * xml_serializer.cpp
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#include "base2/xml/xml_serializer.h"
#include <iostream>

namespace base {

XMLSerializer::XMLSerializer(std::ostream& out, size_t indentSpace)
    : d_error(false), d_depth(0), d_indentSpace(indentSpace), 
      d_needClose(false), d_lastIsText(false), d_stream(out) {
    d_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    d_error = ! d_stream;
}


XMLSerializer::~XMLSerializer(void) {
    if (!d_error || !d_tagStack.empty()) {
        d_stream << std::endl;
    }
}

XMLSerializer& XMLSerializer::openTag(const std::string& name) {
    if (! d_error) {
        ++d_tagCount;
        if (d_needClose) {
            d_stream << '>';	  
        }
        if (!d_lastIsText) {
            d_stream << std::endl;
            indentLine();
        }
        d_stream << '<' << name.c_str() << ' ';
        d_tagStack.push_back(name);
        ++d_depth;
        d_needClose = true;
        d_lastIsText = false;
        d_error = ! d_stream;
    }
    return *this;
}

XMLSerializer& XMLSerializer::closeTag(void) {
    std::string back =  d_tagStack.back();
    if (! d_error) {
        --d_depth;
        if (d_needClose) {
            d_stream << "/>";
        }
        else if (! d_lastIsText) {
            d_stream << std::endl;
            indentLine();
            d_stream << "</" << back.c_str() << '>';
        } else {
            d_stream << "</" << back.c_str() << '>';
        }
        d_lastIsText = false;
        d_needClose = false;
        d_tagStack.pop_back();
        d_error = ! d_stream;
    }
    return *this;
}


XMLSerializer& XMLSerializer::attribute(const std::string& name, const std::string& value) {
    if (!d_needClose) {
        d_error = true;
    }
    if (!d_error) {
        d_stream << name.c_str() << "=\""
            << convertEntityInAttribute(value).c_str()
            << "\" ";
        d_lastIsText = false;
        d_error = ! d_stream;
    }
    return *this;
}

XMLSerializer& XMLSerializer::attributeInt(const std::string& name, int value) {
  if (!d_needClose) {
    d_error = true;
  }
  if (!d_error) {
    d_stream << name.c_str() << "=\""
    << value
    << "\" ";
    d_lastIsText = false;
    d_error = ! d_stream;
  }
  return *this;
}

XMLSerializer& XMLSerializer::attributeInt64(const std::string& name, int64 value) {
  if (!d_needClose) {
    d_error = true;
  }
  if (!d_error) {
    d_stream << name.c_str() << "=\""
    << value
    << "\" ";
    d_lastIsText = false;
    d_error = ! d_stream;
  }
  return *this;
}

XMLSerializer& XMLSerializer::attributeFloat(const std::string& name, float value) {
  if (!d_needClose) {
    d_error = true;
  }
  if (!d_error) {
    d_stream << name.c_str() << "=\""
    << value
    << "\" ";
    d_lastIsText = false;
    d_error = ! d_stream;
  }
  return *this;
}

XMLSerializer& XMLSerializer::attributeBoolAsTrueFalse(const std::string& name, bool value) {
  if (!d_needClose) {
    d_error = true;
  }
  if (!d_error) {
    d_stream << name.c_str() << "=\""
    << (value ? "true" : "false")
    << "\" ";
    d_lastIsText = false;
    d_error = ! d_stream;
  }
  return *this;
}

XMLSerializer& XMLSerializer::attributeBoolAsOneZero(const std::string& name, bool value) {
  if (!d_needClose) {
    d_error = true;
  }
  if (!d_error) {
    d_stream << name.c_str() << "=\""
    << (value ? "1" : "0")
    << "\" ";
    d_lastIsText = false;
    d_error = ! d_stream;
  }
  return *this;
}

XMLSerializer& XMLSerializer::text(const std::string& text) {
    if (! d_error) {
        if (d_needClose) {
            d_stream << '>';
            d_needClose = false;
        }
        d_stream << convertEntityInText(text).c_str();
        d_lastIsText = true;
        d_error = ! d_stream;
    }
    return *this;
}

unsigned int XMLSerializer::getTagCount() const {
    return d_tagCount;
}

void XMLSerializer::indentLine(void) {
    size_t spaceCount = d_depth * d_indentSpace;
    // There's for sure a best way to do this but it works 
    for (size_t i = 0 ; i < spaceCount ; ++i) {
        d_stream << ' ';
    }  
}

std::string XMLSerializer::convertEntityInText(const std::string& text) {
    std::string res;
    res.reserve(text.size()*2);
    const std::string::const_iterator iterEnd = text.end();
    for (std::string::const_iterator iter = text.begin(); iter != iterEnd ; ++iter) {
        switch(*iter) {  
            case '<':
                res += "&lt;";
                break;
      
            case '>':
                res += "&gt;";
                break;
        
            case '&':
                res += "&amp;";
                break;
        
            case '\'':
                res += "&apos;";
                break;

            case '"':
                res += "&quot;";
                break;
      
            default:
                res += *iter;
        }
    }
    return res;
}

std::string XMLSerializer::convertEntityInAttribute(const std::string& attributeValue) {
    // Reserve a lot of space 
    std::string res;
    res.reserve(attributeValue.size()*2);
    const std::string::const_iterator iterEnd = attributeValue.end();
    for (std::string::const_iterator iter = attributeValue.begin(); iter != iterEnd ; ++iter) {
        switch(*iter) {  
            case '<':
                res += "&lt;";
                break;
      
            case '>':
                res += "&gt;";
                break;
        
            case '&':
                res += "&amp;";
                break;
        
            case '\'':
                res += "&apos;";
                break;

            case '"':
                res += "&quot;";
                break;
      
            case '\n':
                res += "\\n";
                break;
            default:
                res += *iter;
        }
    }
    return res;
}

} // End of  base namespace section
