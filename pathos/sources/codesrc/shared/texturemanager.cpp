/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.

===============================================
*/

#include "includes.h"
#include "common.h"
#include "texturemanager.h"
#include "r_glextf.h"

#include "tga.h"
#include "dds.h"
#include "bmp.h"

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY
#define GL_MAX_TEXTURE_MAX_ANISOTROPY 0x84FF
#endif //GL_MAX_TEXTURE_MAX_ANISOTROPY

#ifndef GL_TEXTURE_MAX_ANISOTROPY
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE
#endif //GL_TEXTURE_MAX_ANISOTROPY

// Default value for specular scale
const Float CTextureManager::DEFAULT_SPECFACTOR = 2.0f;
// Default phong exponent value
const Float CTextureManager::DEFAULT_PHONG_EXP = 16.0f;
// Anisotropy off value
const Uint32 CTextureManager::ANISOTROPY_OFF_VALUE = 1;

// Texture manager instance
CTextureManager* CTextureManager::g_pInstance = nullptr;

//=============================================
// @brief Constructor
//
//=============================================
CTextureManager::CTextureManager( const file_interface_t& fileFuncs, pfnPrintf_t printFunction, pfnPrintf_t printErrorFunction, const CGLExtF& glExtF, bool onlyMaterials  ):
	m_pDummyTexture(nullptr),
	m_pDummyMaterial(nullptr),
	m_currentAnisotropySetting(0),
	m_currentAnisotropyValue(ANISOTROPY_OFF_VALUE),
	m_fileFuncs(fileFuncs),
	m_printFunction(printFunction),
	m_printErrorFunction(printErrorFunction),
	m_glExtF(glExtF)
{
}

//=============================================
// @brief Destructor
//
//=============================================
CTextureManager::~CTextureManager( void )
{
	Shutdown();
}

//=============================================
// @brief Initializes the class
//
//=============================================
void CTextureManager::Init( void )
{
	// Create the dummy texture
	CreateDummyTexture();

	// Reload any textures into OpenGL
	ReloadResources();

	// Set this first for the cvar
	PopulateAnisotropyList();
}

//=============================================
// @brief Reloads any textures that need loading
//
//=============================================
void CTextureManager::ReloadResources( void )
{
	// Reload all textures
	if(!m_texturesList.empty())
	{
		m_texturesList.begin();
		while(!m_texturesList.end())
		{
			en_texture_t* ptexture = m_texturesList.get();
			if(ptexture->needsload && ptexture->format != TX_FORMAT_MEMORY)
			{
				// Load from the disc again
				LoadTexture(ptexture->filepath.c_str(), ptexture->level, ptexture->flags, (ptexture->flags & TX_FL_BORDER) ? ptexture->bordercolor : nullptr);
			}
			else if(ptexture->format == TX_FORMAT_MEMORY)
			{
				// Find any material files referring to this texture
				m_materialsList.begin();
				while(!m_materialsList.end())
				{
					en_material_t* pmaterial = m_materialsList.get();
					for(Uint32 i = 0; i < NB_MT_TX; i++)
					{
						if(pmaterial->ptextures[i] == ptexture)
							pmaterial->ptextures[i] = nullptr;
					}

					m_materialsList.next();
				}

				// Remove textures that were loaded from memory
				m_texturesList.remove(m_texturesList.get_link());
				delete ptexture;
			}

			m_texturesList.next();
		}
	}
}

//=============================================
// @brief Frees all resources
//
//=============================================
void CTextureManager::Shutdown( void )
{
	// Delete all allocations
	if(!m_allocsList.empty())
	{
		m_allocsList.begin();
		while(!m_allocsList.end())
		{
			en_texalloc_t* palloc = m_allocsList.get();
			glDeleteTextures(1, &palloc->gl_index);
			delete palloc;

			m_allocsList.next();
		}
		m_allocsList.clear();
	}

	// Delete all materials
	if(!m_materialsList.empty())
	{
		m_materialsList.begin();
		while(!m_materialsList.end())
		{
			delete m_materialsList.get();
			m_materialsList.next();
		}
		m_materialsList.clear();
	}

	// Delete aliases
	if(!m_aliasMappingsList.empty())
		m_aliasMappingsList.clear();

	// Delete all textures
	if(!m_texturesList.empty())
	{
		m_texturesList.begin();
		while(!m_texturesList.end())
		{
			delete m_texturesList.get();
			m_texturesList.next();
		}
		m_texturesList.clear();
	}
}

