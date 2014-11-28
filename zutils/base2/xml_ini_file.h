// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
//	使用xml文件实现windows下ini文件的读写设置。
//  可以参考windows下的ini文件读写配置类：CiniFile(iniFile.h)
//

/**
 * Edited By Wubenqi, 2009-03-12
 * add 
	GetStrings
	GetInts
	GetBools
	修改每次更新以后刷新配置文件
 */

#ifndef BASE2_XML_INI_FILE_H_
#define BASE2_XML_INI_FILE_H_

#include <string>
#include <vector>

#include "base2/markup_stl.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

/**
Windows Ini Examples File:
[Showyz]
Server=172.16.100.113
[Sign]
PfxFile=cert.pfx
[Database]
SignDBAddr=mysql://root:@localhost/eseal/
StampDBAddr=Provider=SQLOLEDB;Server=210.74.41.61;Database=stamp_online;UserId=stamper;Password=stamperkba97013

相应的 XMLIni Examples File:
<?xml version="1.0" encoding="gb2312"?>
<XMLIni>
 <Section name="Showyz">
  <Item name="Server">172.16.100.113</Item>
 </Section >
 <Section name="Sign">
  <Item name="PfxFile">cert.pfx</Item>
 </Section >
 <Section name="Database">
  <Item name="SignDBAddr">mysql://root:@localhost/eseal/</Item>
  <Item name="Server" >StampDBAddr=Provider=SQLOLEDB;Server=210.74.41.61;Database=stamp_online;UserId=stamper;Password=stamperkba97013</Item>
 </Section>
</XMLIni>
*/

/**
 * DESCRIPTION：
 *	首先需要调用LoadXMLIniFile方法，然后才能使用读写配置文件信息
 *	如果要在整个程序里要频繁操作XMLIniFile文件，可以做为一个单件使用，
 *	设置XMLIniFile元素值(调用Put函数)以后，设置的数据在内存里，如果需要立即更新文件，则要调用Flush函数
 */
class XMLIniFile {
public:
	XMLIniFile(void);
	virtual ~XMLIniFile(void);

public:
	/**
	 * DESCRIPTION：
	 *	装载XMLIni文件，如果文件不存在或打开出错， 返回false,
	 *  以后的操作无效
	 * return:
	 *  成功为true， 否则为false
	 */
	bool LoadXMLIniFile( const char* pstrFilename );
	
	/**
	 * DESCRIPTION：
	 *	获得配置信息值，返回char*
	 * param:
	 *  pstrSection，pstrKey，cchMax，pstrDefault为输入参数，pstrValue为输出参数
	 *  cchMax为返回参数pstrValue的缓冲最大值，如果超过这个长度，则只返回cchmax的大小
	 *  别的字符被自动截断
	 *	如果没有找到健值，则返回的是默认值，使用的时候要默认值长度不能超过
	 *  输入缓冲的最大长度。
	 * return:
	 *  成功为true， 如果输入参数为null，返回为false
	 */
	bool GetString( const char* pstrSection, const char* pstrKey, char* pstrValue,  unsigned int cchMax, const char* pstrDefault=NULL );
	
	/**
	 * DESCRIPTION：
	 *	获得配置信息值，返回string
	 * param:
	 *  pstrSection，pstrKey，pstrDefault为输入参数，sValue为输出参数
	 * return:
	 *  成功为true， 如果传入参数为null，返回为false
	 */
	bool GetString( const char* pstrSection, const char* pstrKey, std::string& sValue, const char* pstrDefault=NULL );
	bool GetStrings( const char* pstrSection, const char* pstrKey, std::vector<std::string>& sValue );
	

	/**
	 * DESCRIPTION：
	 *	获得配置信息值，返回int, 如果值不属于int类型，则输出参数iValue为0
	 * param:
	 *  pstrSection，pstrKey，iDefault为输入参数，iValue为输出参数
	 * return:
	 *  成功为true， 如果传入参数为null，返回为false
	 */
	bool GetInt( const char* pstrSection, const char* pstrKey, int& iValue,  int iDefault=0 );
	bool GetInts( const char* pstrSection, const char* pstrKey, std::vector<int>& iValue );

