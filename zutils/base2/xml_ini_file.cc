// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/xml_ini_file.h"

XMLIniFile::XMLIniFile(void) : 
	m_bInited(false),
	updated_(false) {
	m_szXMLIniFile[0] = '\0';
}

XMLIniFile::~XMLIniFile(void) {
	Flush();
}

bool XMLIniFile::LoadXMLIniFile(const char* pstrFilename) {
    if(!pstrFilename)
		return false;
	strcpy( m_szXMLIniFile, pstrFilename );
	return m_bInited = m_xmlIniParser.Load(pstrFilename);
}

bool XMLIniFile::GetString( const char* pstrSection, const char* pstrKey, char* pstrValue,  unsigned int cchMax, const char* pstrDefault) {
	if (!m_bInited || !pstrSection || !pstrKey )
		return false;
	std::string strValue;
	if( FindEntry( pstrSection, pstrKey, strValue ) ) {
		if (strValue.length()+1 > cchMax) {
			strncpy( pstrValue, strValue.c_str(), cchMax-1 );
			pstrValue[cchMax-1] = 0;
		} else
			strcpy( pstrValue, strValue.c_str() );
	} else {
		if ( !pstrDefault )
			pstrValue[0] = '\0';
		else
			strcpy( pstrValue, pstrDefault );
	}
	return true;
}

bool XMLIniFile::GetString(const char* pstrSection, const char* pstrKey, std::string& sValue, const char* pstrDefault ) {      
	if (!m_bInited || !pstrSection || !pstrKey )
		return false;
	if (!FindEntry( pstrSection, pstrKey, sValue )) {
		if ( !pstrDefault )
			sValue = "";
		else
			sValue = pstrDefault;
	}
	return true;
}

bool XMLIniFile::GetStrings( const char* pstrSection, const char* pstrKey, std::vector<std::string>& sValue ) {
	if (!m_bInited || !pstrSection || !pstrKey )
		return false;
	FindEntrys( pstrSection, pstrKey, sValue );
	return true;
}

bool XMLIniFile::GetInt(const char* pstrSection, const char* pstrKey, int& iValue,  int iDefault) {
	if (!m_bInited || !pstrSection || !pstrKey )
		return false;
	std::string strValue;
	if( FindEntry( pstrSection, pstrKey, strValue ) )
		iValue = atoi(strValue.c_str());
	else
		iValue = iDefault;
	return true;
}

bool XMLIniFile::GetInts( const char* pstrSection, const char* pstrKey, std::vector<int>& iValue ) {
	//todo
	return false;
}


bool XMLIniFile::GetBool(const char* pstrSection, const char* pstrKey, bool& bValue, bool bDefault ) {
	if (!m_bInited || !pstrSection  || !pstrKey)
		return false;
	std::string strValue;
	if( FindEntry( pstrSection, pstrKey, strValue ) ) {
		//char szValue[2] = { 0 };
		switch( strValue[0] ) {
		case 'y': // Yes
		case 'Y':
		case 't': // True
		case 'T':
		case '1': // 1
			bValue = true;
			break;
		case 'n': // No
		case 'N':
		case 'f': // False
		case 'F':
		case '0': // 0
			bValue = false;
			break;
		default:
			bValue = bDefault;
		}
	}
	return true;
}

bool XMLIniFile::GetBools( const char* pstrSection, const char* pstrKey, std::vector<bool>& bValue ) {
	//todo
	return false;
}


bool XMLIniFile::PutString(const char* pstrSection, const char* pstrKey, const char* pstrValue) {
	if ( !m_bInited || !pstrSection || !pstrKey )
		return false;
	SetEntry( pstrSection, pstrKey, pstrValue );
	updated_ = true;
    return true;
}

bool XMLIniFile::PutInt(const char* pstrSection, const char* pstrKey, int iValue) {
	if ( !m_bInited || !pstrSection || !pstrKey)
		return false;
	char szVal[25];
	sprintf( szVal, "%d", iValue );
	SetEntry( pstrSection, pstrKey, szVal );
	updated_ = true;
    return true;
}

bool XMLIniFile::PutBool(const char* pstrSection, const char* pstrKey, bool bValue) {
	if (!m_bInited || !pstrSection || !pstrKey)
		return false;
	std::string strVal = "false";
	if(bValue)
		strVal = "true";
  	SetEntry( pstrSection, pstrKey, strVal );
	updated_ = true;
	return true;
}

