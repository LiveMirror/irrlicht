// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_MS3D_LOADER_

#include "IReadFile.h"
#include "os.h"
#include "CMS3DMeshFileLoader.h"
#include "CSkinnedMesh.h"



namespace irr
{
namespace scene
{

// byte-align structures
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

// File header
struct MS3DHeader
{
	c8 ID[10];
	s32 Version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	u8 Flags;
	f32 Vertex[3];
	s8 BoneID;
	u8 RefCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	u16 Flags;
	u16 VertexIndices[3];
	f32 VertexNormals[3][3];
	f32 S[3], T[3];
	u8 SmoothingGroup;
	u8 GroupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
    s8 Name[32];
    f32 Ambient[4];
    f32 Diffuse[4];
    f32 Specular[4];
    f32 Emissive[4];
    f32 Shininess;	// 0.0f - 128.0f
    f32 Transparency;	// 0.0f - 1.0f
    u8 Mode;	// 0, 1, 2 is unused now
    s8 Texture[128];
    s8 Alphamap[128];
} PACK_STRUCT;

// Joint information
struct MS3DJoint
{
	u8 Flags;
	s8 Name[32];
	s8 ParentName[32];
	f32 Rotation[3];
	f32 Translation[3];
	u16 NumRotationKeyframes;
	u16 NumTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	f32 Time;
	f32 Parameter[3];
} PACK_STRUCT;

// Default alignment
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

//! Constructor
CMS3DMeshFileLoader::CMS3DMeshFileLoader(video::IVideoDriver *driver)
: Driver(driver), AnimatedMesh(0)
{
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CMS3DMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".ms3d")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CMS3DMeshFileLoader::createMesh(irr::io::IReadFile* file)
{
	if (!file)
		return 0;

	AnimatedMesh = new CSkinnedMesh();

	if ( load(file) )
	{
		AnimatedMesh->finalize();
	}
	else
	{
		AnimatedMesh->drop();
		AnimatedMesh = 0;
	}

	return AnimatedMesh;
}


//! loads an md2 file
bool CMS3DMeshFileLoader::load(io::IReadFile* file)
{
	s32 i=0, j;

	if (!file)
		return false;

	// find file size
	size_t fileSize = file->getSize();

	// read whole file

	u8* buffer = new u8[fileSize];
	size_t read = (s32)file->read(buffer, fileSize);
	if (read != fileSize)
	{
		delete [] buffer;
		os::Printer::log("Could not read full file. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read header

	const u8 *pPtr = (u8*)((void*)buffer);
	MS3DHeader *pHeader = (MS3DHeader*)pPtr;
	pPtr += sizeof(MS3DHeader);

	if ( strncmp( pHeader->ID, "MS3D000000", 10 ) != 0 )
	{
		delete [] buffer;
		os::Printer::log("Not a valid Milkshape3D Model File. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

#ifdef __BIG_ENDIAN__
	pHeader->Version = os::Byteswap::byteswap(pHeader->Version);
#endif
	if ( pHeader->Version < 3 || pHeader->Version > 4 )
	{
		delete [] buffer;
		os::Printer::log("Only Milkshape3D version 1.3 and 1.4 is supported. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// get pointers to data

	// vertices
	u16 numVertices = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numVertices = os::Byteswap::byteswap(numVertices);
#endif
	pPtr += sizeof(u16);
	MS3DVertex *vertices = (MS3DVertex*)pPtr;
	pPtr += sizeof(MS3DVertex) * numVertices;
#ifdef __BIG_ENDIAN__
	for (i=0; i<numVertices; ++i)
		for (j=0; j<3; ++j)
			vertices[i].Vertex[j] = os::Byteswap::byteswap(vertices[i].Vertex[j]);
#endif

	// triangles
	u16 numTriangles = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numTriangles = os::Byteswap::byteswap(numTriangles);
#endif
	pPtr += sizeof(u16);
	MS3DTriangle *triangles = (MS3DTriangle*)pPtr;
	pPtr += sizeof(MS3DTriangle) * numTriangles;
#ifdef __BIG_ENDIAN__
	for (i=0; i<numTriangles; ++i)
	{
		triangles[i].Flags = os::Byteswap::byteswap(triangles[i].Flags);
		for (j=0; j<3; ++j)
		{
			triangles[i].VertexIndices[j] = os::Byteswap::byteswap(triangles[i].VertexIndices[j]);
			for (u16 k=0; k<3; ++k)
				triangles[i].VertexNormals[j][k] = os::Byteswap::byteswap(triangles[i].VertexNormals[j][k]);
			triangles[i].S[j] = os::Byteswap::byteswap(triangles[i].S[j]);
			triangles[i].T[j] = os::Byteswap::byteswap(triangles[i].T[j]);
		}
	}
#endif

	// groups
	u16 numGroups = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numGroups = os::Byteswap::byteswap(numGroups);
#endif
	pPtr += sizeof(u16);

	//skip groups
	for (i=0; i<numGroups; ++i)
	{
		Groups.push_back(SGroup());
		SGroup& grp = Groups.getLast();

		// The byte flag is before the name, so add 1
		grp.Name = ((const c8*) pPtr) + 1;

		pPtr += 33; // name and 1 byte flags
		u16 triangleCount = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
		triangleCount = os::Byteswap::byteswap(triangleCount);
#endif
		pPtr += sizeof(u16);

		//pPtr += sizeof(u16) * triangleCount; // triangle indices
		for (j=0; j<triangleCount; ++j)
		{
#ifdef __BIG_ENDIAN__
			grp.VertexIds.push_back(os::Byteswap::byteswap(*(u16*)pPtr));
#else
			grp.VertexIds.push_back(*(u16*)pPtr);
#endif
			pPtr += sizeof (u16);
		}

		grp.MaterialIdx = *(u8*)pPtr;
		if (grp.MaterialIdx == 255)
			grp.MaterialIdx = 0;

		pPtr += sizeof(c8); // material index
	}

	// skip materials
	u16 numMaterials = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numMaterials = os::Byteswap::byteswap(numMaterials);
#endif
	pPtr += sizeof(u16);

	// MS3DMaterial *materials = (MS3DMaterial*)pPtr;
	// pPtr += sizeof(MS3DMaterial) * numMaterials;

	if(numMaterials <= 0)
	{
		// if there are no materials, add at least one buffer

		AnimatedMesh->createBuffer();

	}

	for (i=0; i<numMaterials; ++i)
	{
		MS3DMaterial *material = (MS3DMaterial*)pPtr;
#ifdef __BIG_ENDIAN__
		for (j=0; j<4; ++j)
			material->Ambient[j] = os::Byteswap::byteswap(material->Ambient[j]);
		for (j=0; j<4; ++j)
			material->Diffuse[j] = os::Byteswap::byteswap(material->Diffuse[j]);
		for (j=0; j<4; ++j)
			material->Specular[j] = os::Byteswap::byteswap(material->Specular[j]);
		for (j=0; j<4; ++j)
			material->Emissive[j] = os::Byteswap::byteswap(material->Emissive[j]);
		material->Shininess = os::Byteswap::byteswap(material->Shininess);
		material->Transparency = os::Byteswap::byteswap(material->Transparency);
#endif
		pPtr += sizeof(MS3DMaterial);


		CSkinnedMesh::SSkinMeshBuffer *tmpBuffer = AnimatedMesh->createBuffer();

		tmpBuffer->Material.MaterialType = video::EMT_SOLID;

		tmpBuffer->Material.AmbientColor = video::SColorf(material->Ambient[0], material->Ambient[1], material->Ambient[2], material->Ambient[3]).toSColor ();
		tmpBuffer->Material.DiffuseColor = video::SColorf(material->Diffuse[0], material->Diffuse[1], material->Diffuse[2], material->Diffuse[3]).toSColor ();
		tmpBuffer->Material.EmissiveColor = video::SColorf(material->Emissive[0], material->Emissive[1], material->Emissive[2], material->Emissive[3]).toSColor ();
		tmpBuffer->Material.SpecularColor = video::SColorf(material->Specular[0], material->Specular[1], material->Specular[2], material->Specular[3]).toSColor ();
		tmpBuffer->Material.Shininess = material->Shininess;

		core::stringc TexturePath=(const c8*)material->Texture;
		TexturePath.trim();
		if (TexturePath!="")
		{
			TexturePath=stripPathFromString(file->getFileName(),true) + stripPathFromString(TexturePath,false);
			tmpBuffer->Material.Textures[0] = Driver->getTexture(TexturePath.c_str() );
		}

		core::stringc AlphamapPath=(const c8*)material->Alphamap;
		AlphamapPath.trim();
		if (AlphamapPath!="")
		{
			AlphamapPath=stripPathFromString(file->getFileName(),true) + stripPathFromString(AlphamapPath,false);
			tmpBuffer->Material.Textures[2] = Driver->getTexture(AlphamapPath.c_str() );
		}

	}

	// animation time
	f32 framesPerSecond = *(f32*)pPtr;
#ifdef __BIG_ENDIAN__
	framesPerSecond = os::Byteswap::byteswap(framesPerSecond);
#endif
	pPtr += sizeof(f32) * 2; // fps and current time


	if (framesPerSecond==0)
		framesPerSecond=1;

	s32 frameCount = *(s32*)pPtr;
#ifdef __BIG_ENDIAN__
	frameCount = os::Byteswap::byteswap(frameCount);
#endif
	pPtr += sizeof(s32);


	u16 jointCount = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	jointCount = os::Byteswap::byteswap(jointCount);
#endif
	pPtr += sizeof(u16);


	core::array<core::stringc> ParentNames;

	// load joints
	for (i=0; i<jointCount; ++i)
	{
		MS3DJoint *pJoint = (MS3DJoint*)pPtr;
#ifdef __BIG_ENDIAN__
		for (j=0; j<3; ++j)
			pJoint->Rotation[j] = os::Byteswap::byteswap(pJoint->Rotation[j]);
		for (j=0; j<3; ++j)
			pJoint->Translation[j] = os::Byteswap::byteswap(pJoint->Translation[j]);
		pJoint->NumRotationKeyframes= os::Byteswap::byteswap(pJoint->NumRotationKeyframes);
		pJoint->NumTranslationKeyframes = os::Byteswap::byteswap(pJoint->NumTranslationKeyframes);
#endif
		pPtr += sizeof(MS3DJoint);



		ISkinnedMesh::SJoint *jnt = AnimatedMesh->createJoint();

		/*
		jnt.Name = pJoint->Name;
		jnt.Index = i;
		jnt.Rotation.X = pJoint->Rotation[0];
		jnt.Rotation.Y = pJoint->Rotation[1];
		jnt.Rotation.Z = pJoint->Rotation[2];
		jnt.Translation.X = pJoint->Translation[0];
		jnt.Translation.Y = pJoint->Translation[1];
		jnt.Translation.Z = pJoint->Translation[2];
		jnt.ParentName = pJoint->ParentName;
		jnt.Parent = -1;
		*/

		jnt->Name = pJoint->Name;

		jnt->LocalMatrix.makeIdentity();


		jnt->LocalMatrix.setRotationRadians(
			core::vector3df(pJoint->Rotation[0], pJoint->Rotation[1], pJoint->Rotation[2]) );

		jnt->LocalMatrix.setTranslation(
			core::vector3df(pJoint->Translation[0], pJoint->Translation[1], pJoint->Translation[2]) );


		ParentNames.push_back( (c8*)pJoint->ParentName );

		/*if (pJoint->NumRotationKeyframes ||
			pJoint->NumTranslationKeyframes)
			HasAnimation = true;*/




		// get rotation keyframes
		for (j=0; j < pJoint->NumRotationKeyframes; ++j)
		{
			MS3DKeyframe* kf = (MS3DKeyframe*)pPtr;
#ifdef __BIG_ENDIAN__
			kf->Time = os::Byteswap::byteswap(kf->Time);
			for (u32 l=0; l<3; ++l)
				kf->Parameter[l] = os::Byteswap::byteswap(kf->Parameter[l]);
#endif
			pPtr += sizeof(MS3DKeyframe);

			ISkinnedMesh::SRotationKey *k=AnimatedMesh->createRotationKey(jnt);
			k->frame = kf->Time * framesPerSecond;

			core::matrix4 tmpMatrix;

			tmpMatrix.setRotationRadians(
				core::vector3df(kf->Parameter[0], kf->Parameter[1], kf->Parameter[2]) );

			tmpMatrix=jnt->LocalMatrix*tmpMatrix;

			k->rotation  = core::quaternion(tmpMatrix);

			//fix
			//k->rotation  = core::vector3df
			//	(kf->Parameter[0],//+pJoint->Rotation[0]*core::RADTODEG,
			//	 kf->Parameter[1],//+pJoint->Rotation[1]*core::RADTODEG,
			//	 kf->Parameter[2]);//+pJoint->Rotation[2]*core::RADTODEG);

		}

		// get translation keyframes
		for (j=0; j<pJoint->NumTranslationKeyframes; ++j)
		{
			MS3DKeyframe* kf = (MS3DKeyframe*)pPtr;
#ifdef __BIG_ENDIAN__
			kf->Time = os::Byteswap::byteswap(kf->Time);
			for (u32 l=0; l<3; ++l)
				kf->Parameter[l] = os::Byteswap::byteswap(kf->Parameter[l]);
#endif
			pPtr += sizeof(MS3DKeyframe);

			ISkinnedMesh::SPositionKey *k=AnimatedMesh->createPositionKey(jnt);
			k->frame = kf->Time * framesPerSecond;

			k->position = core::vector3df
				(kf->Parameter[0]+pJoint->Translation[0],
				 kf->Parameter[1]+pJoint->Translation[1],
				 kf->Parameter[2]+pJoint->Translation[2]);


		}
	}

	//find parent of every joint
	for (i=0; i<(s32)AnimatedMesh->getAllJoints().size(); ++i)
		for (j=0; j<(s32)AnimatedMesh->getAllJoints().size(); ++j)
			if (i != j && ParentNames[i] == AnimatedMesh->getAllJoints()[j]->Name )
			{
				AnimatedMesh->getAllJoints()[j]->Children.push_back(AnimatedMesh->getAllJoints()[i]);
				break;
			}

			/*if (Joints[i].Parent == -1)
				os::Printer::log("Found joint in model without parent.", ELL_WARNING);*/


	// create vertices and indices, attach them to the joints.
	video::S3DVertex v;
	core::array<video::S3DVertex> *Vertices;
	core::array<u16> Indices;

	for (i=0; i<numTriangles; ++i)
	{
		u32 tmp = Groups[triangles[i].GroupIndex].MaterialIdx;
		Vertices = &AnimatedMesh->getMeshBuffers()[tmp]->Vertices_Standard;

		for (j = 0; j<3; ++j)
		{
			v.TCoords.X = triangles[i].S[j];
			v.TCoords.Y = triangles[i].T[j];

			v.Normal.X = triangles[i].VertexNormals[j][0];
			v.Normal.Y = triangles[i].VertexNormals[j][1];
			v.Normal.Z = triangles[i].VertexNormals[j][2];

			if(triangles[i].GroupIndex < Groups.size() && Groups[triangles[i].GroupIndex].MaterialIdx < AnimatedMesh->getMeshBuffers().size())
				v.Color = AnimatedMesh->getMeshBuffers()[Groups[triangles[i].GroupIndex].MaterialIdx]->Material.DiffuseColor;
			else
				v.Color.set(255,255,255,255);
			v.Pos.X = vertices[triangles[i].VertexIndices[j]].Vertex[0];
			v.Pos.Y = vertices[triangles[i].VertexIndices[j]].Vertex[1];
			v.Pos.Z = vertices[triangles[i].VertexIndices[j]].Vertex[2];

			// check if we already have this vertex in our vertex array
			s32 index = -1;
			for (u32 iV = 0; iV < Vertices->size(); ++iV)
			{
				if (v == (*Vertices)[iV])
				{
					index = (s32)iV;
					break;
				}
			}
			if (index == -1)
			{
				s32 boneid = vertices[triangles[i].VertexIndices[j]].BoneID;
				if (boneid>=0 && boneid<(s32)AnimatedMesh->getAllJoints().size())
				{
					ISkinnedMesh::SWeight *w=AnimatedMesh->createWeight(AnimatedMesh->getAllJoints()[boneid]);
					w->buffer_id = Groups[triangles[i].GroupIndex].MaterialIdx;
					w->strength = 1.0f;
					w->vertex_id = Vertices->size();
					//Joints[boneid]->VertexIds.push_back(Vertices.size());


				}

				Vertices->push_back(v);
				index = Vertices->size() - 1;
			}
			Indices.push_back(index);
		}
	}

	//create groups
	s32 iIndex = -1;
	for (i=0; i<(int)Groups.size(); ++i)
	{
		SGroup& grp = Groups[i];

		if (grp.MaterialIdx >= AnimatedMesh->getMeshBuffers().size())
			grp.MaterialIdx = 0;

		core::array<u16>& indices = AnimatedMesh->getMeshBuffers()[grp.MaterialIdx]->Indices;

		for (u32 k=0; k < grp.VertexIds.size(); ++k)
			for (u32 l=0; l<3; ++l)
				indices.push_back(Indices[++iIndex]);
	}

	// calculate bounding box
/*
	// inverse translate and rotate all vertices for making animation easier
	if (HasAnimation)
	for (i=0; i<(s32)Joints.size(); ++i)
	{
		for (j=0; j<(s32)Joints[i].VertexIds.size(); ++j)
		{
			Joints[i].AbsoluteTransformation.inverseTranslateVect(
				Vertices[Joints[i].VertexIds[j]].Pos);

			Joints[i].AbsoluteTransformation.inverseRotateVect(
				Vertices[Joints[i].VertexIds[j]].Pos);

			Joints[i].AbsoluteTransformation.inverseRotateVect(
				Vertices[Joints[i].VertexIds[j]].Normal);
		}
	}

	AnimatedVertices = Vertices;
*/

	delete [] buffer;
	// clear arrays
	Groups.clear();

	return true;
}


core::stringc CMS3DMeshFileLoader::stripPathFromString(core::stringc string, bool returnPath)
{
	s32 slashIndex=string.findLast('/'); // forward slash
	s32 backSlash=string.findLast('\\'); // back slash

	if (backSlash>slashIndex) slashIndex=backSlash;

	if (slashIndex==-1)//no slashes found
		if (returnPath)
			return core::stringc(); //no path to return
		else
			return string;

	if (returnPath)
		return string.subString(0, slashIndex + 1);
	else
		return string.subString(slashIndex+1, string.size() - (slashIndex+1));
}


} // end namespace scene
} // end namespace irr

#endif