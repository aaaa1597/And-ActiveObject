#include <android/log.h>
#include "Matrix.h"
#include "GLDraw.h"

/************/
/* 親クラス */
/************/
/* GL-Program生成 */
void GLDraw::createProgram(const char *vertexshader, const char *fragmentshader, GLuint &ProgramHandle, std::map<std::string, int> &VariableHandles) {
	ProgramHandle = GL_FALSE;
	VariableHandles.clear();

	GLuint vhandle = loadShader(GL_VERTEX_SHADER, vertexshader);
	if(vhandle == GL_FALSE) return;

	GLuint fhandle = loadShader(GL_FRAGMENT_SHADER, fragmentshader);
	if(fhandle == GL_FALSE) return;

	ProgramHandle = glCreateProgram();
	if(ProgramHandle == GL_FALSE) {
		checkGlError("glCreateProgram");
		return;
	}

	glAttachShader(ProgramHandle, vhandle);
	checkGlError("glAttachShader(VERTEX_SHADER)");
	glAttachShader(ProgramHandle, fhandle);
	checkGlError("glAttachShader(FRAGMENT_SHADER)");

//	if(seqno == 0)
//		glBindAttribLocation( programhandle, seqno, "vPosition");
//	else if(seqno == 1)
//		glBindAttribLocation( programhandle, seqno, "vLinePosition");

	glLinkProgram(ProgramHandle);
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkStatus);
	if(linkStatus != GL_TRUE) {
		GLint bufLen = 0;
		glGetProgramiv(ProgramHandle, GL_INFO_LOG_LENGTH, &bufLen);
		if(bufLen) {
			char *logstr = new char[bufLen];
			glGetProgramInfoLog(ProgramHandle, bufLen, NULL, logstr);
			__android_log_print(ANDROID_LOG_ERROR, "aaaaa", "GLDraw::createProgram() glLinkProgram() Fail!!\n %s", logstr);
			delete[] logstr;
		}
		glDeleteProgram(ProgramHandle);
		ProgramHandle = -1;
	}

	std::string vpos = "vPosition";
	GLint vposhandle = glGetAttribLocation(ProgramHandle, vpos.c_str());
	VariableHandles.insert(std::make_pair(vpos, vposhandle));

	std::string urotM = "u_rotMatrix";
	GLint urotMhandle = glGetUniformLocation(ProgramHandle, urotM.c_str());
	VariableHandles.insert(std::make_pair(urotM, urotMhandle));

	/* 後処理 */
	glDeleteShader(vhandle);
	glDeleteShader(fhandle);

	return ;
}

/* GL-シェーダ生成 */
GLuint GLDraw::loadShader(int shadertype, const char *shadersource) {
	GLuint shaderhandle = glCreateShader(shadertype);
	if(!shaderhandle) return GL_FALSE;

	glShaderSource(shaderhandle, 1, &shadersource, NULL);
	glCompileShader(shaderhandle);

	GLint compiled = GL_FALSE;
	glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shaderhandle, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen) {
			char *logbuf = new char[infoLen];
			if(logbuf) {
				glGetShaderInfoLog(shaderhandle, infoLen, NULL, logbuf);
				__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "shader failuer!!\n%s", logbuf);
				delete[] logbuf;
			}
		}
		glDeleteShader(shaderhandle);
		shaderhandle = GL_FALSE;
	}

	return shaderhandle;
}

/* GL-エラーチェック */
void GLDraw::checkGlError(const char *argstr) {
	for(GLuint error = glGetError(); error; error = glGetError())
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "after %s errcode=%d", argstr, error);
}

/**************/
/* 三角形描画 */
/**************/
/* 初期化 */
void GLDrawTriangle::Init() {
	GLDraw::createProgram(VERTEXSHADER, FRAGMENTSHADER, mProgramHandle, mVariableHandles);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa GLDrawTriangle::Init() mProgramHandle=%d mVariableHandles.size()=%d vshader=%s fshader=%s ", mProgramHandle, mVariableHandles.size(), VERTEXSHADER, FRAGMENTSHADER);
	return;
}

/* 前準備 */
void GLDrawTriangle::PreDraw(int width, int height) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa GLDrawTriangle::PreDraw(w=%d h=%d)", width, height);
	mDspW = width, mDspH = height;
	glUseProgram(mProgramHandle);

	static const GLfloat vertexes[] = {
		  0,   0, 0,
		0.5,   0, 0,
		  0,-0.5, 0,

		  0,-0.5, 0,
		0.5,   0, 0,
		0.5,-0.5, 0,
	};

	GLuint handle = mVariableHandles.find("vPosition")->second;
	glVertexAttribPointer(handle, 3, GL_FLOAT, GL_FALSE, 0, vertexes);
	glEnableVertexAttribArray(handle);

	return;
};

/* 描画 */
void GLDrawTriangle::Draw() {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa GLDrawTriangle::Draw()");

	mxPos += mMoveX;
	myPos += mMoveY;
	if((mxPos > (2*mDspW)) || (mxPos < 0)) mMoveX = -mMoveX;
	if((myPos > (2*mDspH)) || (myPos < 0)) mMoveY = -mMoveY;

	float translateMatrix[4][4];
	Matrix::LoadIdentity(translateMatrix);
//        Matrix::TranslateMatrix(mxPos/DspW-1, myPos/DspH-1, 0, translateMatrix);
	Matrix::TranslateMatrix(mDspW/2.f/mDspW, mDspH/2.f/mDspH, 0, translateMatrix);	/* 1st ドット座標中心へ移動 */
	Matrix::ScaleMatrix(1, -1, 1, translateMatrix);									/* 2nd Y軸反転 */
	Matrix::ScaleMatrix(1.1, 1, 0, translateMatrix);								/* 3rd アスペクト比 */
	Matrix::RotateMatrix(36, 0, 0, 1, translateMatrix);								/* 4th 回転 */

	/* OpenGL用に転置 */
	float glM[] = {
		translateMatrix[0][0],translateMatrix[1][0],translateMatrix[2][0],translateMatrix[3][0],
		translateMatrix[0][1],translateMatrix[1][1],translateMatrix[2][1],translateMatrix[3][1],
		translateMatrix[0][2],translateMatrix[1][2],translateMatrix[2][2],translateMatrix[3][2],
		translateMatrix[0][3],translateMatrix[1][3],translateMatrix[2][3],translateMatrix[3][3]
	};

	glUseProgram(mProgramHandle);
	GLuint handle = mVariableHandles.find("u_rotMatrix")->second;
	glUniformMatrix4fv(handle, 1, false, glM);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	return;
}

/* 終了処理 */
void GLDrawTriangle::Fin() {
	return;
}
