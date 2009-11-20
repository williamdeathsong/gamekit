#include "IrrBlendNew.h"

#include "autogenerated/blender.h"
#include "bMain.h"
#include "bBlenderFile.h"




BulletBlendReaderNew::BulletBlendReaderNew(class btDynamicsWorld* destinationWorld)
:m_blendFile(0)
{
}

BulletBlendReaderNew::~BulletBlendReaderNew()
{
	if (m_blendFile)
	{
		delete m_blendFile;
	}
}

	///if you already have a file pointer, call readFile
int		BulletBlendReaderNew::readFile(char* memoryBuffer, int fileLen, int verboseDumpAllTypes )
{
	if (m_blendFile)
	{
		delete m_blendFile;
	}

	m_blendFile = new bParse::bBlenderFile(memoryBuffer, fileLen);
	bool ok = (m_blendFile->getFlags()& bParse::FD_OK)!=0;
	if (ok)
		m_blendFile->parse(verboseDumpAllTypes);
	
//	m_blendFile->getMain()->dumpChunks(m_blendFile->getFileDNA());
	
	return ok;
}



void	BulletBlendReaderNew::convertAllObjects(int verboseDumpAllBlocks)
{
	btAssert(m_blendFile);

	bParse::bMain* mainPtr = m_blendFile->getMain();

	// bMain contains sorted void* pointers.

	// Iterate through the scene.
	bParse::bListBasePtr *sceneBase = mainPtr->getScene();
	int numScenes = sceneBase->size();
	printf("File contains %i scene(s)\n", numScenes);

	bParse::bListBasePtr* objBase = mainPtr->getObject();
	int numObj = objBase->size();
	printf("File contains %i object(s)\n", numObj);
	
	bParse::bListBasePtr* camBase = mainPtr->getCamera();
	int numCam = camBase->size();
	printf("File contains %i camera(s)\n", numCam);
	

	Blender::FileGlobal* glob = (Blender::FileGlobal*)m_blendFile->getFileGlobal();
	

#ifdef EXTRACT_ALL_SCENES
	for (int sce = 0; sce<numScenes; sce++)
	{
		// Get the scene structure.
		Blender::Scene *scene = (Blender::Scene*) sceneBase->at(sce);
#else
	{
		Blender::Scene* scene = (Blender::Scene*) glob->curscene;//mainPtr->findLibPointer(glob->curscene);
#endif
		// ListBase pointers must be linked
		//mainPtr->linkList(&scene->base);

		// Loop all objects in the scene.
		Blender::Base *base = (Blender::Base*)scene->base.first;
		while (base)
		{
			// Lookup object pointer.
			//base->object = (Blender::Object*)mainPtr->findLibPointer(base->object);

			if (base->object)
			{
				Blender::Object *ob = base->object;
				///only process objects in active scene layer
				if (ob->lay & scene->lay)
				{
					if (verboseDumpAllBlocks)
						printf("Current object : %s\n", ob->id.name);

					switch (ob->type)
					{
					//OB_MESH
					case 1:
						{
							if (verboseDumpAllBlocks)
							{
								printf("\tObject is a mesh\n");
							}

							// Lookup data pointer
							//ob->data = mainPtr->findLibPointer(ob->data);

							if (ob->data)
							{
								Blender::Mesh *me = (Blender::Mesh*)ob->data;
								if (verboseDumpAllBlocks)
								{
									printf("\t\tFound mesh data for %s\n", me->id.name);
									printf("\t\tTotal verts %i, faces %i\n", me->totvert, me->totface);
								}

								btCollisionObject* bulletObject = createBulletObject(ob);
								
								createGraphicsObject(ob,bulletObject);
							}
						}break;

					//OB_LAMP
					case 10:
						{

							if (verboseDumpAllBlocks)
							{
								printf ("\tObject is a lamp\n");
							}

							//ob->data = mainPtr->findLibPointer(ob->data);
							if (ob->data)
							{
								Blender::Lamp *la = (Blender::Lamp*)ob->data;
								if (verboseDumpAllBlocks)
								{
									printf("\t\tFound lamp data for %s\n", la->id.name);
									printf("\t\tRGB, %f,%f,%f\n", la->r, la->g, la->b);
								}
								
								addLight(ob);
							}

						}break;
					//OB_CAMERA
					case 11:
						{
							if (verboseDumpAllBlocks)
							{
								printf ("\tObject is a camera\n");
							}

							//ob->data = mainPtr->findLibPointer(ob->data);
							if (ob->data)
							{
								Blender::Camera *cam = (Blender::Camera*)ob->data;
								if (verboseDumpAllBlocks)
								{
									printf("\t\tFound camera data for %s\n", cam->id.name);
									printf("\t\t(lens, near, far) %f,%f,%f\n", cam->lens, cam->clipsta, cam->clipend);
								}
								
								addCamera(ob);
							}
						}break;
					}
				}
			}
			base = (Blender::Base*)base->next;
		}
	}

	if (verboseDumpAllBlocks)
	{
		mainPtr->dumpChunks(m_blendFile->getFileDNA());
	}

}



btCollisionObject* BulletBlendReaderNew::createBulletObject(Blender::Object* object)
{
#if 0
	Blender::Mesh *me = (Blender::Mesh*)object->data;

	//let's try to create some static collision shapes from the triangle meshes
	if (me)
	{
		btTriangleMesh* meshInterface = new btTriangleMesh();

		btVector3 minVert(1e30f,1e3f,1e30f);
		btVector3 maxVert(-1e30f,-1e30f,-1e30f);
		for (int t=0;t<me->totface;t++)
		{

			float* vt0 = object->data.mesh->vert[object->data.mesh->face[t].v[0]].xyz;
			btVector3 vtx0(vt0[0],vt0[1],vt0[2]);
			minVert.setMin(vtx0); maxVert.setMax(vtx0);
			float* vt1 = object->data.mesh->vert[object->data.mesh->face[t].v[1]].xyz;
			btVector3 vtx1(vt1[0],vt1[1],vt1[2]);
			minVert.setMin(vtx1); maxVert.setMax(vtx1);
			float* vt2 = object->data.mesh->vert[object->data.mesh->face[t].v[2]].xyz;
			btVector3 vtx2(vt2[0],vt2[1],vt2[2]);
			minVert.setMin(vtx2); maxVert.setMax(vtx2);
			meshInterface ->addTriangle(vtx0,vtx1,vtx2);

			if (object->data.mesh->face[t].v[3])
			{
				float* vt3 = object->data.mesh->vert[object->data.mesh->face[t].v[3]].xyz;
				btVector3 vtx3(vt3[0],vt3[1],vt3[2]);
				minVert.setMin(vtx3); maxVert.setMax(vtx3);
				meshInterface ->addTriangle(vtx0,vtx3,vtx2);//?
			}
		}
		if (!meshInterface->getNumTriangles())
			return 0;

/* boundtype */
#define OB_BOUND_BOX		0
#define OB_BOUND_SPHERE		1
#define OB_BOUND_CYLINDER	2
#define OB_BOUND_CONE		3
#define OB_BOUND_POLYH		4
#define OB_BOUND_POLYT		5
#define OB_BOUND_DYN_MESH   6

/* ob->gameflag */
#define OB_DYNAMIC		1
//#define OB_CHILD		2
//#define OB_ACTOR		4
//#define OB_INERTIA_LOCK_X	8
//#define OB_INERTIA_LOCK_Y	16
//#define OB_INERTIA_LOCK_Z	32
//#define OB_DO_FH			64
//#define OB_ROT_FH			128
//#define OB_ANISOTROPIC_FRICTION 256
//#define OB_GHOST			512
#define OB_RIGID_BODY		1024
//#define OB_BOUNDS		2048

//#define OB_COLLISION_RESPONSE	4096//??
//#define OB_COLLISION	65536
//#define OB_SOFT_BODY	0x20000

		btVector3 localPos = (minVert+maxVert)*0.5f;
		btVector3 localSize= (maxVert-minVert)*0.5f;


		btTransform	worldTrans;
		worldTrans.setIdentity();
		worldTrans.setOrigin(btVector3(object->location[0],object->location[1],object->location[2]));
		//		blenderobject->loc[0]+blenderobject->dloc[0]//??

		//btVector3 eulerXYZ(object->rotphr[0],object->rotphr[1],object->rotphr[2]);
		worldTrans.getBasis().setEulerZYX(object->rotphr[0],object->rotphr[1],object->rotphr[2]);
		btVector3 scale(object->scaling[0],object->scaling[1],object->scaling[2]);

		if ( (object->gameflag & OB_RIGID_BODY) || (object->gameflag & OB_DYNAMIC))
		{
			//m_destinationWorld->addRigidBody(
			btCollisionShape* colShape = 0;

			switch (object->boundtype)
			{
			case OB_BOUND_SPHERE:
				{
					btScalar radius = localSize[localSize.maxAxis()];
					colShape = new btSphereShape(radius);
					break;
				};
			case OB_BOUND_BOX:
				{
					colShape = new btBoxShape(localSize);
					break;
				}
			case OB_BOUND_CYLINDER:
				{
					colShape = new btCylinderShapeZ(localSize);
					break;
				}
			case OB_BOUND_CONE:
				{
					btScalar radius = btMax(localSize[0], localSize[1]);
					btScalar height = 2.f*localSize[2];
					colShape = new btConeShapeZ(radius,height);
					break;
				}
			case OB_BOUND_POLYT:

				{
					//better to approximate it, using btShapeHull
					colShape = new btConvexTriangleMeshShape(meshInterface);
					break;
				}
			case OB_BOUND_POLYH:
			case OB_BOUND_DYN_MESH:
				{
					btGImpactMeshShape* gimpact = new btGImpactMeshShape(meshInterface);
					gimpact->postUpdate();
					colShape = gimpact;
					break;
				}

			default:
				{

				}
			};

			if (colShape)
			{
				colShape->setLocalScaling(scale);
				btVector3 inertia;
				colShape->calculateLocalInertia(object->mass,inertia);
				btRigidBody* body = new btRigidBody(object->mass,0,colShape,inertia);
				if (!(object->gameflag & OB_RIGID_BODY))
				{
					body->setAngularFactor(0.f);
				}
				body->setWorldTransform(worldTrans);
				m_destinationWorld->addRigidBody(body);
				//body->setActivationState(DISABLE_DEACTIVATION);

				void* gfxObject = createGraphicsObject(object,body);
				body->setUserPointer(gfxObject);

				return body;
			}

		} else
		{

			btCollisionShape* colShape =0;
			if (meshInterface->getNumTriangles()>0)
			{
				btBvhTriangleMeshShape* childShape = new btBvhTriangleMeshShape(meshInterface,true);

				if (scale[0]!=1. || scale[1]!=1. || scale[2]!=1.)
				{
					colShape = new btScaledBvhTriangleMeshShape(childShape,scale);
				} else
				{
					colShape = childShape;
				}

				btVector3 inertia(0,0,0);
				btRigidBody* colObj = new btRigidBody(0.f,0,colShape,inertia);
				colObj->setWorldTransform(worldTrans);
				colObj->setCollisionShape(colShape);

				m_destinationWorld->addRigidBody(colObj);

				void* gfxObject = createGraphicsObject(object,colObj);
				colObj->setUserPointer(gfxObject);

				return colObj;

			}

		}
	}
#endif
	return 0;
}




void	BulletBlendReaderNew::convertConstraints()
{
}

