// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_COLLADA_WRITER_

#include "CColladaMeshWriter.h"
#include "os.h"
#include "IFileSystem.h"
#include "IWriteFile.h"
#include "IXMLWriter.h"
#include "IMesh.h"
#include "IAttributes.h"

namespace irr
{
namespace scene
{

//! Which lighting model should be used in the technique (FX) section when exporting effects (materials)
E_COLLADA_TECHNIQUE_FX CColladaMeshWriterProperties::getTechniqueFx(const video::SMaterial& material) const
{
	return ECTF_BLINN;
}

//! Which texture index should be used when setting the emissive texture
s32 CColladaMeshWriterProperties::getEmissiveTextureIdx(const video::SMaterial& material) const
{
	return 0;
}

//! Which texture index should be used when setting the ambient texture
s32 CColladaMeshWriterProperties::getAmbientTextureIdx(const video::SMaterial& material) const
{
	return 1;
}

//! Which texture index should be used when setting the diffuse texture
s32 CColladaMeshWriterProperties::getDiffuseTextureIdx(const video::SMaterial& material) const
{
	return 2;
}

//! Which texture index should be used when setting the specular texture
s32 CColladaMeshWriterProperties::getSpecularTextureIdx(const video::SMaterial& material) const
{
	return 3;
}

//! Which texture index should be used when writing the transparent texture
s32 CColladaMeshWriterProperties::getTransparentTextureIdx( const video::SMaterial& material) const
{
	// TODO
	return -1;
}

//! Return the settings for transparence
E_COLLADA_TRANSPARENT_FX CColladaMeshWriterProperties::getTransparentFx(const video::SMaterial& material) const
{
	// TODO
	return ECOF_A_ONE;
}

//! Transparency value for the material. 
f32 CColladaMeshWriterProperties::getTransparency(const video::SMaterial& material) const
{
	// TODO
	return -1.f;
}


CColladaMeshWriter::CColladaMeshWriter(video::IVideoDriver* driver,
					io::IFileSystem* fs)
	: FileSystem(fs), VideoDriver(driver), Writer(0)
{

	#ifdef _DEBUG
	setDebugName("CColladaMeshWriter");
	#endif

	if (VideoDriver)
		VideoDriver->grab();

	if (FileSystem)
		FileSystem->grab();

	CColladaMeshWriterProperties * p = new CColladaMeshWriterProperties();
	setDefaultProperties(p);
	setProperties(p);
	p->drop();
}


CColladaMeshWriter::~CColladaMeshWriter()
{
	if (VideoDriver)
		VideoDriver->drop();

	if (FileSystem)
		FileSystem->drop();
}


//! Returns the type of the mesh writer
EMESH_WRITER_TYPE CColladaMeshWriter::getType() const
{
	return EMWT_COLLADA;
}


//! writes a mesh
bool CColladaMeshWriter::writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags)
{
	if (!file)
		return false;

	Writer = FileSystem->createXMLWriter(file);

	if (!Writer)
	{
		os::Printer::log("Could not write file", file->getFileName());
		return false;
	}

	Directory = FileSystem->getFileDir(FileSystem->getAbsolutePath( file->getFileName() ));

	os::Printer::log("Writing mesh", file->getFileName());

	// write COLLADA header

	Writer->writeXMLHeader();

	Writer->writeElement(L"COLLADA", false,
		L"xmlns", L"http://www.collada.org/2005/11/COLLADASchema",
		L"version", L"1.4.1");
	Writer->writeLineBreak();

	// write asset data 

	Writer->writeElement(L"asset", false);
	Writer->writeLineBreak();

	Writer->writeElement(L"contributor", false);
	Writer->writeLineBreak();
	Writer->writeElement(L"authoring_tool", false);
	Writer->writeText(L"Irrlicht Engine / irrEdit");  // this code has originated from irrEdit 0.7
	Writer->writeClosingTag(L"authoring_tool");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"contributor");
	Writer->writeLineBreak();

	// The next two are required
	Writer->writeElement(L"created", false);
	Writer->writeText(L"2008-01-31T00:00:00Z");
	Writer->writeClosingTag(L"created");
	Writer->writeLineBreak();

	Writer->writeElement(L"modified", false);
	Writer->writeText(L"2008-01-31T00:00:00Z");
	Writer->writeClosingTag(L"modified");
	Writer->writeLineBreak();

