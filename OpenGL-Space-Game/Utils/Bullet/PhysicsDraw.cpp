#include "PhysicsDraw.h"

// GLuint PhysicsDraw::ProgramObject;

void PhysicsDraw::drawLine(const btVector3& from,
	const btVector3& to,
	const btVector3& color)
{
	// draws a simple line of pixels between points but stores them for later draw
	LineValues Line;
	Line.p1 = from;
	Line.p2 = to;

	TheLines.push_back(Line);
	// we don't care about colour?	

}

void PhysicsDraw::DoDebugDraw(const Camera& the_active_camera)
{
	m_the_shader->use();
	const glm::mat4& Projection	= the_active_camera.get_projection_m4();
	const glm::mat4& View		= the_active_camera.get_view_m4();
	glm::mat4 VP = Projection * View;  // Remember order seems backwards
	m_the_shader->set_mat4("MVP", VP);

	// set up the line shader and then draw the buffer
			//load the vertex data info
	glVertexAttribPointer(this->positionLoc,
		3,	// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		4 * sizeof(float),  // how many floats to the next one(be aware btVector3 uses 4 floats)
		(GLfloat*)&this->TheLines[0]  // where do they start as an index); // use 3 values, but add stride each time to get to the next
	);

	glEnableVertexAttribArray(this->positionLoc);
	glDrawArrays(GL_LINES, 0, TheLines.size() * 2);

	TheLines.clear();

}
void PhysicsDraw::drawContactPoint(const btVector3& pointOnB,
	const btVector3& normalOnB,
	btScalar distance,
	int lifeTime,
	const btVector3& color)
{
	drawLine(pointOnB, pointOnB + normalOnB * distance, color);
}


void PhysicsDraw::ToggleDebugFlag(int flag)
{
	// checks if a flag is set and enables/
	// disables it
	if (m_debugMode & flag)
		// flag is enabled, so disable it
		m_debugMode = m_debugMode & (~flag);
	else
		// flag is disabled, so enable it
		m_debugMode |= flag;
}