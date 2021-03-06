#include <gl/pogl.h>
#include <gl/poglmath.h>
#include <thread>
#include "POGLExampleWindow.h"

int main()
{
	POGL_HANDLE windowHandle = POGLCreateExampleWindow(POGL_SIZE(1024, 768), POGL_TOCHAR("Example: Draw Cube"));

	POGL_DEVICE_INFO deviceInfo = { 0 };
#ifdef _DEBUG
	deviceInfo.flags = POGLDeviceInfoFlags::DEBUG_MODE;
#else
	deviceInfo.flags = 0;
#endif
	deviceInfo.windowHandle = windowHandle;
	deviceInfo.colorBits = 32;
	deviceInfo.depthBits = 16;
	deviceInfo.pixelFormat = POGLPixelFormat::R8G8B8A8;
	IPOGLDevice* device = POGLCreateDevice(&deviceInfo);

	try {
		IPOGLRenderContext* context = device->GetRenderContext();

		IPOGLShader* vertexShader = context->CreateShaderFromFile(POGL_TOCHAR("simple.vs"), POGLShaderType::VERTEX_SHADER);
		IPOGLShader* fragmentShader = context->CreateShaderFromFile(POGL_TOCHAR("simple.fs"), POGLShaderType::FRAGMENT_SHADER);
		IPOGLShader* shaders[] = { vertexShader, fragmentShader };
		IPOGLProgram* program = context->CreateProgramFromShaders(shaders, 2);
		vertexShader->Release();
		fragmentShader->Release();

		//
		// Create vertex data for a cube.
		//

		const POGL_POSITION_COLOR_VERTEX VERTICES[] = {
			// Front vertices
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(-1.0f, -1.0f, 1.0f), POGL_COLOR4(1.0f, 0.0f, 0.0f, 1.0f)),
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(1.0f, -1.0f, 1.0f), POGL_COLOR4(0.0f, 1.0f, 0.0f, 1.0f)),
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(1.0f, 1.0f, 1.0f), POGL_COLOR4(0.0f, 0.0f, 1.0f, 1.0f)),
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(-1.0f, 1.0f, 1.0f), POGL_COLOR4(1.0f, 1.0f, 1.0f, 1.0f)),

			// Back vertices
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(-1.0f, -1.0f, -1.0f), POGL_COLOR4(1.0f, 0.0f, 0.0f, 1.0f)),
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(1.0f, -1.0f, -1.0f), POGL_COLOR4(0.0f, 1.0f, 0.0f, 1.0f)),
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(1.0f, 1.0f, -1.0f), POGL_COLOR4(0.0f, 0.0f, 1.0f, 1.0f)),
			POGL_POSITION_COLOR_VERTEX(POGL_VECTOR3(-1.0f, 1.0f, -1.0f), POGL_COLOR4(1.0f, 1.0f, 1.0f, 1.0f))
		};
		IPOGLVertexBuffer* vertexBuffer = context->CreateVertexBuffer(VERTICES, sizeof(VERTICES), POGLPrimitiveType::TRIANGLE, POGLBufferUsage::IMMUTABLE);

		//
		// Create the index buffer for the cube
		//

		const POGL_UINT32 INDICES[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			3, 2, 6,
			6, 7, 3,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// left
			4, 0, 3,
			3, 7, 4,
			// right
			1, 5, 6,
			6, 2, 1
		};
		IPOGLIndexBuffer* indexBuffer = context->CreateIndexBuffer(INDICES, sizeof(INDICES), POGLVertexType::UNSIGNED_INT, POGLBufferUsage::IMMUTABLE);

		//
		// Set viewport
		//

		context->SetViewport(POGL_RECT(0, 0, 1024, 768));

		//
		// Generate a perspective- and lookat matrix
		//

		POGL_MAT4 perspective;
		POGLMat4Perspective(45.0f, 1.2f, 0.1f, 100.0f, &perspective);
	
		POGL_MAT4 lookAt;
		POGLMat4LookAt(POGL_VECTOR3(0.0f, 20.0f, 20.0f), POGL_VECTOR3(0.f, 0.f, 0.f), POGL_VECTOR3(0.0f, 1.0f, 0.0f), &lookAt);

		//
		// Set default uniforms for this program
		//

		program->FindUniformByName("ProjectionMatrix")->SetMatrix(perspective);
		program->FindUniformByName("ViewMatrix")->SetMatrix(lookAt);

		//
		// Set default properties for this program
		//

		program->SetDepthTest(true);
		program->SetDepthFunc(POGLDepthFunc::LESS);
		
		//
		// Apply the program. No need to apply it in the render loop since we only have one program and
		// we don't update the program's properties.
		//

		IPOGLRenderState* state = context->Apply(program);

		//
		// Poll the opened window's events. This is NOT part of the POGL library
		//

		POGL_FLOAT angle = 0.0f;
		const POGL_FLOAT ROTATION_SPEED = 90.0f; 

		while (POGLProcessEvents()) {
			angle += POGLGetTimeSinceLastTick() * ROTATION_SPEED;
			if (angle > 360.0f)
				angle = 360.0f - angle;
			
			state->SetVertexBuffer(vertexBuffer);
			state->SetIndexBuffer(indexBuffer);
			state->Clear(POGLClearType::COLOR | POGLClearType::DEPTH);

			//
			// Retrieve the ModelMatrix for the currently bound program
			//

			IPOGLUniform* modelMatrixUniform = state->FindUniformByName("ModelMatrix");

			//
			// Draw four translated and rotated cubes onto the screen
			//

			POGL_MAT4 modelMatrix;
			POGLMat4Translate(POGL_VECTOR3(-5.0f, 0.0f, 0.0f), &modelMatrix);
			POGLMat4Rotate(angle, modelMatrix, POGL_VECTOR3(0.0f, 1.0f, 0.0f), &modelMatrix);
			modelMatrixUniform->SetMatrix(modelMatrix);
			state->DrawIndexed();

			modelMatrix = POGL_MAT4();
			POGLMat4Translate(POGL_VECTOR3(5.0f, 0.0f, 0.0f), &modelMatrix);
			POGLMat4Rotate(angle, modelMatrix, POGL_VECTOR3(0.0f, 1.0f, 0.0f), &modelMatrix);
			modelMatrixUniform->SetMatrix(modelMatrix);
			state->DrawIndexed();

			modelMatrix = POGL_MAT4();
			POGLMat4Translate(POGL_VECTOR3(0.0f, 0.0f, 5.0f), &modelMatrix);
			POGLMat4Rotate(angle, modelMatrix, POGL_VECTOR3(0.0f, 1.0f, 0.0f), &modelMatrix);
			modelMatrixUniform->SetMatrix(modelMatrix);
			state->DrawIndexed();

			modelMatrix = POGL_MAT4();
			POGLMat4Translate(POGL_VECTOR3(0.0f, 0.0f, -5.0f), &modelMatrix);
			POGLMat4Rotate(angle, modelMatrix, POGL_VECTOR3(0.0f, 1.0f, 0.0f), &modelMatrix);
			modelMatrixUniform->SetMatrix(modelMatrix);
			state->DrawIndexed();

			device->EndFrame();
		}

		state->Release();

		indexBuffer->Release();
		vertexBuffer->Release();
		program->Release();
		context->Release();
		device->Release();
	}
	catch (POGLException e) {
		POGLAlert(e);
	}

	
	POGLDestroyExampleWindow(windowHandle);
	return 0;
}