	Writer->writeElement(L"revision", false);
	Writer->writeText(L"1.0");
	Writer->writeClosingTag(L"revision");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"asset");
	Writer->writeLineBreak();


	// write all materials

	Writer->writeElement(L"library_materials", false);
	Writer->writeLineBreak();

	u32 i;
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringw strMat = "mat";
		strMat += i;

		Writer->writeElement(L"material", false,
			L"id", strMat.c_str(),
			L"name", strMat.c_str());
		Writer->writeLineBreak();

		strMat += L"-fx";
		Writer->writeElement(L"instance_effect", true,
			L"url", (core::stringw(L"#") + strMat).c_str());
		Writer->writeLineBreak();

		Writer->writeClosingTag(L"material");
		Writer->writeLineBreak();
	}

	Writer->writeClosingTag(L"library_materials");
	Writer->writeLineBreak();

	Writer->writeElement(L"library_effects", false);
	Writer->writeLineBreak();
	
	LibraryImages.clear();
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringw strMat = "mat";
		strMat += i;
		strMat += L"-fx";

		Writer->writeElement(L"effect", false,
			L"id", strMat.c_str(),
			L"name", strMat.c_str());
		Writer->writeLineBreak();
		Writer->writeElement(L"profile_COMMON", false);
		Writer->writeLineBreak();

		video::SMaterial & material = mesh->getMeshBuffer(i)->getMaterial();

		int numTextures = 0;
		if ( getWriteTextures() )
		{
			// write texture surfaces and samplers and buffer all used imagess
			for ( int t=0; t<4; ++t )
			{
				video::SMaterialLayer& layer  = material.TextureLayer[t];
				if ( !layer.Texture )
					break;
				++numTextures;

				if ( LibraryImages.linear_search(layer.Texture) < 0 )
					 LibraryImages.push_back( layer.Texture );

				irr::core::stringw texName("tex");
				texName += irr::core::stringw(t);

				// write texture surface
				//<newparam sid="tex0-surface">
				irr::core::stringw texSurface(texName);
				texSurface += L"-surface";
				Writer->writeElement(L"newparam", false, L"sid", texSurface.c_str());
				Writer->writeLineBreak();
				//  <surface type="2D">
					Writer->writeElement(L"surface", false, L"type", L"2D");
					Writer->writeLineBreak();
    
			//          <init_from>internal_texturename</init_from>
						Writer->writeElement(L"init_from", false);
						irr::io::path p(FileSystem->getRelativeFilename(layer.Texture->getName().getPath(), Directory));
						Writer->writeText(pathToNCName(p).c_str());
						Writer->writeClosingTag(L"init_from");
						Writer->writeLineBreak();

			//          <format>A8R8G8B8</format>
						Writer->writeElement(L"format", false);
						video::ECOLOR_FORMAT format = layer.Texture->getColorFormat();
						Writer->writeText(toString(format).c_str());
						Writer->writeClosingTag(L"format");
						Writer->writeLineBreak();
			//      </surface>
					Writer->writeClosingTag(L"surface");
					Writer->writeLineBreak();
			//  </newparam>
				Writer->writeClosingTag(L"newparam");
				Writer->writeLineBreak();

				// write texture sampler
			//  <newparam sid="tex0-sampler">
				irr::core::stringw texSampler(texName);
				texSampler += L"-sampler";
				Writer->writeElement(L"newparam", false, L"sid", texSampler.c_str());
				Writer->writeLineBreak();
			//      <sampler2D>
					Writer->writeElement(L"sampler2D", false);
					Writer->writeLineBreak();

			//          <source>tex0-surface</source>
						Writer->writeElement(L"source", false);
						Writer->writeText(texSurface.c_str());
						Writer->writeClosingTag(L"source");
						Writer->writeLineBreak();

			//			<wrap_s>WRAP</wrap_s>
						Writer->writeElement(L"wrap_s", false);
						Writer->writeText(toString((video::E_TEXTURE_CLAMP)layer.TextureWrapU).c_str());
						Writer->writeClosingTag(L"wrap_s");
						Writer->writeLineBreak();

			//			<wrap_t>WRAP</wrap_t>
						Writer->writeElement(L"wrap_t", false);
						Writer->writeText(toString((video::E_TEXTURE_CLAMP)layer.TextureWrapV).c_str());
						Writer->writeClosingTag(L"wrap_t");
						Writer->writeLineBreak();

			//			<minfilter>LINEAR_MIPMAP_LINEAR</minfilter> 
						Writer->writeElement(L"minfilter", false);
						Writer->writeText(minTexfilterToString(layer.BilinearFilter, layer.TrilinearFilter).c_str());
						Writer->writeClosingTag(L"minfilter");
						Writer->writeLineBreak();

			//			<magfilter>LINEAR</magfilter>
						Writer->writeElement(L"magfilter", false);
						Writer->writeText(magTexfilterToString(layer.BilinearFilter, layer.TrilinearFilter).c_str());
						Writer->writeClosingTag(L"magfilter");
						Writer->writeLineBreak();

						// TBD - actually not sure how anisotropic should be written, so for now it writes in a way
						// that works with the way the loader reads it again.
						if ( layer.AnisotropicFilter )
						{
			//			<mipfilter>LINEAR_MIPMAP_LINEAR</mipfilter>
							Writer->writeElement(L"mipfilter", false);
							Writer->writeText(L"LINEAR_MIPMAP_LINEAR");
							Writer->writeClosingTag(L"mipfilter");
							Writer->writeLineBreak();
						}

			//     </sampler2D>
					Writer->writeClosingTag(L"sampler2D");
					Writer->writeLineBreak();
			//  </newparam>
				Writer->writeClosingTag(L"newparam");
				Writer->writeLineBreak();
			}
		}

		Writer->writeElement(L"technique", false, L"sid", L"common");
		Writer->writeLineBreak();

		E_COLLADA_TECHNIQUE_FX techFx = getProperties() ? getProperties()->getTechniqueFx(material) : ECTF_BLINN;
		writeFxElement(material, techFx);

		Writer->writeClosingTag(L"technique");
		Writer->writeLineBreak();
		Writer->writeClosingTag(L"profile_COMMON");
		Writer->writeLineBreak();
		Writer->writeClosingTag(L"effect");
		Writer->writeLineBreak();
	}

	Writer->writeClosingTag(L"library_effects");
	Writer->writeLineBreak();

	// images
	if ( getWriteTextures() && !LibraryImages.empty() )
	{
		Writer->writeElement(L"library_images", false);
		Writer->writeLineBreak();

		for ( irr::u32 i=0; i<LibraryImages.size(); ++i )
		{
			irr::io::path p(FileSystem->getRelativeFilename(LibraryImages[i]->getName().getPath(), Directory));
			//<image name="rose01"> 
			irr::core::stringw ncname(pathToNCName(p));
			Writer->writeElement(L"image", false, L"id", ncname.c_str(), L"name", ncname.c_str());
			Writer->writeLineBreak();
			//  <init_from>../flowers/rose01.jpg</init_from> 
				Writer->writeElement(L"init_from", false);
				Writer->writeText(pathToURI(p).c_str());
				Writer->writeClosingTag(L"init_from");
				Writer->writeLineBreak();
	 		//  </image> 
			Writer->writeClosingTag(L"image");
			Writer->writeLineBreak();
		}

		Writer->writeClosingTag(L"library_images");
		Writer->writeLineBreak();
	}

	// write mesh

	Writer->writeElement(L"library_geometries", false);
	Writer->writeLineBreak();

	Writer->writeElement(L"geometry", false, L"id", L"mesh", L"name", L"mesh");
	Writer->writeLineBreak();
	Writer->writeElement(L"mesh");
	Writer->writeLineBreak();

	// do some statistics for the mesh to know which stuff needs to be saved into
	// the file:
	// - count vertices
	// - check for the need of a second texture coordinate
	// - count amount of second texture coordinates
	// - check for the need of tangents (TODO)

	u32 totalVertexCount = 0;
	u32 totalTCoords2Count = 0;
	bool needsTangents = false; // TODO: tangents not supported here yet

	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		totalVertexCount += mesh->getMeshBuffer(i)->getVertexCount();

		if (hasSecondTextureCoordinates(mesh->getMeshBuffer(i)->getVertexType()))
			totalTCoords2Count += mesh->getMeshBuffer(i)->getVertexCount();

		if (!needsTangents)
			needsTangents = mesh->getMeshBuffer(i)->getVertexType() == video::EVT_TANGENTS;
	}

	SComponentGlobalStartPos* globalIndices = new SComponentGlobalStartPos[mesh->getMeshBufferCount()];

	// write positions

	Writer->writeElement(L"source", false, L"id", L"mesh-Pos");
	Writer->writeLineBreak();

	core::stringw vertexCountStr(totalVertexCount*3);
	Writer->writeElement(L"float_array", false, L"id", L"mesh-Pos-array",
				L"count", vertexCountStr.c_str());
	Writer->writeLineBreak();

	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
		video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
		u32 vertexCount = buffer->getVertexCount();

		globalIndices[i].PosStartIndex = 0;

		if (i!=0)
			globalIndices[i].PosStartIndex = globalIndices[i-1].PosLastIndex + 1;

		globalIndices[i].PosLastIndex = globalIndices[i].PosStartIndex + vertexCount - 1;

		switch(vtxType)
		{
		case video::EVT_STANDARD:
			{
				video::S3DVertex* vtx = (video::S3DVertex*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].Pos).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_2TCOORDS:
			{
				video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].Pos).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_TANGENTS:
			{
				video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].Pos).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		}
	}

	Writer->writeClosingTag(L"float_array");
	Writer->writeLineBreak();

	Writer->writeElement(L"technique_common", false);
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount);

		Writer->writeElement(L"accessor", false, L"source", L"#mesh-Pos-array",
					L"count", vertexCountStr.c_str(), L"stride", L"3");
		Writer->writeLineBreak();

			Writer->writeElement(L"param", true, L"name", L"X", L"type", L"float");
			Writer->writeLineBreak();
			Writer->writeElement(L"param", true, L"name", L"Y", L"type", L"float");
			Writer->writeLineBreak();
			Writer->writeElement(L"param", true, L"name", L"Z", L"type", L"float");
			Writer->writeLineBreak();

			Writer->writeClosingTag(L"accessor");
			Writer->writeLineBreak();

	Writer->writeClosingTag(L"technique_common");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"source");
	Writer->writeLineBreak();

	// write texture coordinates

	Writer->writeElement(L"source", false, L"id", L"mesh-TexCoord0");
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount*2);
	Writer->writeElement(L"float_array", false, L"id", L"mesh-TexCoord0-array",
				L"count", vertexCountStr.c_str());
	Writer->writeLineBreak();

	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
		video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
		u32 vertexCount = buffer->getVertexCount();

		globalIndices[i].TCoord0StartIndex = 0;

		if (i!=0)
			globalIndices[i].TCoord0StartIndex = globalIndices[i-1].TCoord0LastIndex + 1;

		globalIndices[i].TCoord0LastIndex = globalIndices[i].TCoord0StartIndex + vertexCount - 1;

		switch(vtxType)
		{
		case video::EVT_STANDARD:
			{
				video::S3DVertex* vtx = (video::S3DVertex*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].TCoords).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_2TCOORDS:
			{
				video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].TCoords).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_TANGENTS:
			{
				video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].TCoords).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		}
	}

	Writer->writeClosingTag(L"float_array");
	Writer->writeLineBreak();

	Writer->writeElement(L"technique_common", false);
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount);

		Writer->writeElement(L"accessor", false, L"source", L"#mesh-TexCoord0-array",
					L"count", vertexCountStr.c_str(), L"stride", L"2");
		Writer->writeLineBreak();

			Writer->writeElement(L"param", true, L"name", L"U", L"type", L"float");
			Writer->writeLineBreak();
			Writer->writeElement(L"param", true, L"name", L"V", L"type", L"float");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"accessor");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"technique_common");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"source");
	Writer->writeLineBreak();

	// write normals

	Writer->writeElement(L"source", false, L"id", L"mesh-Normal");
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount*3);
	Writer->writeElement(L"float_array", false, L"id", L"mesh-Normal-array",
				L"count", vertexCountStr.c_str());
	Writer->writeLineBreak();

	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
		video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
		u32 vertexCount = buffer->getVertexCount();

		globalIndices[i].NormalStartIndex = 0;

		if (i!=0)
			globalIndices[i].NormalStartIndex = globalIndices[i-1].NormalLastIndex + 1;

		globalIndices[i].NormalLastIndex = globalIndices[i].NormalStartIndex + vertexCount - 1;

		switch(vtxType)
		{
		case video::EVT_STANDARD:
			{
				video::S3DVertex* vtx = (video::S3DVertex*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].Normal).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_2TCOORDS:
			{
				video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].Normal).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_TANGENTS:
			{
				video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].Normal).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		}
	}

	Writer->writeClosingTag(L"float_array");
	Writer->writeLineBreak();

	Writer->writeElement(L"technique_common", false);
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount);

		Writer->writeElement(L"accessor", false, L"source", L"#mesh-Normal-array",
								L"count", vertexCountStr.c_str(), L"stride", L"3");
		Writer->writeLineBreak();

			Writer->writeElement(L"param", true, L"name", L"X", L"type", L"float");
			Writer->writeLineBreak();
			Writer->writeElement(L"param", true, L"name", L"Y", L"type", L"float");
			Writer->writeLineBreak();
			Writer->writeElement(L"param", true, L"name", L"Z", L"type", L"float");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"accessor");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"technique_common");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"source");
	Writer->writeLineBreak();

	// write second set of texture coordinates

	if (totalTCoords2Count)
	{
		Writer->writeElement(L"source", false, L"id", L"mesh-TexCoord1");
		Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalTCoords2Count*2);
		Writer->writeElement(L"float_array", false, L"id", L"mesh-TexCoord1-array",
								L"count", vertexCountStr.c_str());
		Writer->writeLineBreak();

		for (i=0; i<mesh->getMeshBufferCount(); ++i)
		{
			scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
			video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
			u32 vertexCount = buffer->getVertexCount();

			if (hasSecondTextureCoordinates(vtxType))
			{
				globalIndices[i].TCoord1StartIndex = 0;

				if (i!=0 && globalIndices[i-1].TCoord1LastIndex != -1)
					globalIndices[i].TCoord1StartIndex = globalIndices[i-1].TCoord1LastIndex + 1;

				globalIndices[i].TCoord1LastIndex = globalIndices[i].TCoord1StartIndex + vertexCount - 1;

				switch(vtxType)
				{
				case video::EVT_2TCOORDS:
					{
						video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
						for (u32 j=0; j<vertexCount; ++j)
						{
							Writer->writeText(toString(vtx[j].TCoords2).c_str());
							Writer->writeLineBreak();
						}
					}
					break;
				default:
					break;
				}
			} // end this buffer has 2 texture coordinates
		}

		Writer->writeClosingTag(L"float_array");
		Writer->writeLineBreak();

		Writer->writeElement(L"technique_common", false);
		Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalTCoords2Count);

			Writer->writeElement(L"accessor", false, L"source", L"#mesh-TexCoord1-array",
									L"count", vertexCountStr.c_str(), L"stride", L"2");
			Writer->writeLineBreak();

				Writer->writeElement(L"param", true, L"name", L"U", L"type", L"float");
				Writer->writeLineBreak();
				Writer->writeElement(L"param", true, L"name", L"V", L"type", L"float");
				Writer->writeLineBreak();

			Writer->writeClosingTag(L"accessor");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"technique_common");
		Writer->writeLineBreak();

		Writer->writeClosingTag(L"source");
		Writer->writeLineBreak();
	}

	// write tangents

	// TODO

	// write vertices

	Writer->writeElement(L"vertices", false, L"id", L"mesh-Vtx");
	Writer->writeLineBreak();

	Writer->writeElement(L"input", true, L"semantic", L"POSITION", L"source", L"#mesh-Pos");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"vertices");
	Writer->writeLineBreak();

	// write polygons

	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);

		const u32 polyCount = buffer->getIndexCount() / 3;
		core::stringw strPolyCount(polyCount);
		core::stringw strMat = "mat";
		strMat += i;

		Writer->writeElement(L"triangles", false, L"count", strPolyCount.c_str(),
								L"material", strMat.c_str());
		Writer->writeLineBreak();

		Writer->writeElement(L"input", true, L"semantic", L"VERTEX", L"source", L"#mesh-Vtx", L"offset", L"0");
		Writer->writeLineBreak();
		Writer->writeElement(L"input", true, L"semantic", L"TEXCOORD", L"source", L"#mesh-TexCoord0", L"offset", L"1");
		Writer->writeLineBreak();
		Writer->writeElement(L"input", true, L"semantic", L"NORMAL", L"source", L"#mesh-Normal", L"offset", L"2");
		Writer->writeLineBreak();

		bool has2ndTexCoords = hasSecondTextureCoordinates(buffer->getVertexType());
		if (has2ndTexCoords)
		{
			Writer->writeElement(L"input", true, L"semantic", L"TEXCOORD", L"source", L"#mesh-TexCoord1", L"idx", L"3");
			Writer->writeLineBreak();
		}

		// write indices now

		s32 posIdx = globalIndices[i].PosStartIndex;
		s32 tCoordIdx = globalIndices[i].TCoord0StartIndex;
		s32 normalIdx = globalIndices[i].NormalStartIndex;
		s32 tCoord2Idx = globalIndices[i].TCoord1StartIndex;

		Writer->writeElement(L"p", false);

		for (u32 p=0; p<polyCount; ++p)
		{
			core::stringw strP;

			strP += buffer->getIndices()[(p*3) + 0] + posIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 0] + tCoordIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 0] + normalIdx;
			strP += " ";
			if (has2ndTexCoords)
			{
				strP += buffer->getIndices()[(p*3) + 0] + tCoord2Idx;
				strP += " ";
			}

			strP += buffer->getIndices()[(p*3) + 1] + posIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 1] + tCoordIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 1] + normalIdx;
			strP += " ";
			if (has2ndTexCoords)
			{
				strP += buffer->getIndices()[(p*3) + 1] + tCoord2Idx;
				strP += " ";
			}

			strP += buffer->getIndices()[(p*3) + 2] + posIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 2] + tCoordIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 2] + normalIdx;
			if (has2ndTexCoords)
			{
				strP += " ";
				strP += buffer->getIndices()[(p*3) + 2] + tCoord2Idx;
			}
			strP += " ";

			Writer->writeText(strP.c_str());
		}

		Writer->writeClosingTag(L"p");
		Writer->writeLineBreak();

		// close index buffer section

		Writer->writeClosingTag(L"triangles");
		Writer->writeLineBreak();
	}

	// close mesh and geometry

	Writer->writeClosingTag(L"mesh");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"geometry");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"library_geometries");
	Writer->writeLineBreak();


	// write scene_library
	if ( getWriteScene() )
	{
		Writer->writeElement(L"library_visual_scenes", false);
		Writer->writeLineBreak();

			Writer->writeElement(L"visual_scene", false, L"id", L"default_scene");
			Writer->writeLineBreak();

				Writer->writeElement(L"node", false);
				Writer->writeLineBreak();

					//<instance_geometry url="#mesh">
					Writer->writeElement(L"instance_geometry", false, L"url", L"#mesh");
					Writer->writeLineBreak();

						Writer->writeElement(L"bind_material", false);
						Writer->writeLineBreak();

							Writer->writeElement(L"technique_common", false);
							Writer->writeLineBreak();
						
							// instance materials
							// <instance_material symbol="leaf" target="#MidsummerLeaf01"/> 
							for (i=0; i<mesh->getMeshBufferCount(); ++i)
							{
								core::stringw strMat = "mat";
								strMat += i;
								core::stringw strMatInst(L"#");
								strMatInst += strMat;
								Writer->writeElement(L"instance_material", false, L"symbol", strMat.c_str(), L"target", strMatInst.c_str());
								Writer->writeLineBreak();

									// <bind_vertex_input semantic="mesh-TexCoord0" input_semantic="TEXCOORD"/>
									Writer->writeElement(L"bind_vertex_input", true, L"semantic", L"mesh-TexCoord0", L"input_semantic", L"TEXCOORD" );
									Writer->writeLineBreak();

								Writer->writeClosingTag(L"instance_material");
								Writer->writeLineBreak();
							}
						
							Writer->writeClosingTag(L"technique_common");
							Writer->writeLineBreak();

						Writer->writeClosingTag(L"bind_material");
						Writer->writeLineBreak();

					Writer->writeClosingTag(L"instance_geometry");
					Writer->writeLineBreak();

				Writer->writeClosingTag(L"node");
				Writer->writeLineBreak();

			Writer->writeClosingTag(L"visual_scene");
			Writer->writeLineBreak();


		Writer->writeClosingTag(L"library_visual_scenes");
		Writer->writeLineBreak();


		// write scene
		Writer->writeElement(L"scene", false);
		Writer->writeLineBreak();

			Writer->writeElement(L"instance_visual_scene", true, L"url", L"#default_scene");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"scene");
		Writer->writeLineBreak();
	}


	// close everything

	Writer->writeClosingTag(L"COLLADA");
	Writer->drop();

	delete [] globalIndices;

	return true;
}


