/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xalan" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */
#include "XercesToXalanNodeMap.hpp"



#include <algorithm>
#include <cassert>
#include <functional>



#include <dom/DOM_Document.hpp>



#include <XalanDOM/XalanDOMException.hpp>
#include <XalanDOM/XalanNode.hpp>



XercesToXalanNodeMap::XercesToXalanNodeMap() :
	m_xalanMap(),
	m_xercesMap()
{
}



XercesToXalanNodeMap::~XercesToXalanNodeMap()
{
}



void
XercesToXalanNodeMap::addAssociation(
			const DOM_Node&		theXercesNode,
			XalanNode*			theXalanNode)
{
	NodeImpl* const		theImpl = XercesDOM_NodeHack::getImpl(theXercesNode);

	m_xercesMap.insert(XercesNodeMapType::value_type(theImpl, theXalanNode));

	// Keeping two-way indexes is very memory consumptive, and we don't
	// need it now...
#if defined(XERCES_PARSER_LIASON_FAST_TWO_WAY_MAPPING)
	try
	{
		m_xalanMap.insert(XalanNodeMapType::value_type(theXalanNode, theImpl));
	}
	catch(...)
	{
		m_xercesMap.erase(m_xercesMap.find(theImpl));

		throw;
	}
#endif
}



void
XercesToXalanNodeMap::clear()
{
	m_xalanMap.clear();
	m_xercesMap.clear();
}



#if !defined(XERCES_PARSER_LIASON_FAST_TWO_WAY_MAPPING)

// I should be able to make this out of a
// bunch of compose<> and select2nd<> adapters...

class NameMapEqualsFunctor
{
public:

	NameMapEqualsFunctor(const XalanNode*	theXalanNode) :
		m_value(theXalanNode)
	{
	}

	bool
	operator()(const XercesToXalanNodeMap::XercesNodeMapType::value_type&	thePair) const
	{
		return m_value == thePair.second;
	}

private:

	const XalanNode*	m_value;
};



NodeImpl*
XercesToXalanNodeMap::getNodeImpl(const XalanNode*	theXalanNode) const
{
#if !defined(XALAN_NO_NAMESPACES)
	using std::find_if;
#endif

	const XercesNodeMapType::const_iterator		i =
		find_if(m_xercesMap.begin(),
				m_xercesMap.end(),
				NameMapEqualsFunctor(theXalanNode));

	if (i != m_xercesMap.end())
	{
		return (*i).first;
	}
	else
	{
		return 0;
	}
}

#endif