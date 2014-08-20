#include "MemCheck.h"
#include "POGLDeferredCommands.h"
#include "POGLFactory.h"
#include "POGLVertexBuffer.h"
#include "POGLTexture2D.h"
#include "POGLRenderState.h"
#include "POGLDeferredRenderContext.h"
#include "POGLFramebuffer.h"
#include "POGLEnum.h"

void POGLNothing_Release(POGL_DEFERRED_COMMAND* command)
{
}

void POGLNothing_Command(POGLDeferredRenderContext*, POGLRenderState*, POGL_DEFERRED_COMMAND*)
{
}

void POGLCreateVertexBuffer_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLCreateVertexBufferCommand* cmd = (POGLCreateVertexBufferCommand*)command;

	//
	// Generate buffers and attach the vertex buffers layout to it
	//

	const GLuint bufferID = POGLFactory::GenBufferID();
	const GLuint vaoID = POGLFactory::GenVertexArrayObjectID(bufferID, cmd->vertexBuffer->GetLayout());
	cmd->vertexBuffer->PostConstruct(bufferID, vaoID);

	//
	// Set the vertex buffer as the "current buffer" on the render state
	//

	state->SetVertexBuffer(cmd->vertexBuffer);

	//
	// Map the data
	//

	void* map = glMapBufferRange(GL_ARRAY_BUFFER, 0, cmd->size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	memcpy(map, context->GetMapPointer(cmd->memoryPoolOffset), cmd->size);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void POGLCreateVertexBuffer_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLCreateVertexBufferCommand* cmd = (POGLCreateVertexBufferCommand*)command;
	cmd->vertexBuffer->Release();
}

void POGLCreateTexture2D_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLCreateTexture2DCommand* cmd = (POGLCreateTexture2DCommand*)command;

	const POGLTextureFormat::Enum format = cmd->texture->GetTextureFormat();
	const POGL_SIZE& size = cmd->texture->GetSize();
	const GLenum _format = POGLEnum::ConvertToTextureFormatEnum(format);
	const GLenum _internalFormat = POGLEnum::ConvertToInternalTextureFormatEnum(format);
	const GLenum minFilter = POGLEnum::Convert(POGLMinFilter::DEFAULT);
	const GLenum magFilter = POGLEnum::Convert(POGLMagFilter::DEFAULT);
	const GLenum textureWrap = POGLEnum::Convert(POGLTextureWrap::DEFAULT);

	const GLuint textureID = POGLFactory::GenTextureID();
	cmd->texture->PostConstruct(textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	POGL_HANDLE pointer = nullptr;
	if (cmd->size > 0) {
		pointer = context->GetMapPointer(cmd->memoryPoolOffset);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, size.width, size.height, 0, _format, GL_UNSIGNED_BYTE, pointer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

	const GLenum status = glGetError();
	if (status != GL_NO_ERROR) {
		THROW_EXCEPTION(POGLResourceException, "Could not create 2D texture. Reason: 0x%x", status);
	}

	state->SetTextureResource((POGLTextureResource*)cmd->texture->GetResourcePtr());
}

void POGLCreateTexture2D_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLCreateTexture2DCommand* cmd = (POGLCreateTexture2DCommand*)command;
	cmd->texture->Release();
}

void POGLMapVertexBuffer_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLMapVertexBufferCommand* cmd = (POGLMapVertexBufferCommand*)command;
	state->BindVertexBuffer(cmd->vertexBuffer);
	void* map = glMapBufferRange(GL_ARRAY_BUFFER, 0, cmd->size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	memcpy(map, context->GetMapPointer(cmd->memoryPoolOffset), cmd->size);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void POGLMapVertexBuffer_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLMapVertexBufferCommand* cmd = (POGLMapVertexBufferCommand*)command;
	cmd->vertexBuffer->Release();
}

void POGLMapRangeVertexBuffer_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLMapRangeVertexBufferCommand* cmd = (POGLMapRangeVertexBufferCommand*)command;
	state->BindVertexBuffer(cmd->vertexBuffer);
	void* map = glMapBufferRange(GL_ARRAY_BUFFER, cmd->offset, cmd->length, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	memcpy(map, context->GetMapPointer(cmd->memoryPoolOffset), cmd->length);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void POGLMapRangeVertexBuffer_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLMapRangeVertexBufferCommand* cmd = (POGLMapRangeVertexBufferCommand*)command;
	cmd->vertexBuffer->Release();
}

void POGLClear_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLClearCommand* cmd = (POGLClearCommand*)command;
	state->Clear(cmd->clearBits);
}

void POGLSetFramebuffer_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLSetFramebufferCommand* cmd = (POGLSetFramebufferCommand*)command;
	state->SetFramebuffer(cmd->framebuffer);
}

void POGLSetFramebuffer_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLSetFramebufferCommand* cmd = (POGLSetFramebufferCommand*)command;
	if (cmd->framebuffer != nullptr)
		cmd->framebuffer->Release();
}

