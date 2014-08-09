#include "MemCheck.h"
#include "POGLDevice.h"

// 
// Exceptions
//
#include <string>
#ifdef UNICODE
std::wstring GenExceptionMessage(const char* format, va_list argp)
{
	POGL_CHAR tmp[5096];
	vswprintf_s(tmp, 5096, format, argp);
	return std::wstring(tmp);
}
#else
std::string GenExceptionMessage(const char* format, va_list argp)
{
	POGL_CHAR tmp[5096];
	vsprintf_s(tmp, 5096, format, argp);
	return std::string(tmp);
}
#endif

POGLException::POGLException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file)
{
	strcpy_s(mFunction, sizeof(mFunction), function);
	strcpy_s(mFile, sizeof(mFile), file);
	mLine = line;
}

POGLException::POGLException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file, const POGL_CHAR* message, ...)
{
	strcpy_s(mFunction, sizeof(mFunction), function);
	strcpy_s(mFile, sizeof(mFile), file);
	mLine = line;

	va_list arglist;
	va_start(arglist, message);
	strcpy_s(mMessage, sizeof(mMessage), GenExceptionMessage(message, arglist).c_str());
	va_end(arglist);
}

POGLException::~POGLException()
{
}

POGLResourceException::POGLResourceException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file, const POGL_CHAR* message, ...)
: POGLException(function, line, file)
{
	va_list arglist;
	va_start(arglist, message);
	strcpy_s(mMessage, sizeof(mMessage), GenExceptionMessage(message, arglist).c_str());
	va_end(arglist);
}

POGLResourceException::~POGLResourceException()
{
}

POGLEffectException::POGLEffectException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file, const POGL_CHAR* message, ...)
: POGLException(function, line, file)
{
	va_list arglist;
	va_start(arglist, message);
	strcpy_s(mMessage, sizeof(mMessage), GenExceptionMessage(message, arglist).c_str());
	va_end(arglist);
}

POGLEffectException::~POGLEffectException()
{
}

POGLStateException::POGLStateException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file, const POGL_CHAR* message, ...)
: POGLException(function, line, file)
{
	va_list arglist;
	va_start(arglist, message);
	strcpy_s(mMessage, sizeof(mMessage), GenExceptionMessage(message, arglist).c_str());
	va_end(arglist);
}

POGLStateException::~POGLStateException()
{
}

POGLSyncException::POGLSyncException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file, const POGL_CHAR* message, ...)
: POGLException(function, line, file)
{
	va_list arglist;
	va_start(arglist, message);
	strcpy_s(mMessage, sizeof(mMessage), GenExceptionMessage(message, arglist).c_str());
	va_end(arglist);
}

POGLSyncException::~POGLSyncException()
{
}

POGLStreamException::POGLStreamException(const POGL_CHAR* function, const POGL_UINT64 line, const POGL_CHAR* file, const POGL_CHAR* message, ...)
: POGLException(function, line, file)
{
	va_list arglist;
	va_start(arglist, message);
	strcpy_s(mMessage, sizeof(mMessage), GenExceptionMessage(message, arglist).c_str());
	va_end(arglist);
}

POGLStreamException::~POGLStreamException()
{
}

//
//
//

POGLDevice::POGLDevice()
{

}

POGLDevice::~POGLDevice()
{
}

const POGL_DEVICE_INFO* POGLDevice::GetDeviceInfo()
{
	return &mDeviceInfo;
}

bool POGLDevice::Initialize(const POGL_DEVICE_INFO* info)
{
	memcpy(&mDeviceInfo, info, sizeof(mDeviceInfo));
	return true;
}