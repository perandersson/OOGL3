#include <gl/pogl.h>
#include <gl/poglmath.h>
#include <thread>
#include <atomic>
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
		IPOGLShader* shaders[] = { vertexShader, fragmentShader, nullptr };
		IPOGLProgram* program = context->CreateProgramFromShaders(shaders);
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
		IPOGLVertexBuffer* vertexBuffer = context->CreateVertexBuffer(VERTICES, sizeof(VERTICES), POGLPrimitiveType::TRIANGLE, POGLBufferUsage::STATIC);

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
		IPOGLIndexBuffer* indexBuffer = context->CreateIndexBuffer(INDICES, sizeof(INDICES), POGLVertexType::UNSIGNED_INT, POGLBufferUsage::STATIC);

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
		POGLMat4LookAt(POGL_VECTOR3(-20.0f, 20.0f, 20.0f), POGL_VECTOR3(0.f, 0.f, 0.f), POGL_VECTOR3(0.0f, 1.0f, 0.0f), &lookAt);

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

		std::atomic<bool> running(true);
		std::atomic<POGL_FLOAT> angle(0.0f);

		IPOGLDeferredRenderContext* deferredContext = device->CreateDeferredRenderContext();
		std::condition_variable condition;
		std::thread t([deferredContext, program, vertexBuffer, indexBuffer, &angle, &condition, &running] {
			//
			// Rotate the cube based on the total application time
			//

			try {
				std::mutex m;
				std::unique_lock<std::mutex> lock(m);
				while (running) {
					POGL_MAT4 modelMatrix;
					POGLMat4Rotate(angle, POGL_VECTOR3(0.0f, 1.0f, 0.0f), &modelMatrix);
					// 
					// Draw the cube
					//
					IPOGLRenderState* state = deferredContext->Apply(program);

					state->FindUniformByName("ModelMatrix")->SetMatrix(modelMatrix);

					state->Clear(POGLClearType::COLOR | POGLClearType::DEPTH);
					state->Draw(vertexBuffer, indexBuffer);
					state->Release();

					//
					// Flush the commands 
					//

					deferredContext->Flush();

					//
					// Wait until the commands have been executed. Otherwise the deferred context will start allocating memory faster then it will be released.
					// The reason for this happening is because this thread will be executed much much faster than the main thread because of all
					// OpenGL commands. We allocate variables faster than we can release them.
					//

					if (running)
						condition.wait(lock);
				}
			}
			catch (POGLException e) {
				POGLAlert(e);
			}
		});
		
		//
		// Poll the opened window's events. This is NOT part of the POGL library
		//

		POGL_FLOAT angleFlt = 0.0f;
		const POGL_FLOAT ROTATION_SPEED = 90.0f;

		while (POGLProcessEvents()) {
			angleFlt += POGLGetTimeSinceLastTick() * ROTATION_SPEED;
			if (angleFlt > 360.0f)
				angleFlt = 360.0f - angle;
			angle = angleFlt;

			//
			// Execute the deferred context commands and notify the thread so that it generates the commands once again
			//
			
			deferredContext->ExecuteCommands(context);
			condition.notify_one();

			device->EndFrame();
		}

		//
		// Stop running and wait for the thread to stop running
		//

		running = false;
		condition.notify_one();
		t.join();

		deferredContext->Release();
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