void XMLIniFile::DeleteKey(const char* pstrSection, const char* pstrKey) {
	// Loop through sections
	m_xmlIniParser.ResetPos();
	while ( m_xmlIniParser.FindChildElem("Section") ) {
#ifdef WIN32
		// Is this the right section?
		if ( !stricmp(m_xmlIniParser.GetChildAttrib("name").c_str(), pstrSection ) )
#else
		if ( !strcasecmp(m_xmlIniParser.GetChildAttrib("name").c_str(), pstrSection ) )
#endif
		{
			// Check entries in this section
			m_xmlIniParser.IntoElem();
			while ( m_xmlIniParser.FindChildElem("Item") ) {
				// Is this the right entry?
#ifdef WIN32
				if ( !stricmp (m_xmlIniParser.GetChildAttrib("name").c_str(), pstrKey) )
#else
				if ( !strcasecmp (m_xmlIniParser.GetChildAttrib("name").c_str(), pstrKey) )
#endif
				{
					m_xmlIniParser.RemoveChildElem();
					return;
				}
			}
			break; // don't check any other sections
		}
	}
	updated_ = true;
}

void XMLIniFile::DeleteSection(const char* pstrSection)
{
	if ( !m_bInited || !pstrSection )
		return;

	// Loop through sections
	m_xmlIniParser.ResetPos();
	while ( m_xmlIniParser.FindChildElem("Section") ) {
		// Is this the right section?
#ifdef WIN32
		if ( !stricmp( m_xmlIniParser.GetChildAttrib("name").c_str(), pstrSection) )
#else
		if ( !strcasecmp( m_xmlIniParser.GetChildAttrib("name").c_str(), pstrSection) )
#endif
		{
			m_xmlIniParser.RemoveChildElem();
			break;
		}
	}
	updated_ = true;
}

void XMLIniFile::Flush() {
	if (m_bInited && updated_)
        m_xmlIniParser.Save( m_szXMLIniFile );	
}

void XMLIniFile::SetEntry( const std::string& strSection, const std::string& strItem, const std::string& strValue ) {
	// Find/Create root element of xml document
	m_xmlIniParser.ResetPos();
	if ( ! m_xmlIniParser.FindElem() )
		m_xmlIniParser.AddElem( "XMLIni" ); // or whatever root element name is

	// Find/Create section
	bool bFoundSection = false;
	while ( m_xmlIniParser.FindChildElem("Section") ) {
		// Is this the right section?
		if ( m_xmlIniParser.GetChildAttrib("name") == strSection ) {
			bFoundSection = true;
			break;
		}
	}
	if ( ! bFoundSection ) {
		m_xmlIniParser.AddChildElem( "Section" );
		m_xmlIniParser.SetChildAttrib( "name", strSection.c_str() );
	}

	// Find/Create entry
	m_xmlIniParser.IntoElem();
	bool bFoundEntry = false;
	while ( m_xmlIniParser.FindChildElem("Item") ) {
		// Is this the right entry?
		if ( m_xmlIniParser.GetChildAttrib("name") == strItem ) {
			bFoundEntry = true;
			break;
		}
	}
	if ( ! bFoundEntry ) {
		m_xmlIniParser.AddChildElem( "Item" );
		m_xmlIniParser.SetChildAttrib( "name", strItem.c_str() );
	}

	// Set value
	m_xmlIniParser.SetChildData( strValue.c_str() );
}

bool XMLIniFile::FindEntry( const std::string& strSection, const std::string& strItem, std::string& strValue ) {
	// Loop through sections
	m_xmlIniParser.ResetPos();
	while ( m_xmlIniParser.FindChildElem("Section") ) {
		// Is this the right section?
		if (  m_xmlIniParser.GetChildAttrib("name") == strSection ) {
			// Check entries in this section
			m_xmlIniParser.IntoElem();
			while ( m_xmlIniParser.FindChildElem("Item") ) {
				// Is this the right entry?
				if ( m_xmlIniParser.GetChildAttrib("name") == strItem ) {
					strValue = m_xmlIniParser.GetChildData();
					return true;
				}
			}
			break; // don't check any other sections
		}
	}
	return false;
}

bool XMLIniFile::FindEntrys( const std::string& strSection, const std::string& strItem, std::vector<std::string>& strValue ) {
	// Loop through sections
	m_xmlIniParser.ResetPos();
	while ( m_xmlIniParser.FindChildElem("Section") ) {
		// Is this the right section?
		if (  m_xmlIniParser.GetChildAttrib("name") == strSection ) {
			// Check entries in this section
			m_xmlIniParser.IntoElem();
			while ( m_xmlIniParser.FindChildElem("Item") ) {
				// Is this the right entry?
				if ( m_xmlIniParser.GetChildAttrib("name") == strItem ) {
					strValue.push_back(m_xmlIniParser.GetChildData());
					//return true;
				}
			}
			break; // don't check any other sections
		}
	}
	return false;
}
