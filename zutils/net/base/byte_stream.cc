// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/base/byte_stream.h"

//#include <stdlib.h>

namespace net {

ByteStream::ByteStream(void) 
	: m_isNew( true )
	, m_maxLen( 0 )
	, m_dataLen( 0 )
	, m_currentPos( 0 )
	, m_failed( 0 )
	, m_pBuffer( 0 ) {
}

ByteStream::ByteStream( const void* pBuffer, uint32 dataLen )
	: m_pBuffer( (char*)pBuffer )
	, m_isNew( false )
	, m_maxLen( dataLen )
	, m_dataLen( dataLen )
	, m_currentPos( 0 )
	, m_failed( 0 ) {
}

ByteStream::~ByteStream( ) {
	Free();
}

//////////////////////////////////////////////////////////
void* ByteStream::Alloc(uint32 dwNewLen) {
	if (m_failed) return 0;
	if(!m_isNew) {
		m_failed = 1;
		return 0;
	}
	if (dwNewLen > m_maxLen) {
		// determine new buffer size
		uint32 dwNewBufferSize = m_maxLen;
		while (dwNewBufferSize < dwNewLen) dwNewBufferSize += 4096;

		// allocate new buffer
		if (m_pBuffer == 0) m_pBuffer = (char *)malloc(dwNewBufferSize);
		else	m_pBuffer = (char *)realloc(m_pBuffer, dwNewBufferSize);
		if (0==m_pBuffer) {
			m_failed = 1;
			return 0;
		}
		//LOG_ASSERT(m_pBuffer!=NULL);
		m_maxLen = dwNewBufferSize;
	}
	return m_pBuffer;
}

void ByteStream::Free() {
	if( m_isNew ) {
		free(m_pBuffer);
		m_pBuffer = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
int32 ByteStream::Seek( uint32 pos ) const {
	if( pos > m_maxLen ) {
		return -1;
	}

	uint32 lOld = m_currentPos;
	m_currentPos = pos;
	m_failed = 0;

	return lOld;
}

uint32 ByteStream::Tell(  ) const {
	return m_currentPos;
}

int32 ByteStream::Skip( uint32 pos ) const {
	if( m_currentPos + pos > m_maxLen ) {
		return -1;
	}

	m_currentPos += pos;
	m_failed = 0;

	return 0;
}

bool ByteStream::Fail( ) const {
	return 0 != m_failed;
}

const char* ByteStream::Data( ) const {
	return m_pBuffer;
}

char* ByteStream::MutableData(void) {
  return m_pBuffer;
}

uint32 ByteStream::Length( ) const {
	return m_dataLen;
}

//////////////////////////////////////////////////////////////////////////
int32 ByteStream::ReadRawData( void* pdata, uint32 dataLen ) const {
	if (m_failed){
		return -1;
	}

	if(m_currentPos + dataLen > m_maxLen) {
		m_failed = 1;
		return -1;
	}

	if ( dataLen == 0 ) {
		return 0;
	}

	memcpy( pdata, m_pBuffer + m_currentPos, dataLen );
	m_currentPos += dataLen;

	return 0;
}

int32 ByteStream::ReadString( char* pStr, uint32 dataLen ) const {
	uint8 ch[4];
	uint32 l;
	if( 0 != ReadRawData(ch, sizeof(ch))) {
		return -1;
	}
	else {
		l = (( ch[0] & 0xFF) << 24) | ((ch[1] & 0xFF) << 16) | ((ch[2] & 0xFF) <<  8) | (ch[3] & 0xFF);
		if (l>dataLen) {
			l = dataLen;
			pStr[l-1]='\0';
		}
	}
	return ReadRawData(pStr, l);
}

int32 ByteStream::ReadString(std::string& str) const {
	uint8 ch[4];
	uint32 l;
	if( 0 != ReadRawData(ch, sizeof(ch))) {
		return -1;
	}
	else {
		l = (( ch[0] & 0xFF) << 24) | ((ch[1] & 0xFF) << 16) | ((ch[2] & 0xFF) <<  8) | (ch[3] & 0xFF);
	}

	if ( m_currentPos + l > m_maxLen ) {
		m_failed = 1;
		return -1;
	}
	str.clear();
	str.append(m_pBuffer + m_currentPos, 0, l);
	m_currentPos += l;
	return 0;
}

const ByteStream& ByteStream::operator >> (std::string& str) const {
	ReadString(str);
	return *this; 
}

const ByteStream& ByteStream::operator >> ( bool& b ) const { 
	if ( m_currentPos + sizeof( char ) > m_maxLen ) {
		m_failed = 1;
	}
	else {
		b = m_pBuffer[ m_currentPos++ ] ? true:false; 
	}
	return *this; 
}

const ByteStream& ByteStream::operator >> ( uint8& ch ) const { 
	if ( m_currentPos + sizeof( ch ) > m_maxLen ) {
		m_failed = 1;
	}
	else {
		ch = m_pBuffer[ m_currentPos++ ]; 
	}
	return *this; 
}

const ByteStream& ByteStream::operator >> ( char& ch ) const { 
	if ( m_currentPos + sizeof( ch ) > m_maxLen ) {
		m_failed = 1;
	}
	else {
		ch = m_pBuffer[ m_currentPos++ ]; 
	}
	return *this; 
}

const ByteStream& ByteStream::operator >> ( int16& s ) const { 
	uint8 ch[2];
	if( 0 == ReadRawData( ch, sizeof( ch ) ) ) {
		s = (int16)((ch[0] & 0xFF) << 8) | (ch[1] & 0xFF);
	}
	return *this; 
}

const ByteStream& ByteStream::operator >> ( uint16& s ) const { 
	uint8 ch[2];
	if( 0 == ReadRawData( ch, sizeof( ch ) ) ) {
		s = (uint16)((ch[0] & 0xFF) << 8) | (ch[1] & 0xFF);
	}
	return *this; 
}

const ByteStream& ByteStream::operator >> ( int32& l ) const { 
	uint8 ch[4];
	if( 0 == ReadRawData( ch, sizeof( ch ) ) ) {
		l = (int32)(( ch[0] & 0xFF) << 24) | ((ch[1] & 0xFF) << 16) | ((ch[2] & 0xFF) <<  8) | (ch[3] & 0xFF);
	}
	return *this; 
}

const ByteStream& ByteStream::operator >> ( uint32& l ) const { 
	uint8 ch[4];
	if( 0 == ReadRawData( ch, sizeof( ch ) ) ) {
		l = (uint32)(( ch[0] & 0xFF) << 24) | ((ch[1] & 0xFF) << 16) | ((ch[2] & 0xFF) <<  8) | (ch[3] & 0xFF);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
int32 ByteStream::WriteRawData( const void* pdata, uint32 dataLen ) {
	if (m_failed) {
		return -1;
	}

	if ( dataLen == 0 ) {
		return 0;
	}

	if( m_currentPos + dataLen > m_maxLen ) {
		if(m_isNew) {
			if(0==Alloc(m_currentPos + dataLen)) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}

	memcpy( m_pBuffer + m_currentPos, pdata, dataLen );
	m_currentPos += dataLen;

	if( m_dataLen < m_currentPos ) {
		m_dataLen = m_currentPos;
	}

	return 0;
}

int32 ByteStream::WriteString( const char* pStr ) {
	uint32	len = strlen( pStr );

	uint8 ch[4];
	ch[0] = (len >> 24) & 0xff;
	ch[1] = (len >> 16) & 0xff;
	ch[2] = (len >> 8) & 0xff;
	ch[3]= len & 0xff;

	if (0!=WriteRawData(ch, sizeof(ch))) {
		return -1;
	}

	return WriteRawData(pStr, len);
}

int32 ByteStream::WriteString(const std::string& str) {
	uint32	len = str.length();

	uint8 ch[4];
	ch[0] = (len >> 24) & 0xff;
	ch[1] = (len >> 16) & 0xff;
	ch[2] = (len >> 8) & 0xff;
	ch[3]= len & 0xff;

	if (0!=WriteRawData(ch, sizeof(ch))) {
		return -1;
	}

	return WriteRawData(str.c_str(), len);
}

ByteStream& ByteStream::operator << (const std::string& str) {
	WriteString(str);
	return *this; 
}

ByteStream& ByteStream::operator << ( bool b ) {
	uint8 ch = b ? 1:0;
	WriteRawData( &ch, sizeof( ch ) );
	return *this; 
}

ByteStream& ByteStream::operator << ( uint8 ch ) { 
	WriteRawData( &ch, sizeof( uint8 ));
	return *this; 
}

ByteStream& ByteStream::operator << ( char ch ) { 
	WriteRawData( &ch, sizeof( char ));
	return *this; 
}

ByteStream& ByteStream::operator << ( int16 s ) { 
	uint8 ch[2];
	ch[0] = (s >> 8) & 0xff;
	ch[1]= s & 0xff;
	WriteRawData( ch, sizeof( ch ));
	return *this; 
}

ByteStream& ByteStream::operator << ( uint16 s ) { 
	uint8 ch[2];
	ch[0] = (s >> 8) & 0xff;
	ch[1]= s & 0xff;
	WriteRawData( ch, sizeof( ch ));
	return *this; 
}

ByteStream& ByteStream::operator << ( int32 l ) { 
	uint8 ch[4];
	ch[0] = (l >> 24) & 0xff;
	ch[1] = (l >> 16) & 0xff;
	ch[2] = (l >> 8) & 0xff;
	ch[3]= l & 0xff;
	WriteRawData( ch, sizeof( ch ));
	return *this; 
}

ByteStream& ByteStream::operator << ( uint32 l ) {
	uint8 ch[4];
	ch[0] = (l >> 24) & 0xff;
	ch[1] = (l >> 16) & 0xff;
	ch[2] = (l >> 8) & 0xff;
	ch[3]= l & 0xff;
	WriteRawData( ch, sizeof( ch ));
	return *this; 
}

}
