#pragma once

// it is sometimes useful to draw the bounding containment around physics objects.
// Bullet was designed to do this with OpenGL 1.xx which had line draw systems
// we are using shader based draw systems so some issues occur when trying to draw lines
// we must store them in a buffer and use a flush sysem to draw and release them
// this will be slow but this is a debug system

#include <bullet/LinearMath/btIDebugDraw.h>

#ifdef RASPBERRY
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif // RASPBERRY

#include <vector>

#include "Utils/Shader/Shader.h"
#include "Headers/Camera.h"

// there are already debug draw systems in Bullet, all we are doing is overriding them
/*
enum	DebugDrawModes
{
	DBG_NoDebug              = 0,
	DBG_DrawWireframe        = 1,
	DBG_DrawAabb             = 2,
	DBG_DrawFeaturesText     = 4,
	DBG_DrawContactPoints    = 8,
	DBG_NoDeactivation       = 16,
	DBG_NoHelpText           = 32,
	DBG_DrawText             = 64,
	DBG_ProfileTimings       = 128,
	DBG_EnableSatComparison  = 256,
	DBG_DisableBulletLCP     = 512,
	DBG_EnableCCD            = 1024,
	DBG_DrawConstraints      = (1 << 11),
	DBG_DrawConstraintLimits = (1 << 12),
	DBG_FastWireframe        = (1 << 13),
	DBG_DrawNormals          = (1 << 14),
	DBG_MAX_DEBUG_DRAW_MODE
};
*/

class PhysicsDraw : public btIDebugDraw
{
public:
	struct
	{
		btVector3 p1;
		btVector3 p2;

	} typedef LineValues;

	// overide the original model funcitons

	void setDebugMode(int debugMode) override
	{
		m_debugMode = debugMode; //DBG_DrawWireframe
	}
	int getDebugMode() const override
	{
		return m_debugMode;
	}

	// and provide a new drawline function
	void drawLine(const btVector3& from,
		const btVector3& to,
		const btVector3& color) override;

	void	DoDebugDraw(const Camera& the_active_camera); // this will flush out the buffer




// we might use this	

	void reportErrorWarning(const char* warningString) override
	{
		printf("Physics reports and error:- %s /n", warningString);
	}


	// though unused we must define these	
	void draw3dText(const btVector3& location, const char* textString) override
	{
		printf("Attempting to draw:- %s /n", textString);
	}

	void drawContactPoint(const btVector3& pointOnB,
		const btVector3& normalOnB,
		btScalar distance,
		int lifeTime,
		const btVector3& color) override;


	void ToggleDebugFlag(int flag);
	int m_debugMode;

	// shader info	
	static GLuint ProgramObject;
	GLuint positionLoc;
	// 	

	std::vector<LineValues> TheLines;

	Shader* m_the_shader;
};