bool CColladaMeshWriter::hasSecondTextureCoordinates(video::E_VERTEX_TYPE type) const
{
	return type == video::EVT_2TCOORDS;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::core::vector3df& vec) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f %f", vec.X, vec.Y, vec.Z);
	core::stringw str  = tmpbuf;

	return str;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::core::vector2df& vec) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f", vec.X, vec.Y);
	core::stringw str  = tmpbuf;

	return str;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::video::SColorf& colorf) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f %f %f", colorf.getRed(), colorf.getGreen(), colorf.getBlue(), colorf.getAlpha());
	core::stringw str = tmpbuf;

	return str;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::video::ECOLOR_FORMAT format) const
{
	switch ( format )
	{
		case video::ECF_A1R5G5B5:	return irr::core::stringw(L"A1R5G5B5");
		case video::ECF_R5G6B5:		return irr::core::stringw(L"R5G6B5");
		case video::ECF_R8G8B8:		return irr::core::stringw(L"R8G8B8");
		case video::ECF_A8R8G8B8:	return irr::core::stringw(L"A8R8G8B8");
		default:					return irr::core::stringw(L"");
	}
}

irr::core::stringw CColladaMeshWriter::toString(const irr::video::E_TEXTURE_CLAMP clamp) const
{
	switch ( clamp )
	{
		case video::ETC_REPEAT:	
			return core::stringw(L"WRAP");
		case video::ETC_CLAMP:
		case video::ETC_CLAMP_TO_EDGE:
			return core::stringw(L"CLAMP");
		case video::ETC_CLAMP_TO_BORDER:
			return core::stringw(L"BORDER");
		case video::ETC_MIRROR:
		case video::ETC_MIRROR_CLAMP:
		case video::ETC_MIRROR_CLAMP_TO_EDGE:	
		case video::ETC_MIRROR_CLAMP_TO_BORDER:	
			return core::stringw(L"MIRROR");
	}
	return core::stringw(L"NONE");
}