void POGLDraw_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLDeferredDrawCommand* cmd = (POGLDeferredDrawCommand*)command;
	state->Draw(cmd->vertexBuffer, cmd->indexBuffer, cmd->startIndex);
}

void POGLDrawCount_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLDeferredDrawCommand* cmd = (POGLDeferredDrawCommand*)command;
	state->Draw(cmd->vertexBuffer, cmd->indexBuffer, cmd->startIndex, cmd->count);
}

void POGLDraw_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLDeferredDrawCommand* cmd = (POGLDeferredDrawCommand*)command;
	cmd->vertexBuffer->Release();
	if (cmd->indexBuffer != nullptr)
		cmd->indexBuffer->Release();
}

void POGLSetDepthTest_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLBooleanCommand* cmd = (POGLBooleanCommand*)command;
	state->SetDepthTest(cmd->value);
}

void POGLSetDepthMask_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLBooleanCommand* cmd = (POGLBooleanCommand*)command;
	state->SetDepthMask(cmd->value);
}

void POGLSetStencilTest_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLBooleanCommand* cmd = (POGLBooleanCommand*)command;
	state->SetStencilTest(cmd->value);
}

void POGLSetBlend_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLBooleanCommand* cmd = (POGLBooleanCommand*)command;
	state->SetBlend(cmd->value);
}

void POGLColorMask_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLColorMaskCommand* cmd = (POGLColorMaskCommand*)command;
	state->SetColorMask(cmd->mask);
}

void POGLStencilMask_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLStencilMaskCommand* cmd = (POGLStencilMaskCommand*)command;
	state->SetStencilMask(cmd->mask);
}

void POGLSetDepthFunc_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLSetDepthFuncCommand* cmd = (POGLSetDepthFuncCommand*)command;
	state->SetDepthFunc(cmd->depthFunc);
}

void POGLSetBlendFunc_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLSetBlendFuncCommand* cmd = (POGLSetBlendFuncCommand*)command;
	state->SetBlendFunc(cmd->sfactor, cmd->dfactor);
}

void POGLSetFrontFace_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLSetFrontFaceCommand* cmd = (POGLSetFrontFaceCommand*)command;
	state->SetFrontFace(cmd->frontFace);
}

void POGLSetCullFace_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLSetCullFaceCommand* cmd = (POGLSetCullFaceCommand*)command;
	state->SetCullFace(cmd->cullFace);
}

void POGLSetViewport_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLSetViewportCommand* cmd = (POGLSetViewportCommand*)command;
	state->SetViewport(cmd->viewport);
}

void POGLApplyProgram_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLApplyProgramCommand* cmd = (POGLApplyProgramCommand*)command;
	state->Apply(cmd->program);
}

void POGLApplyProgram_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLApplyProgramCommand* cmd = (POGLApplyProgramCommand*)command;
	cmd->program->Release();
}

void POGLCreateFrameBuffer_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLCreateFrameBufferCommand* cmd = (POGLCreateFrameBufferCommand*)command;

	IPOGLTexture* textures[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	const POGL_UINT32 size = cmd->framebuffer->GetNumDrawBuffers();
	for (POGL_UINT32 i = 0; i < size; ++i) {
		IPOGLTexture* texture = cmd->framebuffer->GetTexture(i);
		textures[i] = texture;
		texture->Release();
	}

	IPOGLTexture* depthStencilTexture = cmd->framebuffer->GetDepthStencilTexture();
	GLuint frameBufferID = POGLFactory::GenFramebufferObjectID(textures, depthStencilTexture);
	cmd->framebuffer->PostConstruct(frameBufferID);
	depthStencilTexture->Release();
	
	state->SetFramebuffer(cmd->framebuffer);
}

void POGLCreateFrameBuffer_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLCreateFrameBufferCommand* cmd = (POGLCreateFrameBufferCommand*)command;
	cmd->framebuffer->Release();
}

void POGLResizeTexture2D_Command(POGLDeferredRenderContext* context, POGLRenderState* state, POGL_DEFERRED_COMMAND* command)
{
	POGLResizeTexture2DCommand* cmd = (POGLResizeTexture2DCommand*)command;

	POGLTextureResource* resource = cmd->texture->GetResourcePtr();
	glBindTexture(GL_TEXTURE_2D, resource->GetTextureID());
	state->SetTextureResource(resource);

	const POGLTextureFormat::Enum format = resource->GetTextureFormat();
	const GLenum _format = POGLEnum::ConvertToTextureFormatEnum(format);
	const GLenum _internalFormat = POGLEnum::ConvertToInternalTextureFormatEnum(format);
	const POGL_SIZE& size = cmd->newSize;

	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, size.width, size.height, 0, _format, GL_UNSIGNED_BYTE, NULL);
	CHECK_GL("Could not set new texture size");

	cmd->texture->SetSize(cmd->newSize);
}

void POGLResizeTexture2D_Release(POGL_DEFERRED_COMMAND* command)
{
	POGLResizeTexture2DCommand* cmd = (POGLResizeTexture2DCommand*)command;
	cmd->texture->Release();
}
