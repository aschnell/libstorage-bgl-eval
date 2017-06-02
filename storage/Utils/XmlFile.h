/*
 * Copyright (c) [2010-2014] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_XML_FILE_H
#define STORAGE_XML_FILE_H


#include <libxml/tree.h>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <boost/noncopyable.hpp>

#include "storage/Utils/AppUtil.h"


namespace storage
{
    using namespace std;


    class XmlFile : private boost::noncopyable
    {

    public:

	XmlFile();
	XmlFile(const string& filename);

	~XmlFile();

	bool save(const string& filename)
	    { return xmlSaveFormatFile(filename.c_str(), doc, 1); }

	void setRootElement(xmlNode* node)
	    { xmlDocSetRootElement(doc, node); }

	const xmlNode* getRootElement() const
	    { return xmlDocGetRootElement(doc); }

    private:

	xmlDoc* doc;

    };


    xmlNode* xmlNewNode(const char* name);
    xmlNode* xmlNewComment(const char* content);

    xmlNode* xmlNewChild(xmlNode* node, const char* name);


    const xmlNode* getChildNode(const xmlNode* node, const char* name);
    list<const xmlNode*> getChildNodes(const xmlNode* node, const char* name);

    list<const xmlNode*> getChildNodes(const xmlNode* node);


    bool getChildValue(const xmlNode* node, const char* name, string& value);
    bool getChildValue(const xmlNode* node, const char* name, bool& value);

    template<typename Type>
    bool getChildValue(const xmlNode* node, const char* name, Type& value)
    {
	string tmp;
	if (!getChildValue(node, name, tmp))
	    return false;

	std::istringstream istr(tmp);
	classic(istr);
	istr >> value;
	return true;
    }

    template<typename Type>
    bool getChildValue(const xmlNode* node, const char* name, vector<Type>& values)
    {
	list<const xmlNode*> children = getChildNodes(node, name);

	for (const xmlNode*& child : children)
	    values.push_back((const char*) child->content);

	return !children.empty();
    }


    void setChildValue(xmlNode* node, const char* name, const char* value);
    void setChildValue(xmlNode* node, const char* name, const string& value);
    void setChildValue(xmlNode* node, const char* name, bool value);

    template<typename Num>
    void setChildValue(xmlNode* node, const char* name, const Num& value)
    {
	static_assert(std::is_integral<Num>::value, "not integral");

	std::ostringstream ostr;
	classic(ostr);
	ostr << value;
	setChildValue(node, name, ostr.str());
    }

    template<typename Type>
    void setChildValue(xmlNode* node, const char* name, const vector<Type>& values)
    {
	for (typename vector<Type>::const_iterator it = values.begin(); it != values.end(); ++it)
	    setChildValue(node, name, *it);
    }


    template<typename Type>
    void setChildValueIf(xmlNode* node, const char* name, const Type& value, bool pred)
    {
	if (pred)
	    setChildValue(node, name, value);
    }

}


#endif
