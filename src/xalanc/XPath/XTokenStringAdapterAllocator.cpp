/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the  "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Class header file.
#include "XTokenStringAdapterAllocator.hpp"



XALAN_CPP_NAMESPACE_BEGIN



XTokenStringAdapterAllocator::XTokenStringAdapterAllocator(MemoryManager&  theManager, size_type    theBlockCount) :
    m_allocator(theManager, theBlockCount)
{
}



XTokenStringAdapterAllocator::~XTokenStringAdapterAllocator()
{
}



XTokenStringAdapterAllocator::object_type*
XTokenStringAdapterAllocator::create(const XToken&  theXToken) 
{
    object_type* const  theBlock = m_allocator.allocateBlock();
    assert(theBlock != 0);

    object_type* const  theResult = new(theBlock) object_type(theXToken, m_allocator.getMemoryManager());

    m_allocator.commitAllocation(theBlock);

    return theResult;
}




bool 
XTokenStringAdapterAllocator::destroy(object_type*  theString)
{
    return m_allocator.destroyObject(theString);
}



void 
XTokenStringAdapterAllocator::reset()
{
    m_allocator.reset();
}



XALAN_CPP_NAMESPACE_END
