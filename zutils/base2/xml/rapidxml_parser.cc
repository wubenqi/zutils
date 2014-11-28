/**
 * rapidxml_parser.cc
 * DESCRIPTION:
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-28
 */

#include "base2/xml/rapidxml_parser.h"

#include "base/logging.h"
#include "base/file_util.h"

#include "base2/xml/xml_handler.h"
#include "base2/xml/xml_attributes.h"

#include "base2/xml/third_party/rapidxml/rapidxml.hpp"
//#include "base/xml/third_party/rapidxml/rapidxml_utils.hpp"
//#include "base/xml/third_party/rapidxml/rapidxml_iterators.hpp"

namespace base {

//class TinyXMLDocument :
//  public TiXmlDocument {
//public:
//	explicit TinyXMLDocument(XMLHandler& handler);
//	~TinyXMLDocument(){}
//  
//	bool ParseXMLData(char* buf, bool b_free = false);
//  
//protected:
//	bool ProcessElement(const TiXmlElement* element);
//  
//private:
//	XMLHandler* d_handler;
//};
//
//TinyXMLDocument::TinyXMLDocument(XMLHandler& handler) {
//	d_handler = &handler;
//}
//
//bool TinyXMLDocument::ParseXMLData(char* buf, bool b_free) {
//	char* d_buf = buf;
//  
//	// Parse the document
//	TiXmlDocument doc;
//	if (!doc.Parse((const char*)d_buf)) {
//		// throw exception
//		if(b_free) delete [] d_buf;
//		
//		LOG(ERROR) << "TinyXMLParser: an error occurred while "
//    "parsing the XML document - check it for potential errors!.";
//		return false;
//	}
//  
//	const TiXmlElement* currElement = doc.RootElement();
//	if (currElement) {
//		if(!ProcessElement(currElement)) {
//			LOG(ERROR) << "TinyXMLParser: an error occurred while process xml element";
//			if(b_free) delete [] d_buf;
//			return false;
//		}
//	} // if (currElement)
//  
//	// Free memory
//	if(b_free) delete [] d_buf;
//	return true;
//}
//
//bool TinyXMLDocument::ProcessElement(const TiXmlElement* element) {
//	// build attributes block for the element
//	XMLAttributes attrs;
//  
//	const TiXmlAttribute *currAttr = element->FirstAttribute();
//	while (currAttr) {
//		attrs.Add(currAttr->Name(), currAttr->Value());
//		currAttr = currAttr->Next();
//	}
//  
//	// start element
//	if(!d_handler->ElementStart(element->Value(), attrs)) {
//		return false;
//	}
//  
//	// do children
//	const TiXmlNode* childNode = element->FirstChild();
//	while (childNode) {
//		switch(childNode->Type())
//		{
//      case ELEMENT:
//			ProcessElement(childNode->ToElement());
//			break;
//      case TEXT:
//			if (childNode->ToText()->Value() != NULL)
//				d_handler->Text(childNode->ToText()->Value());
//			break;
//      
//			// Silently ignore unhandled node type
//		};
//		childNode = childNode->NextSibling();
//	}
//  
//	// end element
//	if(!d_handler->ElementEnd(element->Value())) {
//		return false;
//	}
//  
//	return true;
//}

namespace {

bool ProcessElement(XMLHandler& handler, const rapidxml::xml_node<>* node) {
	XMLAttributes attrs;

  rapidxml::xml_attribute<> *curr_attr = node->first_attribute();
	while (curr_attr) {
		attrs.Add(curr_attr->name(), curr_attr->value());
		curr_attr = curr_attr->next_attribute();
	}

	// start element
	if(!handler.ElementStart(node->name(), attrs)) {
		return false;
	}

	// do children
	const rapidxml::xml_node<>* child_node = node->first_node();
	while (child_node) {
		switch(child_node->type())
		{
      case rapidxml::node_element:
        ProcessElement(handler, child_node);
			break;
      case rapidxml::node_data:
        if (child_node->value() != NULL)
          handler.Text(child_node->value());
        break;
      default:
        // Silently ignore unhandled node type
        break;
		};
		child_node = child_node->next_sibling();
	}

	// end element
	if(!handler.ElementEnd(node->name())) {
		return false;
	}
  
	return true;
}
  
}

  
RapidxmlParser::RapidxmlParser(void) {
  // set ID string
	d_identifierString = "base::RapidxmlParser - Official Rapidxml based parser module";
}

RapidxmlParser::~RapidxmlParser(void) {
}

bool RapidxmlParser::ParseXMLData(XMLHandler& handler, const char* xml_text, size_t xml_text_size) {
  rapidxml::xml_document<>  doc;
  doc.parse<0>(const_cast<char*>(xml_text));
  rapidxml::xml_node<>* root = doc.first_node();
  if (root == 0) {
    LOG(ERROR) << "RapidxmlParser::ParseXMLData - XML Parsing error";
  }
  return ProcessElement(handler, root);
}

bool RapidxmlParser::ParseXMLData(XMLHandler& handler, const std::string &xml_text) {
	// TinyXMLDocument doc(handler);
	// return doc.ParseXMLData(const_cast<char*>(xml_text.c_str()));
  return ParseXMLData(handler, xml_text.c_str(), xml_text.length());
}

bool RapidxmlParser::ParseXMLFile(XMLHandler& handler, const std::string& filename) {
	std::string data;
	if(!ReadFileToString(filename.c_str(), &data)) {
		LOG(ERROR) << "RapidxmlParser::ParseXMLFile - Unable to open XMLFile";
		return false;
	} else {
    data.push_back('\n');
    data.push_back('\0');
  }
  
  return ParseXMLData(handler, data);
}

bool RapidxmlParser::InitialiseImpl(void) {
  return true;
}

void RapidxmlParser::CleanupImpl(void) {
  
}

}
