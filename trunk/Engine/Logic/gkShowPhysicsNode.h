/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkShowPhysicsNode_h_
#define _gkShowPhysicsNode_h_

#include "gkLogicNode.h"

class gkShowPhysicsNode : public gkLogicNode
{
public:

	enum
	{
		ENABLE,
		SHOW_AABB,
		MAX_SOCKETS
	};

    gkShowPhysicsNode(gkLogicTree *parent, size_t id);

	virtual ~gkShowPhysicsNode() {}

	bool evaluate(gkScalar tick);

    GK_INLINE gkLogicSocket* getEnable() {return &m_sockets[ENABLE];}
	GK_INLINE gkLogicSocket* getShowAabb() {return &m_sockets[SHOW_AABB];}

private:

	gkLogicSocket m_sockets[MAX_SOCKETS];
};


#endif//_gkShowPhysicsNode_h_