irr::core::stringw CColladaMeshWriter::toString(const irr::scene::E_COLLADA_TRANSPARENT_FX transparent) const
{
	if ( transparent & ECOF_RGB_ZERO )
		return core::stringw(L"RGB_ZERO");
	else
		return core::stringw(L"A_ONE");
}

irr::core::stringw CColladaMeshWriter::minTexfilterToString(bool bilinear, bool trilinear) const
{
	if ( trilinear )
		return core::stringw(L"LINEAR_MIPMAP_LINEAR");
	else if ( bilinear )
		return core::stringw(L"LINEAR_MIPMAP_NEAREST");
	
	return core::stringw(L"NONE");
}

inline irr::core::stringw CColladaMeshWriter::magTexfilterToString(bool bilinear, bool trilinear) const
{
	if ( bilinear || trilinear )
		return core::stringw(L"LINEAR");

	return core::stringw(L"NONE");
}

bool CColladaMeshWriter::isXmlNameStartChar(wchar_t c) const
{
	return		(c >= 'A' && c <= 'Z')
			||	c == L'_'
			||	(c >= 'a' && c <= 'z')
			||	(c >= 0xC0 && c <= 0xD6)
			||	(c >= 0xD8 && c <= 0xF6)
			||	(c >= 0xF8 && c <= 0x2FF)
			||  (c >= 0x370 && c <= 0x37D)
			||  (c >= 0x37F && c <= 0x1FFF)
			||  (c >= 0x200C && c <= 0x200D)
			||  (c >= 0x2070 && c <= 0x218F)
			||  (c >= 0x2C00 && c <= 0x2FEF) 
			||  (c >= 0x3001 && c <= 0xD7FF)
			||  (c >= 0xF900 && c <= 0xFDCF)
			||  (c >= 0xFDF0 && c <= 0xFFFD)
			||  (c >= 0x10000 && c <= 0xEFFFF);
}