//=============================================
// @brief Checks anisotropy cvar setting
//
//=============================================
void CTextureManager::UpdateAnisotropySettings( Float cvarValue )
{
	// Update anisotropy if needed
	Int32 anisotropySetting = cvarValue;
	if(anisotropySetting == m_currentAnisotropySetting)
		return;

	// Make sure it's valid
	if(anisotropySetting < 0 || anisotropySetting >= static_cast<Int32>(m_anisotropySettingsArray.size()))
		anisotropySetting = 0;

	m_currentAnisotropySetting = anisotropySetting;
	m_currentAnisotropyValue = m_anisotropySettingsArray[anisotropySetting];
	if(!m_currentAnisotropyValue)
		m_currentAnisotropyValue = ANISOTROPY_OFF_VALUE;

	if(!m_texturesList.empty())
	{
		// Set value for each texture
		m_texturesList.begin();
		while(!m_texturesList.end())
		{
			en_texture_t* ptexture = m_texturesList.get();
			if(ptexture->palloc && !(ptexture->flags & (TX_FL_NOMIPMAPS|TX_FL_RECTANGLE)))
			{
				glBindTexture(GL_TEXTURE_2D, ptexture->palloc->gl_index);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, m_currentAnisotropyValue);
			}

			m_texturesList.next();
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//=============================================
// @brief Generates an OpenGL texture object
//
// @param level Resource level for the bind
// @return Allocation data structure
//=============================================
en_texalloc_t* CTextureManager::GenTextureIndex( rs_level_t level )
{
	en_texalloc_t* pnew = new en_texalloc_t();
	pnew->level = level;

	glGenTextures(1, &pnew->gl_index);

	// Add it to the list
	m_allocsList.add(pnew);
	return pnew;
}

//=============================================
// @brief Generates a texture object
//
// @return Allocation texture object
//=============================================
en_texture_t* CTextureManager::AllocTexture( void )
{
	en_texture_t* ptexture = new en_texture_t();
	m_texturesList.add(ptexture);

	return ptexture;
}

//=============================================
// @brief Deletes all texture binds on a resource level
//
// @param level Resource level to delete in OpenGL
// @param bdelete Determines if the entry should be deleted too
//=============================================
void CTextureManager::DeleteBinds( rs_level_t level )
{
	// Delete all objects on this level
	if(!m_allocsList.empty())
	{
		m_allocsList.begin();
		while(!m_allocsList.end())
		{
			AllocList_t::link_t* plink = m_allocsList.get_link();
			en_texalloc_t* palloc = plink->_val;

			if(palloc->level == level)
			{
				glDeleteTextures(1, &palloc->gl_index);

				delete palloc;
				m_allocsList.remove(plink);
			}

			m_allocsList.next();
		}
	}
}

//=============================================
// @brief Deletes all materials on a resource level
//
// @param level Resource level to delete in OpenGL
//=============================================
void CTextureManager::DeleteMaterials( rs_level_t level )
{
	// Delete all textures
	if(!m_materialsList.empty())
	{
		m_materialsList.begin();
		while(!m_materialsList.end())
		{
			MaterialsList_t::link_t* plink = m_materialsList.get_link();
			en_material_t* pmaterial = plink->_val;

			if(pmaterial->level == level)
			{
				delete plink->_val;
				m_materialsList.remove(plink);
			}

			m_materialsList.next();
		}
	}

	// Delete from alias mappings as well
	if(!m_aliasMappingsList.empty())
	{
		m_aliasMappingsList.begin();
		while(!m_aliasMappingsList.end())
		{
			const alias_mapping_t& mapping = m_aliasMappingsList.get();
			if(level == mapping.level)
				m_aliasMappingsList.remove(m_aliasMappingsList.get_link());

			m_aliasMappingsList.next();
		}
	}

	// Reset index array
	ResetMaterialsIndexArray();
}

//=============================================
// @brief Deletes all texture binds for texture objects on a resource level
//
// @param level Resource level to delete in OpenGL
// @param keepentry Determines if the entry should be deleted too, or just marked as needing data load
//=============================================
void CTextureManager::DeleteTextures( rs_level_t level, bool keepentry )
{
	// Delete all textures
	if(!m_texturesList.empty())
	{
		m_texturesList.begin();
		while(!m_texturesList.end())
		{
			TexturesList_t::link_t* plink = m_texturesList.get_link();
			en_texture_t* ptexture = plink->_val;

			if(ptexture->palloc && ptexture->level == level)
			{
				m_allocsList.remove(ptexture->palloc);
				glDeleteTextures(1, &ptexture->palloc->gl_index);
				delete ptexture->palloc;

				ptexture->needsload = true;
				ptexture->palloc = nullptr;
			}

			if(!keepentry && ptexture->level == level)
			{
				delete plink->_val;
				m_texturesList.remove(plink);
			}

			m_texturesList.next();
		}
	}

	// Delete any non-texture file binds
	DeleteBinds(level);
}

//=============================================
// @brief Deletes a texture
//
// @param pstrFilename Filename of the texture to delete
//=============================================
void CTextureManager::DeleteTexture( const Char *pstrFilename )
{
	// Delete the texture
	if(m_texturesList.empty())
		return;

	m_texturesList.begin();
	while(!m_texturesList.end())
	{
		TexturesList_t::link_t* plink = m_texturesList.get_link();
		en_texture_t* ptexture = plink->_val;

		if(!qstrcmp(pstrFilename, ptexture->filepath))
		{
			if(ptexture->palloc)
			{
				m_allocsList.remove(ptexture->palloc);
				glDeleteTextures(1, &ptexture->palloc->gl_index);
				delete ptexture->palloc;
			}

			delete plink->_val;
			m_texturesList.remove(plink);
			break;
		}

		m_texturesList.next();
	}
}

//=============================================
// @brief Deletes a texture
//
// @param pstrFilename Filename of the texture to delete
//=============================================
void CTextureManager::DeleteTexture( en_texture_t* ptexture )
{
	// Delete the texture
	if(m_texturesList.empty())
		return;

	if(ptexture->palloc)
	{
		m_allocsList.remove(ptexture->palloc);
		glDeleteTextures(1, &ptexture->palloc->gl_index);
		delete ptexture->palloc;
	}	

	m_texturesList.remove(ptexture);
	delete ptexture;
}

//=============================================
// @brief Creates the dummy for missing textures
//
//=============================================
void CTextureManager::DeleteAllocation( en_texalloc_t* palloc )
{
	if(m_allocsList.empty())
		return;

	// Do not allow removal on allocations tied to textures
	m_texturesList.begin();
	while(!m_texturesList.end())
	{
		en_texture_t* ptexture = m_texturesList.get();
		if(ptexture && ptexture->palloc == palloc)
		{
			m_printFunction("%s - Allocation is tied to a texture, not deleted.\n", __FUNCTION__);
			return;
		}

		m_texturesList.next();
	}

	m_allocsList.remove(palloc);

	glDeleteTextures(1, &palloc->gl_index);
	delete palloc;
}

//=============================================
// @brief Creates the dummy for missing textures
//
//=============================================
void CTextureManager::CreateDummyTexture( void )
{
	constexpr Uint32 dummyTextureSize = 16;
	Uint32 dataSize = dummyTextureSize*dummyTextureSize*4;

	byte *pdata = new byte[dataSize];
	memset(pdata, 0, sizeof(byte)*dataSize);

	// This is based after the Quake code
	byte* pdest = pdata;
	for (Uint32 y = 0; y < dummyTextureSize; y++)
	{
		for (Uint32 x = 0; x < dummyTextureSize; x++)
		{
			if ((y < 8) ^ (x < 8))
			{
				*pdest++ = 0;
				*pdest++ = 0;
				*pdest++ = 0;
			}
			else
			{
				*pdest++ = 255;
				*pdest++ = 0;
				*pdest++ = 255;
			}

			*pdest++ = 255;
		}
	}

	if(!m_pDummyTexture)
	{
		m_pDummyTexture = AllocTexture();

		m_pDummyTexture->filepath = "dummy";
		m_pDummyTexture->bpp = 4;
		m_pDummyTexture->width = dummyTextureSize;
		m_pDummyTexture->height = dummyTextureSize;
		m_pDummyTexture->level = RS_WINDOW_LEVEL;
	}

	m_pDummyTexture->palloc = GenTextureIndex(m_pDummyTexture->level);
	m_pDummyTexture->needsload = false;

	// Bind it in OpenGL
	glBindTexture(GL_TEXTURE_2D, m_pDummyTexture->palloc->gl_index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_pDummyTexture->width, m_pDummyTexture->height, FALSE, GL_RGBA, GL_UNSIGNED_BYTE, pdata);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pdata;

	// Create the dummy material object
	en_material_t* pdummymaterial = new en_material_t();

	pdummymaterial->alpha = 1.0;
	pdummymaterial->dt_scalex = 0;
	pdummymaterial->dt_scaley = 0;
	pdummymaterial->filepath = "dummy";
	pdummymaterial->int_height = 16;
	pdummymaterial->int_width = 16;
	pdummymaterial->level = RS_WINDOW_LEVEL;
	pdummymaterial->index = m_materialsList.size();

	pdummymaterial->ptextures[MT_TX_DIFFUSE] = m_pDummyTexture;

	m_pDummyMaterial = pdummymaterial;
	m_materialsList.radd(pdummymaterial);

	// Add it to the index list
	m_materialsIndexPtrArray.push_back(pdummymaterial);
}

//=============================================
// @brief Returns the format for a filename extension
//
// @return Format identifier
//=============================================
texture_format_t CTextureManager::GetFormat( const Char* pstrFilename )
{
	if(!qstrcicmp(pstrFilename + qstrlen(pstrFilename) - 3, "tga"))
		return TX_FORMAT_TGA;
	else if(!qstrcicmp(pstrFilename + qstrlen(pstrFilename) - 3, "dds"))
		return TX_FORMAT_DDS;
	else if (!qstrcicmp(pstrFilename + qstrlen(pstrFilename) - 3, "bmp"))
		return TX_FORMAT_BMP;
	else
		return TX_FORMAT_UNDEFINED;
}

//=============================================
// @brief Returns the format for a filename extension
//
// @return Texture type identifier
//=============================================
mt_texture_t CTextureManager::GetTextureType( const Char* pstrTypename )
{
	if(!qstrcmp(pstrTypename, "diffuse"))
		return MT_TX_DIFFUSE;
	else if(!qstrcmp(pstrTypename, "normalmap") || !qstrcmp(pstrTypename, "normal"))
		return MT_TX_NORMALMAP;
	else if(!qstrcmp(pstrTypename, "detail"))
		return MT_TX_DETAIL;
	else if(!qstrcmp(pstrTypename, "specular"))
		return MT_TX_SPECULAR;
	else if(!qstrcmp(pstrTypename, "luminance"))
		return MT_TX_LUMINANCE;
	else if (!qstrcmp(pstrTypename, "ao"))
		return MT_TX_AO;
	else
		return MT_TX_UNKNOWN;
}

//=============================================
// @brief Loads a texture into OpenGL
//
// @param pstrFilename Path to the file to load
// @param level Resource level to load onto
// @return Pointer to material object
//=============================================
en_material_t* CTextureManager::LoadMaterialScript( const Char* pstrFilename, rs_level_t level, bool prompt, bool isloadingfromalias )
{
	// See if we already have it loaded
	en_material_t* pmaterial = FindMaterialScript(pstrFilename, level);
	if(pmaterial)
		return pmaterial;

	// True if this is an alias script
	static Char aliasscriptpath[MAX_PARSE_LENGTH];
	bool isaliasscript = false;

	// Base directory is fixed
	CString filePath;
	if(qstrstr(pstrFilename, ":") == nullptr)
		filePath << TEXTURE_BASE_DIRECTORY_PATH << pstrFilename;
	else
		filePath = pstrFilename;

	// Load the file in
	const Char* pfile = reinterpret_cast<const Char*>(m_fileFuncs.pfnLoadFile(filePath.c_str(), nullptr));
	if(!pfile)
	{
		if(prompt)
			m_printErrorFunction("Failed to load material script '%s'.\n", filePath.c_str());

		return nullptr;
	}

	// Make sure the syntax is valid
	static Char token[MAX_PARSE_LENGTH];
	static Char value[MAX_PARSE_LENGTH];

	const Char* pchar = Common::Parse(pfile, token);
	if(!pchar)
	{
		m_printErrorFunction("Unexpected EOF in '%s'.\n", filePath.c_str());
		m_fileFuncs.pfnFreeFile(pfile);
		return nullptr;
	}

	// Make sure the name is present
	if(!qstrcmp(token, "$alias"))
	{
		// Alias script
		isaliasscript = true;
	}
	else if(qstrcmp(token, "$texture"))
	{
		m_printErrorFunction("Expected $texture or $alias token, got '%s' instead in '%s'.\n", token, filePath.c_str());
		m_fileFuncs.pfnFreeFile(pfile);
		return nullptr;
	}

	// Prevent infinite recursion
	if(isloadingfromalias && isaliasscript)
	{
		m_printErrorFunction("Could not load '%s', because it is an alias script linked to by another alias script.\n", pstrFilename);
		m_fileFuncs.pfnFreeFile(pfile);
		return nullptr;
	}

	// Seek out the opening bracket
	pchar = Common::Parse(pchar, token);
	if(!pchar)
	{
		m_printErrorFunction("Unexpected EOF in '%s'.\n", filePath.c_str());
		m_fileFuncs.pfnFreeFile(pfile);
		return nullptr;
	}

	// Make sure the bracket is present
	if(qstrcmp(token, "{"))
	{
		m_printErrorFunction("Expected { token, got '%s' instead in '%s'.\n", token, filePath.c_str());
		m_fileFuncs.pfnFreeFile(pfile);
		return nullptr;
	}

	// Only allocate if not an alias script
	if(!isaliasscript)
	{
		// Allocate a new material object
		pmaterial = new en_material_t();

		// Set basic info
		pmaterial->filepath = pstrFilename;
		pmaterial->level = level;
		pmaterial->index = m_materialsList.size();

		// Set defaults
		pmaterial->alpha = 1.0;
		pmaterial->spec_factor = DEFAULT_SPECFACTOR;
		pmaterial->phong_exp = DEFAULT_PHONG_EXP;
		pmaterial->cubemapnormal = 0.15;
	}

	static Char line[MAX_LINE_LENGTH];

	// Because we load textures after reading the script in
	Char texturePaths[NB_MT_TX][MAX_PARSE_LENGTH] = { 0 };
	for(Uint32 i = 0; i < NB_MT_TX; i++)
		texturePaths[i][0] = '\0';

	// Parse the file line by line
	const Char* pstr = pchar;
	while(true)
	{
		if(!pstr)
		{
			m_printErrorFunction("Unexpected EOF in '%s', missing } token at end.\n", filePath.c_str());
			if(pmaterial) delete pmaterial;
			m_fileFuncs.pfnFreeFile(pfile);
			break;
		}

		// Read the line in
		pstr = Common::ReadLine(pstr, line);
		if(!qstrlen(line))
			continue;

		// Skip comments
		if(!qstrncmp(line, "//", 2))
			continue;

		// Parse fields
		pchar = Common::Parse(line, token);
		if(!qstrcmp(token, "}"))
			break;

		if(isaliasscript)
		{
			// Make sure the token is valid
			if(qstrcmp(token, "$scriptfile"))
			{
				m_printErrorFunction("Expected $scriptfile token, got '%s' instead in '%s'.\n", token, filePath.c_str());
				if(pmaterial) delete pmaterial;
				m_fileFuncs.pfnFreeFile(pfile);
				return nullptr;
			}

			if(!pchar)
			{
				m_printErrorFunction("Parameter specification for '%s' is incomplete in '%s'.\n", token, filePath.c_str());
				continue;
			}

			// Read in the value token
			pchar = Common::Parse(pchar, aliasscriptpath);
		}
		else
		{
			// Identify the field
			if(!qstrcmp(token, "$cubemaps"))
				pmaterial->flags |= TX_FL_CUBEMAPS;
			else if(!qstrcmp(token, "$fullbright"))
				pmaterial->flags |= TX_FL_FULLBRIGHT;
			else if(!qstrcmp(token, "$nodecal"))
				pmaterial->flags |= TX_FL_NODECAL;
			else if(!qstrcmp(token, "$alphatest"))
				pmaterial->flags |= TX_FL_ALPHATEST;
			else if(!qstrcmp(token, "$nomipmaps"))
				pmaterial->flags |= TX_FL_NOMIPMAPS;
			else if(!qstrcmp(token, "$clamp"))
				pmaterial->flags |= (TX_FL_CLAMP_S | TX_FL_CLAMP_T);
			else if(!qstrcmp(token, "$eyeglint"))
				pmaterial->flags |= TX_FL_EYEGLINT;
			else if(!qstrcmp(token, "$chrome"))
				pmaterial->flags |= TX_FL_CHROME;
			else if(!qstrcmp(token, "$additive"))
				pmaterial->flags |= TX_FL_ADDITIVE;
			else if(!qstrcmp(token, "$alphablend"))
				pmaterial->flags |= TX_FL_ALPHABLEND;
			else if(!qstrcmp(token, "$scope"))
				pmaterial->flags |= TX_FL_SCOPE;
			else if(!qstrcmp(token, "$noimpactfx"))
				pmaterial->flags |= TX_FL_NO_IMPACT_EFFECTS;
			else if(!qstrcmp(token, "$nostepsound"))
				pmaterial->flags |= TX_FL_NO_STEPSOUND;
			else if(!qstrcmp(token, "$nofacecull"))
				pmaterial->flags |= TX_FL_NO_CULLING;
			else if(!qstrcmp(token, "$nopenetration"))
				pmaterial->flags |= TX_FL_NO_PENETRATION;
			else if(!qstrcmp(token, "$bulletproof"))
				pmaterial->flags |= TX_FL_BULLETPROOF;
			else if(!qstrcmp(token, "$dt_scalex") || !qstrcmp(token, "$dt_scaley")
					|| !qstrcmp(token, "$int_width") || !qstrcmp(token, "$int_height")
					|| !qstrcmp(token, "$alpha") || !qstrcmp(token, "$phong_exp")
				    || !qstrcmp(token, "$alpha") || !qstrcmp(token, "$cubemapnormal")
					|| !qstrcmp(token, "$spec") || !qstrcmp(token, "$scopescale") 
					|| !qstrcmp(token, "$cubemapstrength") || !qstrcmp(token, "$container")
					|| !qstrcmp(token, "$scrollu") || !qstrcmp(token, "$scrollv"))
			{
				if(!pchar)
				{
					m_printErrorFunction("Parameter specification for '%s' is incomplete in '%s'.\n", token, filePath.c_str());
					continue;
				}

				// Read in the value token
				pchar = Common::Parse(pchar, value);

				// Make sure it's a valid value
				if(qstrcmp(token, "$container") && !Common::IsNumber(value))
				{
					m_printErrorFunction("Invalid value '%s' for field '%s' in '%s'.\n", token, value, filePath.c_str());
					continue;
				}

				if(!qstrcmp(token, "$dt_scalex"))
					pmaterial->dt_scalex = SDL_atof(value);
				else if(!qstrcmp(token, "$dt_scaley"))
					pmaterial->dt_scaley = SDL_atof(value);
				else if(!qstrcmp(token, "$int_width"))
					pmaterial->int_width = SDL_atoi(value);
				else if(!qstrcmp(token, "$int_height"))
					pmaterial->int_height = SDL_atoi(value);
				else if(!qstrcmp(token, "$alpha"))
					pmaterial->alpha = static_cast<Float>(SDL_atof(value));
				else if(!qstrcmp(token, "$phong_exp"))
					pmaterial->phong_exp = static_cast<Float>(SDL_atof(value));
				else if (!qstrcmp(token, "$cubemapnormal"))
					pmaterial->cubemapnormal = (Float)SDL_atof(value);
				else if(!qstrcmp(token, "$spec"))
					pmaterial->spec_factor = static_cast<Float>(SDL_atof(value));
				else if(!qstrcmp(token, "$scopescale"))
					pmaterial->scale = static_cast<Float>(SDL_atof(value));
				else if(!qstrcmp(token, "$cubemapstrength"))
					pmaterial->cubemapstrength = static_cast<Float>(SDL_atof(value));
				else if(!qstrcmp(token, "$container"))
					pmaterial->containername = value;
				else if(!qstrcmp(token, "$scrollu"))
					pmaterial->scrollu = static_cast<Float>(SDL_atof(value));
				else if(!qstrcmp(token, "$scrollv"))
					pmaterial->scrollv = static_cast<Float>(SDL_atof(value));
			}
			else if(!qstrcmp(token, "$texture"))
			{
				if(!pchar)
				{
					m_printErrorFunction("$texture command is incomplete in '%s'.\n", filePath.c_str());
					continue;
				}

				// Read in the type token
				pchar = Common::Parse(pchar, token);
				if(!pchar)
				{
					m_printErrorFunction("$texture command is incomplete in '%s'.\n", filePath.c_str());
					continue;
				}

				// Get texture type
				mt_texture_t typeIndex = GetTextureType(token);
				if(typeIndex == MT_TX_UNKNOWN)
				{
					m_printErrorFunction("Unknown $texture typename '%s' in '%s'.\n", token, filePath.c_str());
					continue;
				}

				// Parse in the file path token
				pchar = Common::Parse(pchar, texturePaths[typeIndex]);
			}
			else if(!qstrcmp(token, "$material"))
			{
				if(!pchar)
				{
					m_printErrorFunction("$material command is incomplete in '%s'.\n", filePath.c_str());
					continue;
				}

				// Read in the type token
				pchar = Common::Parse(pchar, token);
				pmaterial->materialname = token;
			}
			else if(!qstrcmp(token, "$fullbrightcolor"))
			{
				Uint32 i = 0;
				for(; i < 3; i++)
				{
					if(!pchar)
					{
						m_printErrorFunction("Parameter specification for '%s' is incomplete in '%s'.\n", token, filePath.c_str());
						break;
					}

					// Read in the value token
					pchar = Common::Parse(pchar, value);

					// Make sure it's a valid value
					if(!Common::IsNumber(value))
					{
						m_printErrorFunction("Invalid value '%s' for field '%s' in '%s'.\n", token, value, filePath.c_str());
						break;
					}

					pmaterial->fullbrightcolor[i] = SDL_atof(value);
				}

				if(i != 3)
					break;
			}
			else if(qstrlen(token) > 0)
			{
				m_printErrorFunction("Unknown field '%s' in '%s'.\n", token, filePath.c_str());
			}
		}
	}

	m_fileFuncs.pfnFreeFile(pfile);

	if(isaliasscript)
	{
		// Load alias script, but prevent infinite recursion
		pmaterial = LoadMaterialScript(aliasscriptpath, level, prompt, true);
		if(!pmaterial)
			return nullptr;

		// Add to mappings
		alias_mapping_t mapping;
		mapping.filename = pstrFilename;
		mapping.pmaterialfile = pmaterial;
		mapping.level = level;

		m_aliasMappingsList.radd(mapping);
		return pmaterial;
	}

	// Print about invalid detail texture scales
	assert(pmaterial != nullptr);
	if(pmaterial->dt_scalex && !pmaterial->dt_scaley)
		m_printFunction("%s - Invalid y detail texture scale for material script file '%s'.\n", __FUNCTION__, filePath.c_str());
	else if(!pmaterial->dt_scalex && pmaterial->dt_scaley)
		m_printFunction("%s - Invalid x detail texture scale for material script file '%s'.\n", __FUNCTION__, filePath.c_str());

	// Load in the individual textures
	for(Uint32 i = 0; i < NB_MT_TX; i++)
	{
		if(!qstrlen(texturePaths[i]))
			continue;

		if(!pmaterial->containername.empty() && i == MT_TX_DIFFUSE)
		{
			pmaterial->containertexturename = texturePaths[i];
			continue;
		}

		en_texture_t* ptexture = LoadTexture(texturePaths[i], level, pmaterial->flags);
		if(!ptexture)
			continue;

		pmaterial->ptextures[i] = ptexture;
	}

	if(!pmaterial->ptextures[MT_TX_DIFFUSE] && pmaterial->containername.empty())
		pmaterial->ptextures[MT_TX_DIFFUSE] = GetDummyTexture();

	if(pmaterial->flags & TX_FL_CUBEMAPS && pmaterial->cubemapstrength <= 0)
		pmaterial->cubemapstrength = 0.1;

	// Add it to the index list
	m_materialsIndexPtrArray.push_back(pmaterial);
	// Add this to the list
	m_materialsList.radd(pmaterial);

	return pmaterial;
}

//=============================================
// @brief Loads a texture into OpenGL
//
// @param pstrFilename Path to the file to load
// @param level Resource level to load onto
// @param clamp Tells if the texture should be clamped
// @param mipmaps Tells if we should generate mipmaps
// @param pborder Border to set
// @return Pointer to texture object
//=============================================
en_texture_t* CTextureManager::LoadTexture( const Char* pstrFilename, rs_level_t level, Int32 flags, const GLint* pborder )
{
	// Don't allow invalid resource levels
	if(level != RS_GAME_LEVEL && level != RS_WINDOW_LEVEL)
	{
		m_printErrorFunction("Invalid resource level for texture '%s'.\n", pstrFilename);
		return nullptr;
	}

	// Do not allow for rectangle textures to be loaded from file
	if(flags & TX_FL_RECTANGLE)
	{
		m_printErrorFunction("Could not load %s - Rectangle textures are only supported for textures loaded from memory.\n");
		return nullptr;
	}

	// See if it's already loaded
	en_texture_t* ptexture = FindTexture(pstrFilename, level);
	if(ptexture && !ptexture->needsload)
		return ptexture;

	// To hold the values
	byte *pdata = nullptr;
	Uint32 width = 0;
	Uint32 height = 0;
	Uint32 bpp = 0;
	Uint32 datasize = 0;
	texture_compression_t compression = TX_COMPRESSION_NONE;

	// Base directory is fixed
	CString filePath;
	if(qstrstr(pstrFilename, ":") == nullptr)
		filePath << TEXTURE_BASE_DIRECTORY_PATH << pstrFilename;
	else
		filePath = pstrFilename;

	// Load the file
	const byte* pfile = m_fileFuncs.pfnLoadFile(filePath.c_str(), nullptr);
	if(!pfile)
	{
		// If it's a DDS, try looking for TGA
		if(filePath.find(0, ".dds") != -1 || filePath.find(0, ".DDS") != -1)
		{
			filePath.erase(filePath.length()-3, 3);
			filePath << "tga";

			pfile = m_fileFuncs.pfnLoadFile(filePath.c_str(), nullptr);
		}

		if (filePath.find(0, ".bmp") != -1 || filePath.find(0, ".BMP") != -1)
		{
			filePath.erase(filePath.length() - 3, 3);
			filePath << "bmp";

			pfile = m_fileFuncs.pfnLoadFile(filePath.c_str(), nullptr);
		}

		if(!pfile)
		{
			m_printErrorFunction("Failed to load texture '%s'.\n", filePath.c_str());
			return nullptr;
		}
	}

	// Determine the format
	texture_format_t format = GetFormat(filePath.c_str());
	if(format == TX_FORMAT_UNDEFINED)
	{
		m_printErrorFunction("Unknown or unsupported file format for '%s'\n", pstrFilename);
		return nullptr;
	}

	if(format == TX_FORMAT_TGA)
	{
		if(!TGA_Load(pstrFilename, pfile, pdata, width, height, bpp, datasize, compression, m_printErrorFunction))
		{
			m_printErrorFunction("Failed to load TGA image file '%s'.\n", pstrFilename);
			m_fileFuncs.pfnFreeFile(pfile);
			return nullptr;
		}
	}
	else if(format == TX_FORMAT_DDS)
	{
		if(!DDS_Load(pstrFilename, pfile, pdata, width, height, bpp, datasize, compression, m_printErrorFunction))
		{
			m_printErrorFunction("Failed to load DDS image file '%s'.\n", pstrFilename);
			m_fileFuncs.pfnFreeFile(pfile);
			return nullptr;
		}
	}
	else if (format == TX_FORMAT_BMP) {
		if (!BMP_Load(pstrFilename, pfile, pdata, width, height, bpp, datasize, compression, m_printErrorFunction)) {
			m_printErrorFunction("Failed to load BMP image file '%s'.\n", pstrFilename);
			m_fileFuncs.pfnFreeFile(pfile);
			return nullptr;
		}
	}
	else
	{
		// Shouldn't happen
		m_printErrorFunction("Unsupported file format %d for '%s'.\n", format, pstrFilename);
		m_fileFuncs.pfnFreeFile(pfile);
		return nullptr;
	}

	// Release the file
	m_fileFuncs.pfnFreeFile(pfile);

	// Allocate a new texture if it's not already present
	if(!ptexture)
		ptexture = AllocTexture();

	ptexture->width = width;
	ptexture->height = height;
	ptexture->bpp = bpp;
	ptexture->filepath = pstrFilename;
	ptexture->level = level;
	ptexture->flags = flags;
	ptexture->format = format;
	ptexture->needsload = false;

	// Set border if any
	if(pborder)
	{
		for(Uint32 i = 0; i < 4; i++)
			ptexture->bordercolor[i] = pborder[i];

		ptexture->flags |= TX_FL_BORDER;
	}

	// Allocate a GL index for it
	ptexture->palloc = GenTextureIndex(ptexture->level);

	// Bind it in OpenGL
	glBindTexture(GL_TEXTURE_2D, ptexture->palloc->gl_index);

	if(format == TX_FORMAT_DDS)
		m_glExtF.glCompressedTexImage2D(GL_TEXTURE_2D, 0, 
		(compression == TX_COMPRESSION_DXT1) ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 
		ptexture->width, ptexture->height, 0, datasize, pdata);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ptexture->width, ptexture->height, FALSE, GL_RGBA, GL_UNSIGNED_BYTE, pdata);
	
	if(ptexture->flags & TX_FL_BORDER)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	else if(ptexture->flags & TX_FL_CLAMP_S)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	if(ptexture->flags & TX_FL_BORDER)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	else if(ptexture->flags & TX_FL_CLAMP_T)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(!(ptexture->flags & TX_FL_NOMIPMAPS))
	{
		m_glExtF.glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(m_currentAnisotropyValue != 0)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, m_currentAnisotropyValue);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if(ptexture->flags & TX_FL_BORDER)
	{
		GLfloat values[4] = { static_cast<GLfloat>(pborder[0])/255.0f,
			static_cast<GLfloat>(pborder[1])/255.0f,
			static_cast<GLfloat>(pborder[2])/255.0f,
			static_cast<GLfloat>(pborder[3])/255.0f };

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, values);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pdata;
	return ptexture;
}

//=============================================
// @brief Loads a texture into OpenGL
//
// @param pstrFilename Path to the file to load
// @param level Resource level to load onto
// @param clamp Tells if the texture should be clamped
// @param mipmaps Tells if we should generate mipmaps
// @param pborder Border to set(only for TGA)
// @return Pointer to texture object
//=============================================
en_texture_t* CTextureManager::LoadFromMemory( const Char* pstrTextureName, rs_level_t level, Int32 flags, const byte* pdata, Uint32 width, Uint32 height, Uint32 bpp, Uint32 datasize )
{
	if(bpp != 4 && bpp != 3 || !width || !height || !pdata)
	{
		m_printErrorFunction("Invalid texture properties given for '%s'.\n", pstrTextureName);
		return nullptr;
	}

	// Don't allow invalid resource levels
	if(level != RS_GAME_LEVEL && level != RS_WINDOW_LEVEL)
	{
		m_printErrorFunction("Invalid resource level for texture '%s'.\n", pstrTextureName);
		return nullptr;
	}

	// See if it's already loaded
	en_texture_t* ptexture = FindTexture(pstrTextureName, level);
	if(ptexture && !ptexture->needsload)
		return ptexture;

	// Allocate a new texture if it's not already present
	if(!ptexture)
		ptexture = AllocTexture();

	ptexture->width = width;
	ptexture->height = height;
	ptexture->bpp = bpp;
	ptexture->filepath = pstrTextureName;
	ptexture->level = level;
	ptexture->flags = flags;
	ptexture->format = TX_FORMAT_MEMORY;
	ptexture->needsload = false;

	if(flags & TX_FL_DXT1)
		ptexture->compression = TX_COMPRESSION_DXT1;
	else if(flags & TX_FL_DXT5)
		ptexture->compression = TX_COMPRESSION_DXT5;
	else
		ptexture->compression = TX_COMPRESSION_NONE;

	// Allocate a GL index for it
	ptexture->palloc = GenTextureIndex(ptexture->level);

	// Support rectangle textures for in-memory textures
	GLenum target = (flags & TX_FL_RECTANGLE) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;

	// Bind it in OpenGL
	glBindTexture(target, ptexture->palloc->gl_index);
	if(flags & (TX_FL_DXT1|TX_FL_DXT5))
		m_glExtF.glCompressedTexImage2D(GL_TEXTURE_2D, 0, 
		(ptexture->compression == TX_COMPRESSION_DXT1) ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 
		ptexture->width, ptexture->height, 0, datasize, pdata);
	else
		glTexImage2D(target, 0, (bpp == 4) ? GL_RGBA : GL_RGB, ptexture->width, ptexture->height, FALSE, (bpp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pdata);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(!(flags & TX_FL_RECTANGLE))
	{
		if(ptexture->flags & TX_FL_CLAMP_S)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		if(ptexture->flags & TX_FL_CLAMP_T)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		if(!(ptexture->flags & TX_FL_NOMIPMAPS))
		{
			m_glExtF.glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if(m_currentAnisotropyValue > 0)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, m_currentAnisotropyValue);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	return ptexture;
}

//=============================================
// @brief Searches the texture list for a filename on a resource level
//
// @param pstrFilename Path to the file to load
// @param level Resource level to load onto
// @return Pointer to texture object if found
//=============================================
en_texture_t* CTextureManager::FindTexture( const Char* pstrFilename, rs_level_t level )
{
	m_texturesList.begin();
	while(!m_texturesList.end())
	{
		en_texture_t* ptexture = m_texturesList.get();
		if(ptexture->level == level && !qstrcmp(pstrFilename, ptexture->filepath))
			return ptexture;

		m_texturesList.next();
	}

	return nullptr;
}

//=============================================
// @brief Searches the materials list for a filename on a resource level
//
// @param pstrFilename Path to the file to load
// @param level Resource level to load onto
// @return Pointer to material object if found
//=============================================
en_material_t* CTextureManager::FindMaterialScript( const Char* pstrFilename, rs_level_t level )
{
	m_materialsList.begin();
	while(!m_materialsList.end())
	{
		en_material_t* pmaterial = m_materialsList.get();
		if(pmaterial->level == level && !qstrcicmp(pstrFilename, pmaterial->filepath))
			return pmaterial;

		m_materialsList.next();
	}

	// Look in alias list too
	m_aliasMappingsList.begin();
	while(!m_aliasMappingsList.end())
	{
		alias_mapping_t& mapping = m_aliasMappingsList.get();
		if(mapping.level == level && !qstrcicmp(pstrFilename, mapping.filename))
			return mapping.pmaterialfile;

		m_aliasMappingsList.next();
	}

	return nullptr;
}

//=============================================
// @brief Loads a paletted texture
//
// @param pstrFilename Filename of the texture
// @param pdata Pointer to texture data
// @param ppal Palette pointer
// @param width Width of the texture
// @param height Height of the texture
// @param flags Texture flags
// @return Pointer to texture object
//=============================================
en_texture_t* CTextureManager::LoadPallettedTexture( const Char* pstrFilename, rs_level_t level, const byte *pdata, const color24_t *ppal, Uint32 width, Uint32 height, Int32 flags )
{
	en_texture_t* ptexture = FindTexture(pstrFilename, level);
	if(ptexture)
	{
		if(ptexture->width == width && ptexture->height == height)
			return ptexture;
	}

	color24_t pix1, pix2, pix3, pix4;
	byte alpha1, alpha2, alpha3, alpha4;

	// convert texture to power of 2
	Uint32 outwidth, outheight;
	for (outwidth = 1; outwidth < width; outwidth <<= 1);
	for (outheight = 1; outheight < height; outheight <<= 1);

	// Allocate arrays
	Int32* prow1 = new Int32[outheight];
	Int32* prow2 = new Int32[outheight];
	Int32* pcol1 = new Int32[outwidth];
	Int32* pcol2 = new Int32[outwidth];

	byte* ptex = new byte[(outwidth*outheight*4*sizeof(byte))];
	byte* pout = ptex;

	for (Uint32 i = 0; i < outwidth; i++)
	{
		pcol1[i] = static_cast<Int32>((i + 0.25) * (width / static_cast<Float>(outwidth)));
		pcol2[i] = static_cast<Int32>((i + 0.75) * (width / static_cast<Float>(outwidth)));
	}

	for (Uint32 i = 0; i < outheight; i++)
	{
		prow1[i] = static_cast<Int32>((i + 0.25) * (height / static_cast<Float>(outheight))) * width;
		prow2[i] = static_cast<Int32>((i + 0.75) * (height / static_cast<Float>(outheight))) * width;
	}

	for (Uint32 i = 0; i < outheight; i++)
	{
		for (Uint32 j = 0; j < outwidth; j++, pout += 4)
		{
			pix1 = ppal[pdata[prow1[i] + pcol1[j]]];
			pix2 = ppal[pdata[prow1[i] + pcol2[j]]];
			pix3 = ppal[pdata[prow2[i] + pcol1[j]]];
			pix4 = ppal[pdata[prow2[i] + pcol2[j]]];
			alpha1 = 0xFF; alpha2 = 0xFF; alpha3 = 0xFF; alpha4 = 0xFF;

			if(flags & TX_FL_ALPHATEST)
			{
				if (pdata[prow1[i] + pcol1[j]] == 0xFF) 
				{
					pix1.r = 0; pix1.g = 0; pix1.b = 0; alpha1 = 0;							
				} 

				if (pdata[prow1[i] + pcol2[j]] == 0xFF) 
				{
					pix2.r = 0; pix2.g = 0; pix2.b = 0; alpha2 = 0;				
				} 

				if (pdata[prow2[i] + pcol1[j]] == 0xFF) 
				{
					pix3.r = 0; pix3.g = 0; pix3.b = 0; alpha3 = 0;
				} 

				if (pdata[prow2[i] + pcol2[j]] == 0xFF) 
				{
					pix4.r = 0; pix4.g = 0; pix4.b = 0; alpha4 = 0;
				}
			}

			pout[0] = (pix1.r + pix2.r + pix3.r + pix4.r)>>2;
			pout[1] = (pix1.g + pix2.g + pix3.g + pix4.g)>>2;
			pout[2] = (pix1.b + pix2.b + pix3.b + pix4.b)>>2;
			pout[3] = (alpha1 + alpha2 + alpha3 + alpha4)>>2;
		}
	}

	delete[] prow1;
	delete[] prow2;
	delete[] pcol1;
	delete[] pcol2;

	// Create entry
	ptexture = new en_texture_t();
	ptexture->bpp = 4;
	ptexture->compression = TX_COMPRESSION_NONE;
	ptexture->filepath = pstrFilename;
	ptexture->format = TX_FORMAT_MEMORY;
	ptexture->height = height;
	ptexture->width = width;
	ptexture->level = level;
	ptexture->needsload = false;
	ptexture->palloc = GenTextureIndex(level);
	ptexture->flags = flags;
	m_texturesList.add(ptexture);

	glBindTexture(GL_TEXTURE_2D, ptexture->palloc->gl_index); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outwidth, outheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptex);

	if(!(flags & TX_FL_NOMIPMAPS))
	{
		m_glExtF.glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, m_currentAnisotropyValue);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] ptex;

	return ptexture;
}

//=============================================
// @brief Resets the material index array
//
//=============================================
void CTextureManager::ResetMaterialsIndexArray( void )
{
	if(m_materialsIndexPtrArray.size())
		m_materialsIndexPtrArray.clear();
	
	// Allocate new size
	m_materialsIndexPtrArray.resize(m_materialsList.size());

	Int32 index = 0;
	m_materialsList.begin();
	while(!m_materialsList.end())
	{
		en_material_t* pmaterial = m_materialsList.get();
		m_materialsIndexPtrArray[index] = pmaterial;
		pmaterial->index = index;

		m_materialsList.next();
	}
}

//=============================================
// @brief Populates the anisotropy list
//
//=============================================
void CTextureManager::PopulateAnisotropyList( void )
{
	// Set default value
	m_anisotropySettingsArray.push_back(ANISOTROPY_OFF_VALUE);

	// Get max anisotropy
	GLint maxAnisotropy;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

	// Start from power of two
	Int32 value = 2;
	while(true)
	{
		// Add the value
		m_anisotropySettingsArray.push_back(value);

		if(value >= maxAnisotropy)
			break;

		// Raise the value
		value <<= 1;
	}
}

//=============================================
// @brief Finds a material by it's index
//
//=============================================
en_material_t* CTextureManager::FindMaterialScriptByIndex( Int32 index )
{
	if(index < 0 || index >= static_cast<Int32>(m_materialsIndexPtrArray.size()))
		return nullptr;
	
	en_material_t* pmaterial = m_materialsIndexPtrArray[index];
	return pmaterial;
}

//=============================================
// @brief Returns the anisotropy value at a setting index
//
//=============================================
Int32 CTextureManager::GetAnisotropySettingValue( Uint32 settingIndex )
{
	assert(settingIndex < m_anisotropySettingsArray.size());
	return m_anisotropySettingsArray[settingIndex];
}

//=============================================
// @brief Returns the number of anisotropy setting
//
//=============================================
Uint32 CTextureManager::GetNbAnisotropySettings( void ) const
{
	return m_anisotropySettingsArray.size();
}

//=============================================
// @brief Writes a PMF material file
//
//=============================================
void CTextureManager::WritePMFFile( en_material_t* pmaterial )
{
	CString data;
	data << "$texture" << NEWLINE;
	data << "{" << NEWLINE;

	// Handle material flags
	if(pmaterial->flags & TX_FL_CUBEMAPS)
		data << "\t$cubemaps" << NEWLINE;

	if(pmaterial->flags & TX_FL_FULLBRIGHT)
		data << "\t$fullbright" << NEWLINE;

	if(pmaterial->flags & TX_FL_NODECAL)
		data << "\t$nodecal" << NEWLINE;

	if(pmaterial->flags & TX_FL_ALPHATEST)
		data << "\t$alphatest" << NEWLINE;

	if(pmaterial->flags & TX_FL_NOMIPMAPS)
		data << "\t$nomipmaps" << NEWLINE;

	if((pmaterial->flags & TX_FL_CLAMP_S) && (pmaterial->flags & TX_FL_CLAMP_T))
		data << "\t$clamp" << NEWLINE;

	if(pmaterial->flags & TX_FL_EYEGLINT)
		data << "\t$eyeglint" << NEWLINE;

	if(pmaterial->flags & TX_FL_CHROME)
		data << "\t$chrome" << NEWLINE;

	if(pmaterial->flags & TX_FL_ADDITIVE)
		data << "\t$additive" << NEWLINE;

	if(pmaterial->flags & TX_FL_ALPHABLEND)
		data << "\t$alphablend" << NEWLINE;

	if(pmaterial->flags & TX_FL_SCOPE)
		data << "\t$scope" << NEWLINE;

	if(pmaterial->flags & TX_FL_NO_IMPACT_EFFECTS)
		data << "\t$noimpactfx" << NEWLINE;

	if(pmaterial->flags & TX_FL_NO_STEPSOUND)
		data << "\t$nostepsound" << NEWLINE;

	if(pmaterial->flags & TX_FL_NO_CULLING)
		data << "\t$nofacecull" << NEWLINE;

	// Handle settings
	if(pmaterial->ptextures[MT_TX_DETAIL])
	{
		if(pmaterial->dt_scalex)
			data << "\t$dt_scalex " << pmaterial->dt_scalex << NEWLINE;

		if(pmaterial->dt_scaley)
			data << "\t$dt_scaley " << pmaterial->dt_scaley << NEWLINE;
	}

	if(pmaterial->int_width)
		data << "\t$int_width " << pmaterial->int_width << NEWLINE;

	if(pmaterial->int_height)
		data << "\t$int_height " << pmaterial->int_height << NEWLINE;

	if(pmaterial->flags & (TX_FL_ADDITIVE|TX_FL_ALPHABLEND))
	{
		if(pmaterial->alpha)
			data << "\t$alpha " << pmaterial->alpha << NEWLINE;
	}

	if(pmaterial->ptextures[MT_TX_SPECULAR])
	{
		if(pmaterial->phong_exp)
			data << "\t$phong_exp " << pmaterial->phong_exp << NEWLINE;

		if(pmaterial->spec_factor)
			data << "\t$spec " << pmaterial->spec_factor << NEWLINE;
	}

	if(pmaterial->scale)
		data << "\t$scopescale " << pmaterial->scale << NEWLINE;

	if (pmaterial->cubemapnormal)
		data << "\t$cubemapnormal " << pmaterial->cubemapnormal << NEWLINE;

	if(pmaterial->cubemapstrength)
		data << "\t$cubemapstrength " << pmaterial->cubemapstrength << NEWLINE;

	if (pmaterial->parallaxscale)
		data << "\t$parallaxscale " << pmaterial->parallaxscale << NEWLINE;

	if (pmaterial->parallaxlayers)
		data << "\t$parallaxlayers " << pmaterial->parallaxlayers << NEWLINE;

	// Set container
	if(!pmaterial->containername.empty())
		data << "\t$container " << pmaterial->containername << NEWLINE;

	// Set material type
	if(!pmaterial->materialname.empty())
		data << "\t$material " << pmaterial->materialname << NEWLINE;

	// Set textures
	if(pmaterial->ptextures[MT_TX_DIFFUSE])
		data << "\t$texture diffuse " << pmaterial->ptextures[MT_TX_DIFFUSE]->filepath << NEWLINE;

	if(pmaterial->ptextures[MT_TX_NORMALMAP])
		data << "\t$texture normalmap " << pmaterial->ptextures[MT_TX_NORMALMAP]->filepath << NEWLINE;

	if(pmaterial->ptextures[MT_TX_DETAIL])
		data << "\t$texture detail " << pmaterial->ptextures[MT_TX_DETAIL]->filepath << NEWLINE;

	if(pmaterial->ptextures[MT_TX_SPECULAR])
		data << "\t$texture specular " << pmaterial->ptextures[MT_TX_SPECULAR]->filepath << NEWLINE;

	if(pmaterial->ptextures[MT_TX_LUMINANCE])
		data << "\t$texture luminance " << pmaterial->ptextures[MT_TX_LUMINANCE]->filepath << NEWLINE;

	if (pmaterial->ptextures[MT_TX_AO])
		data << "\t$texture ao " << pmaterial->ptextures[MT_TX_AO]->filepath << NEWLINE;

	data << "}" << NEWLINE;

	CString fullpath;
	fullpath << "textures/" << pmaterial->filepath;

	const byte* pwritedata = reinterpret_cast<const byte*>(data.c_str());
	m_fileFuncs.pfnWriteFile(pwritedata, data.length(), fullpath.c_str(), false);
}

//=============================================
// @brief Creates an instance of the texture manager
//
//=============================================
CTextureManager* CTextureManager::CreateInstance( const file_interface_t& fileFuncs, pfnPrintf_t printFunction, pfnPrintf_t printErrorFunction, const CGLExtF& glExtF, bool onlyMaterials )
{
	if(!g_pInstance)
		g_pInstance = new CTextureManager(fileFuncs, printFunction, printErrorFunction, glExtF, onlyMaterials);

	return g_pInstance;
}

//=============================================
// @brief Returns the current instance of the texture manager
//
//=============================================
CTextureManager* CTextureManager::GetInstance( void )
{
	return g_pInstance;
}

//=============================================
// @brief Deletes the current instance
//
//=============================================
void CTextureManager::DeleteInstance( void )
{
	if(g_pInstance)
	{
		delete g_pInstance;
		g_pInstance = nullptr;
	}
}