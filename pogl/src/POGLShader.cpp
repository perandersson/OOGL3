#include "MemCheck.h"
#include "POGLShader.h"

namespace {
	std::atomic<POGL_UINT32> uid;
	POGL_UINT32 GenShaderProgramUID() {
		return ++uid;
	}
}

POGLShader::POGLShader(POGLShaderType::Enum shaderType)
: mRefCount(1), mUID(0), mShaderID(0), mShaderType(shaderType)
{
}

POGLShader::~POGLShader()
{
}

void POGLShader::PostConstruct(GLuint shaderID)
{
	mShaderID = shaderID;
	mUID = GenShaderProgramUID();
}

void POGLShader::AddRef()
{
	mRefCount++;
}

void POGLShader::Release()
{
	if (--mRefCount == 0) {
		if (mShaderID != 0) {
			glDeleteShader(mShaderID);
			mShaderID = 0;
		}
		delete this;
	}
}

POGLResourceType::Enum POGLShader::GetType() const
{
	return POGLResourceType::SHADER;
}