	/**
	 * DESCRIPTION：
	 *	获得配置信息值，返回bool
	 *	xml文件里的值的第一个字符值为 t/T/y/Y/1,返回true，为f/F/n/N/0，返回false 
	 *	都不是，返回默认值
	 * param:
	 *  pstrSection，pstrKey，bDefault为输入参数，bValue为输出参数
	 * return:
	 *  成功为true， 如果传入参数为null，返回为false
	 */
	bool GetBool( const char* pstrSection, const char* pstrKey, bool& bValue, bool bDefault=true );
	bool GetBools( const char* pstrSection, const char* pstrKey, std::vector<bool>& bValue );
	
	/**
	 * DESCRIPTION：
	 *	添加配置信息值,先在XMLIni文件里查找，找到更新为pstrValue
	 *	找不到，新添加一个pstrSection，pstrKey，pstrValue
	 * param:
	 *  pstrSection，pstrKey，pstrValue为输入参数
	 * return:
	 *  成功为true， 如果传入参数为null，返回为false
	 */
	bool PutString( const char* pstrSection, const char* pstrKey, const char* pstrValue );

	/**
	 * DESCRIPTION：
	 *	添加配置信息值，先在XMLIni文件里查找，找到更新，否则新增一个
	 * param:
	 *  pstrSection，pstrKey，iValue为输入参数
	 * return:
	 *  成功返回true， 如果传入参数为null，返回为false
	 */
	bool PutInt( const char* pstrSection, const char* pstrKey, int iValue );

	/**
	 * DESCRIPTION：
	 *	添加配置信息值, 先在XMLIni文件里查找，找到更新，否则新增一个
	 *	输入参数bValue在后，在XMLIni文件里的存储的值：bValue为true，则"true", 为false, 则"false"
	 * param:
	 *  pstrSection，pstrKey，bValue为输入参数
	 * return:
	 *  成功返回true， 如果传入参数为null，返回为false
	 */
	bool PutBool( const char* pstrSection, const char* pstrKey, bool bValue );

	/**
	 * DESCRIPTION：
	 *	删除配置信息值元素。
	 * param:
	 *  pstrSection，pstrKey
	 * return:
	 *  无返回值
	 */
	void DeleteKey( const char* pstrSection, const char* pstrKey );
	
	/**
	 * DESCRIPTION：
	 *	删除配置信息节点。
	 * param:
	 *  pstrSection
	 * return:
	 *  无返回值
	 */
	void DeleteSection( const char* pstrSection );
	
	/**
	 * DESCRIPTION：
	 *	将所做的操作保存到文件里。
	 * param:
	 *  无
	 * return:
	 *  无返回值
	 */	
	void Flush(void);

private:
	/**
	 * DESCRIPTION：
	 *	设置配置信息
	 * param:
	 *  输入: szSection，strItem，strValue
	 *	输出: 无
	 * return:
	 *  无返回值
	 */	
	void SetEntry ( const std::string& szSection, const std::string& strItem, const std::string& strValue );
	
	/**
	 * DESCRIPTION：
	 *	得到配置信息
	 * param:
	 *  输入: szSection，strItem
	 *	输出: strValue
	 * return:
	 *  无返回值
	 */	
	bool FindEntry( const std::string& szSection, const std::string& strItem, std::string& strValue );
	bool FindEntrys( const std::string& szSection, const std::string&, std::vector<std::string>& strValue );

   CMarkupSTL	m_xmlIniParser;				//xml文件处理类
   bool			m_bInited;					//初始化标志。如果文件打开出错，为false,否则为true, 值为false，后续操作都将失效
   char			m_szXMLIniFile[MAX_PATH];	//储存文件名，最终要用到。
   bool			updated_;
};

#endif //endif _XMLINIFILE_H_
