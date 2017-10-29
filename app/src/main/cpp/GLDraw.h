#ifndef GLDRAW_H
#define GLDRAW_H

#include <map>
#include <string>
#include <utility>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

class GLDraw {
public:	/* 抽象関数 */
	virtual void Init() = 0;
	virtual void PreDraw(int width, int height) = 0;
	virtual void Draw() = 0;
	virtual void Fin() = 0;

protected:
	GLuint mProgramHandle = -1;
	std::map<std::string, int> mVariableHandles;

protected:
	void createProgram(const char *vertexshader, const char *fragmentshader, GLuint &ProgramHandle, std::map<std::string, int> &VariableHandles);

private:
	GLuint loadShader(int i, const char *vertexshader);
	void checkGlError(const char *argstr);

public:	/* for System */
	GLDraw(){};
	virtual ~GLDraw(){};
	GLDraw(const GLDraw &obj){}
	GLDraw(const GLDraw&& obj){}
};

class GLDrawTriangle  : public GLDraw {
public:
	void Init();
	void PreDraw(int width, int height);
	void Draw();
	void Fin();

private:
	const char *VERTEXSHADER =
		"attribute vec4 vPosition;\n"
		"uniform mat4 u_rotMatrix;\n"
		"void main() {\n"
		"    gl_Position = u_rotMatrix * vPosition;\n"
		"}\n";

	const char *FRAGMENTSHADER =
		"precision mediump float;\n"
		"void main() {\n"
		"    gl_FragColor = vec4(1.0, 0.0, 1.0, 0.2);\n"
		"}\n";

	static const int AMOUNTOFMOVE = -5;
	float mMoveX = AMOUNTOFMOVE;
	float mMoveY = AMOUNTOFMOVE;
	float mxPos = 100;
	float myPos = 130;
	int mDspW = 0;
	int mDspH = 0;
};

#endif //GLDRAW_H
