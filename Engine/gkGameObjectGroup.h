/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

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
#ifndef _gkGameObjectGroup_h_
#define _gkGameObjectGroup_h_

#include "gkCommon.h"
#include "gkTransformState.h"
#include "gkMathUtils.h"


///Groups are a list of game objects that are grouped together by a common name. 
///They manage gkGameObjectInstance objects which are created by cloning the group as a whole.
///This allows multiple objects from the same group to be placed anywhere in the scene together.
class gkGameObjectGroup
{
public:
	typedef utHashTable<gkHashedString, gkGameObject *>  Objects;
	typedef utHashSet<gkGameObjectInstance *>            Instances;


protected:

	Ogre::StaticGeometry    *m_geometry;

	const gkHashedString    m_name;

	Instances               m_instances;

	// for generating unique handles on created instances
	UTsize                  m_handle;

	Objects                 m_objects;

	gkGroupManager          *m_manager;


public:

	gkGameObjectGroup(gkGroupManager *manager, const gkHashedString &name);
	~gkGameObjectGroup();


	void addObject(gkGameObject *v);
	void removeObject(gkGameObject *v);

	///Destroys all gkGameObjectInstance objects managed by this group.
	void destroyAllInstances(void);



	bool           hasObject(const gkHashedString &name);
	gkGameObject  *getObject(const gkHashedString &name);


	gkGameObjectInstance *createInstance(gkScene *scene);
	void                  destroyInstance(gkGameObjectInstance *inst);


    ///This will group all meshes based on their material to be
    ///rendered by Ogre with one call to the underlying graphics API
    ///In OgreKit, this only works for truly static objects.
    ///Things like grass, tree leaves, or basically
    ///any gkEntity that does not respond to collisions (GK_NO_COLLISION).
	///\todo This needs a better static object check.
	void createStaticBatches(gkScene *scene);
	void destroyStaticBatches(gkScene *scene);


	///Places all gkGameObjectInstance objects in the Ogre scene 
	void createGameObjectInstances(void);

	///Removes all gkGameObjectInstance objects from the Ogre scene 
	void destroyGameObjectInstances(void);



	void cloneObjects(gkScene *scene,
	                  const gkTransformState &from, int time,
	                  const gkVector3 &linearVelocity=gkVector3::ZERO, 
					  bool tsLinLocal = true,
	                  const gkVector3 &angularVelocity=gkVector3::ZERO, 
					  bool tsAngLocal = true);


	GK_INLINE Instances                 &getInstances(void)      {return m_instances;}
	GK_INLINE const gkHashedString      &getName(void)           {return m_name;}
	GK_INLINE Objects                   &getObjects(void)        {return m_objects;}
	GK_INLINE bool                       isEmpty(void)           {return m_objects.empty();}
	GK_INLINE gkGroupManager            *getOwner(void)          {return m_manager;}

};

#endif//_gkGameObjectGroup_h_