#include "Font3D.h"
#include <assert.h>
//Font 3D does not load shaders, let the code that uses it create the shaders
// quite a simple font system, and only prints 1 colour (could change colour from a uniform to an attrib)


Font3D::Font3D()
{ // don't really need to do much here the init should handle it but do ensure the buffer is nulled 
  // that way if the buffer is not actually created, it won't accidently remove random memory	
	this->VertexTextBuffer = NULL; // going to make sure it is set to NULL
	SetFontSize(16); // set a default size for the font, it can be altered;
	SetScale(1.0f);
	CurrentColour = glm::vec4(1.0); // white as default
}

Font3D::~Font3D()
{    // there should be a buffer, if there is remove it and any textures can be freed as the buffer 
	 // won't get created if the init didn't happen
	if (this->VertexTextBuffer != NULL)
	{
		free(this->VertexTextBuffer);
	}
}

// load a font, convert to texture, create a buffer...easy, PO must be set up 1st
void Font3D::Init(int Width, int Height, const char* FontFile)
{
	this->ScreenHeight = Height; // this has to be the FB resolution, not the screen res.
	this->ScreenWidth = Width; // our text will not draw outside these


// load the font image, if it fails stop
	MyFiles FH;

	int	 Theight, Twidth;
	TexBuffer = NULL;

	TexBuffer = FH.Load(FontFile, &Twidth, &Theight);
	assert(TexBuffer != NULL); // we will stop if we failed to load

	U_Size = 1.0f / (Twidth / Font_Size);
	V_Size = 1.0f / (Theight / Font_Size);


	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Twidth, Theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	free(TexBuffer); // its in GPU MEM now so kill the daa

// ok it loaded	let our handle know

	the_shader->use();
	
	glGenBuffers(1, &this->TexBuffHandle);

	// we dont' load any shaders in here bUt we should be comfortable that the shader has been made so we can set up values
	this->HalfSizeID = the_shader->get_uniform("u_HalfSize");
	this->SamplerLoc = the_shader->get_uniform("u_Sampler");
	this->ColourLoc = the_shader->get_uniform("u_Colour");
	// the uniforms are given const values	

	glUniform1i(SamplerLoc, 0); // use texture 0
	glUniform2f(HalfSizeID, ScreenWidth / 2, ScreenHeight / 2);
	glUniform4fv(ColourLoc, 1, &this->CurrentColour[0]);


	// get the attrib	

	this->PosLoc = the_shader->get_attrib("a_Position");
	this->texCoord = the_shader->get_attrib("a_texCoord");

	// now create a buffer
	this->VertexTextBuffer = (VertexInfo*)malloc(sizeof(VertexInfo) * (MAX_TEXT * VERTS_PER_CHAR)); // this is where we create our draw array, destructor must free it.
	this->CharsToWrite = 0;
	glUseProgram(0); // release the PO


}


// We print text into our buffer but do not render it until a game cycle is complete
void Font3D::PrintText(const char* text, int x, int y)
{
	// shaders are not so good with ints doing clipspace calcs so we need to send them floats

	float fx = x;
	float fy = y;

	VertexInfo* tBuffer = &VertexTextBuffer[CharsToWrite * VERTS_PER_CHAR];

	for (; *text != 0 && CharsToWrite < MAX_TEXT; CharsToWrite++, text++, fx += Font_Size * Scale) // parse the text string until 0 is fouund, or max chars is reached
	{
		char character = *text - 32; // we discount anythng below the space (ascii 32)
		float u = (character % Font_Size) * (U_Size);
		float v = (character / Font_Size) * (V_Size);
		// we store 6 vertex values, x and y, but also the uv coords for the tile
		*tBuffer++ = { fx, fy, u, v };
		*tBuffer++ = { fx, fy + Font_Size * Scale, u, v + V_Size };
		*tBuffer++ = { fx + Font_Size * Scale, fy, u + U_Size, v };
		*tBuffer++ = { fx + Font_Size * Scale, fy + Font_Size * Scale, u + U_Size, v + V_Size };
		*tBuffer++ = { fx + Font_Size * Scale, fy, u + U_Size, v };
		*tBuffer++ = { fx, fy + Font_Size * Scale, u, v + V_Size };
	}
}



void Font3D::RenderText()
{
	the_shader->use();

// bind that texture buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->TexBuffHandle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	// now set up the data buffer with our vertices	
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(VertexInfo) * VERTS_PER_CHAR * CharsToWrite,
		VertexTextBuffer,
		GL_STATIC_DRAW);

	// prep the attribute writes
	glVertexAttribPointer(this->PosLoc,
		2, // 2 values x and y
		GL_FLOAT, // these are ints cast to floats
		false,
		sizeof(VertexInfo),
		(const void*)0);

	glVertexAttribPointer(this->texCoord,
		2, // 2 values u v
		GL_FLOAT,
		false,
		sizeof(VertexInfo),
		(const void*)(sizeof(float) * 2));

	// enable them

	glEnableVertexAttribArray(PosLoc);
	glEnableVertexAttribArray(texCoord);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// get busy with it!	
	glDrawArrays(GL_TRIANGLES, 0, CharsToWrite * VERTS_PER_CHAR);

	// clean up

	CharsToWrite = 0;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}