bool CColladaMeshWriter::isXmlNameChar(wchar_t c) const
{
	return isXmlNameStartChar(c)
		||	c == L'-' 
		||	c == L'.'
		||	(c >= '0' && c <= '9')
		||	c == 0xB7 
		||	(c >= 0x0300 && c <= 0x036F)
		||	(c >= 0x203F && c <= 0x2040);
}

// Restrict the characters to a set of allowed characters in xs::NCName.
irr::core::stringw CColladaMeshWriter::pathToNCName(const irr::io::path& path) const
{
	irr::core::stringw result(L"_NCNAME_");	// ensure id starts with a valid char and reduce chance of name-conflicts
	if ( path.empty() )
		return result;

	result.append( irr::core::stringw(path) );

	// We replace all characters not allowed by a replacement char
	const wchar_t REPLACMENT = L'-';
	for ( irr::u32 i=1; i < result.size(); ++i )
	{
		if ( result[i] == L':' || !isXmlNameChar(result[i]) )
		{
			result[i] = REPLACMENT;
		}
	}
	return result;
}

irr::core::stringw CColladaMeshWriter::pathToURI(const irr::io::path& path) const
{
	irr::core::stringw result;

	// is this a relative path?
	if ( path.size() > 1 
		&& path[0] != _IRR_TEXT('/') 
		&& path[0] != _IRR_TEXT('\\')
		&& path[1] != _IRR_TEXT(':') )
	{
		// not already starting with "./" ?
		if (	path[0] != _IRR_TEXT('.') 
			||	path[1] != _IRR_TEXT('/') )
		{
			result.append(L"./");
		}
	}
	result.append(path);

	// TODO: make correct URI (without whitespaces)

	return result;
}

