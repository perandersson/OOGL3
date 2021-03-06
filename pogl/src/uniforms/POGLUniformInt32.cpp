#include "MemCheck.h"
#include "POGLUniformInt32.h"

POGLUniformInt32::POGLUniformInt32(POGL_UINT32 programUID, POGLRenderState* state, GLint componentID, GLenum uniformType)
: POGLDefaultUniform(programUID, state, componentID, uniformType), mCount(0)
{
	mValues[0] = 0;
	mValues[1] = 0;
	mValues[2] = 0;
	mValues[3] = 0;

	mValuesSet[0] = INT_MAX;
	mValuesSet[1] = INT_MAX;
	mValuesSet[2] = INT_MAX;
	mValuesSet[3] = INT_MAX;
}

POGLUniformInt32::~POGLUniformInt32()
{
}

void POGLUniformInt32::Apply()
{
	switch (mCount) {
	case 3:
		if (mValuesSet[0] == mValues[0] && mValuesSet[1] == mValues[1] && mValuesSet[2] == mValues[2]) return;
		mValuesSet[0] = mValues[0];
		mValuesSet[1] = mValues[1];
		mValuesSet[2] = mValues[2];
		glUniform3iv(mComponentID, 1, mValues);
		break;
	case 2:
		if (mValuesSet[0] == mValues[0] && mValuesSet[1] == mValues[1]) return;
		mValuesSet[0] = mValues[0];
		mValuesSet[1] = mValues[1];
		glUniform2iv(mComponentID, 1, mValues);
		break;
	case 4:
		if (mValuesSet[0] == mValues[0] && mValuesSet[1] == mValues[1] && mValuesSet[2] == mValues[2] && mValuesSet[3] == mValues[3]) return;
		mValuesSet[0] = mValues[0];
		mValuesSet[1] = mValues[1];
		mValuesSet[2] = mValues[2];
		mValuesSet[3] = mValues[3];
		glUniform4iv(mComponentID, 1, mValues);
		break;
	case 1:
		if (mValuesSet[0] == mValues[0]) return;
		mValuesSet[0] = mValues[0];
		glUniform1iv(mComponentID, 1, mValues);
		break;
	default:
		return;
	}

	CHECK_GL("Could not assign int uniform values");
}

void POGLUniformInt32::SetInt32(POGL_INT32 a)
{
	mCount = 1;
	mValues[0] = a;
	mValues[1] = INT_MAX;
	mValues[2] = INT_MAX;
	mValues[3] = INT_MAX;

	if (IsProgramActive())
		POGLUniformInt32::Apply();
}

void POGLUniformInt32::SetInt32(POGL_INT32 a, POGL_INT32 b)
{
	mCount = 2;
	mValues[0] = a;
	mValues[1] = b;
	mValues[2] = INT_MAX;
	mValues[3] = INT_MAX;

	if (IsProgramActive())
		POGLUniformInt32::Apply();
}

void POGLUniformInt32::SetInt32(POGL_INT32 a, POGL_INT32 b, POGL_INT32 c)
{
	mCount = 3;
	mValues[0] = a;
	mValues[1] = b;
	mValues[2] = c;
	mValues[3] = INT_MAX;

	if (IsProgramActive())
		POGLUniformInt32::Apply();
}

void POGLUniformInt32::SetInt32(POGL_INT32 a, POGL_INT32 b, POGL_INT32 c, POGL_INT32 d)
{
	mCount = 4;
	mValues[0] = a;
	mValues[1] = b;
	mValues[2] = c;
	mValues[3] = c;

	if (IsProgramActive())
		POGLUniformInt32::Apply();
}

void POGLUniformInt32::SetInt32(POGL_INT32* ptr, POGL_UINT32 count)
{
	const POGL_UINT32 clampedCount = count > 4 ? 4 : count;
	for (POGL_UINT32 i = 0; i < clampedCount; ++i)
		mValues[i] = ptr[i];

	if (IsProgramActive())
		POGLUniformInt32::Apply();
}

void POGLUniformInt32::SetUInt32(POGL_UINT32 a)
{
	POGLUniformInt32::SetInt32((POGL_INT32)a);
}

void POGLUniformInt32::SetUInt32(POGL_UINT32 a, POGL_UINT32 b)
{
	POGLUniformInt32::SetInt32((POGL_INT32)a, (POGL_INT32)b);
}

void POGLUniformInt32::SetUInt32(POGL_UINT32 a, POGL_UINT32 b, POGL_UINT32 c)
{
	POGLUniformInt32::SetInt32((POGL_INT32)a, (POGL_INT32)b, (POGL_INT32)c);
}

void POGLUniformInt32::SetUInt32(POGL_UINT32 a, POGL_UINT32 b, POGL_UINT32 c, POGL_UINT32 d)
{
	POGLUniformInt32::SetInt32((POGL_INT32)a, (POGL_INT32)b, (POGL_INT32)c, (POGL_INT32)d);
}

void POGLUniformInt32::SetUInt32(POGL_UINT32* ptr, POGL_UINT32 count)
{
	const POGL_UINT32 clampedCount = count > 4 ? 4 : count;
	for (POGL_UINT32 i = 0; i < clampedCount; ++i)
		mValues[i] = (POGL_INT32)ptr[i];

	if (IsProgramActive())
		POGLUniformInt32::Apply();
}

void POGLUniformInt32::SetSize(const POGL_SIZE& size)
{
	POGLUniformInt32::SetInt32(size.x, size.y);
}

void POGLUniformInt32::SetRect(const POGL_RECT& rect)
{
	POGLUniformInt32::SetInt32(rect.x, rect.y, rect.width, rect.height);
}
