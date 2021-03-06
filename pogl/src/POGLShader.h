#pragma once
#include "config.h"

class POGLShader : public IPOGLShader
{
public:
	POGLShader(POGLShaderType::Enum shaderType);
	virtual ~POGLShader();

	/*!
		\brief Method called when this object is constructed by the immediately render context

		\param shaderID
	*/
	void PostConstruct(GLuint shaderID);
	
	/*!
		\brief Retrieves a unique ID for this vertex buffer
	*/
	inline POGL_UINT32 GetUID() const {
		return mUID;
	}

	/*!
		\brief Retrieves the OpenGL Shader ID
	*/
	inline GLuint GetShaderID() const {
		return mShaderID;
	}

	/*!
		\brief Retrieves this programs type
	*/
	inline POGLShaderType::Enum GetShaderType() const {
		return mShaderType;
	}
	
// IPOGLInterface
public:
	virtual void AddRef();
	virtual void Release();

// IPOGLResource
public:
	virtual POGLResourceType::Enum GetType() const;

private:
	REF_COUNTER mRefCount;
	POGL_UID mUID;
	GLuint mShaderID;
	POGLShaderType::Enum mShaderType;
};