void CColladaMeshWriter::writeColorElement(const video::SColor & col)
{
	Writer->writeElement(L"color", false);

	irr::core::stringw str( toString(video::SColorf(col)) );
	Writer->writeText(str.c_str());

	Writer->writeClosingTag(L"color");
	Writer->writeLineBreak();
}

s32 CColladaMeshWriter::getTextureIdx(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs)
{
	if (	!getWriteTextures()
		||	!getProperties() )
		return -1;

	s32 idx = -1;
	switch ( cs )
	{
		case ECS_DIFFUSE:
			idx = getProperties()->getDiffuseTextureIdx(material);
			break;
		case ECS_AMBIENT:
			idx = getProperties()->getAmbientTextureIdx(material);
			break;
		case ECS_EMISSIVE:
			idx = getProperties()->getEmissiveTextureIdx(material);
			break;
		case ECS_SPECULAR:
			idx = getProperties()->getSpecularTextureIdx(material);
			break;
		case ECS_TRANSPARENT:
			idx = getProperties()->getTransparentTextureIdx(material);
			break;
	}

	if ( idx >= 0 && !material.TextureLayer[idx].Texture )
		return -1;

	return idx;
}

bool CColladaMeshWriter::writeTextureSampler(s32 textureIdx)
{
	if ( textureIdx < 0 )
		return false;

	irr::core::stringw sampler(L"tex");
	sampler += irr::core::stringw(textureIdx);
	sampler += L"-sampler";

	// <texture texture="sampler" texcoord="texCoord"/>
	Writer->writeElement(L"texture", true, L"texture", sampler.c_str(), L"texcoord", L"mesh-TexCoord0" );
	Writer->writeLineBreak();

	return true;
}

void CColladaMeshWriter::writeFxElement(const video::SMaterial & material, E_COLLADA_TECHNIQUE_FX techFx)
{
	core::stringw fxLabel;
	bool writeEmission = true;
	bool writeAmbient = true;
	bool writeDiffuse = true;
	bool writeSpecular = true;
	bool writeShininess = true;
	bool writeReflective = true;
	bool writeReflectivity = true;
	bool writeTransparent = true;
	bool writeTransparency = true;
	bool writeIndexOfRefraction = true;
	switch ( techFx )
	{
		case ECTF_BLINN:
			fxLabel = L"blinn";
			break;
		case ECTF_PHONG:
			fxLabel = L"phong";
			break;
		case ECTF_LAMBERT:
			fxLabel = L"lambert";
			writeSpecular = false;
			writeShininess = false;
			break;
		case ECTF_CONSTANT:
			fxLabel = L"constant";
			writeAmbient = false;
			writeDiffuse = false;
			writeSpecular = false;
			writeShininess = false;
			break;
	}

	Writer->writeElement(fxLabel.c_str(), false);
	Writer->writeLineBreak();

	// write all interesting material parameters 
	// attributes must be written in fixed order
	if ( writeEmission )
	{
		Writer->writeElement(L"emission", false);
		Writer->writeLineBreak();
		if ( !writeTextureSampler( getTextureIdx(material, ECS_EMISSIVE)) )
			writeColorElement(material.EmissiveColor);
		Writer->writeClosingTag(L"emission");
		Writer->writeLineBreak();
	}

	if ( writeAmbient )
	{
		Writer->writeElement(L"ambient", false);
		Writer->writeLineBreak();
		if ( !writeTextureSampler( getTextureIdx(material, ECS_AMBIENT)) )
			writeColorElement(material.AmbientColor);
		Writer->writeClosingTag(L"ambient");
		Writer->writeLineBreak();
	}

	if ( writeDiffuse )
	{
		Writer->writeElement(L"diffuse", false);
		Writer->writeLineBreak();
		if ( !writeTextureSampler( getTextureIdx(material, ECS_DIFFUSE)) )
			writeColorElement(material.DiffuseColor);
		Writer->writeClosingTag(L"diffuse");
		Writer->writeLineBreak();
	}

	if ( writeSpecular )
	{
		Writer->writeElement(L"specular", false);
		Writer->writeLineBreak();
		if ( !writeTextureSampler( getTextureIdx(material, ECS_SPECULAR)) )
			writeColorElement(material.SpecularColor);
		Writer->writeClosingTag(L"specular");
		Writer->writeLineBreak();
	}

	if ( writeShininess )
	{
		Writer->writeElement(L"shininess", false);
		Writer->writeLineBreak();
		writeFloatElement(material.Shininess);
		Writer->writeClosingTag(L"shininess");
		Writer->writeLineBreak();
	}

	if ( writeReflective )
	{
		// TODO reflective
	}
	if ( writeReflectivity )
	{
		// TODO reflectivity
	}

	if ( writeTransparent )
	{
		s32 textureIdx = getTextureIdx(material, ECS_TRANSPARENT);
		E_COLLADA_TRANSPARENT_FX transparentFx = getProperties() ? getProperties()->getTransparentFx(material) : ECOF_A_ONE;
		if ( textureIdx >= 0 || transparentFx >= ECOF_TRANSPARENT_DIFFUSE )
		{
			Writer->writeElement(L"transparent", false, L"opaque", toString(transparentFx).c_str());
			Writer->writeLineBreak();
			if ( !writeTextureSampler(textureIdx) )
			{
				if ( transparentFx & ECOF_TRANSPARENT_DIFFUSE )
					writeColorElement(material.DiffuseColor);
				else if ( transparentFx & ECOF_TRANSPARENT_AMBIENT)
					writeColorElement(material.AmbientColor);
				else if ( transparentFx & ECOF_TRANSPARENT_EMISSIVE )
					writeColorElement(material.EmissiveColor);
				else if ( transparentFx & ECOF_TRANSPARENT_SPECULAR )
					writeColorElement(material.SpecularColor);
			}
			Writer->writeClosingTag(L"transparent");
			Writer->writeLineBreak();
		}
	}

	if ( writeTransparency && getProperties() )
	{
		f32 t = getProperties()->getTransparency(material);
		if ( t >= 0.f )
		{
			// <transparency>  <float>1.000000</float> </transparency> 
			Writer->writeElement(L"transparency", false);
			Writer->writeLineBreak();
			writeFloatElement(t);
			Writer->writeClosingTag(L"transparency");
			Writer->writeLineBreak();
		}
	}

	if ( writeIndexOfRefraction )
	{
		// TODO index_of_refraction>
	}
	

	Writer->writeClosingTag(fxLabel.c_str());
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeFloatElement(irr::f32 value)
{
	Writer->writeElement(L"float", false);
	Writer->writeText(core::stringw(value).c_str());
	Writer->writeClosingTag(L"float");
	Writer->writeLineBreak();
}

} // end namespace
} // end namespace

#endif
