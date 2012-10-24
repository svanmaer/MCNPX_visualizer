#ifndef TREKMANIA_TEXTUREFACTORY
#define TREKMANIA_TEXTUREFACTORY

#define NR_TEXTURES (50)

#include <QGLWidget>

//#include "freeglut.h"
#include "Singleton.h"
#include <string>
#include <map>
using namespace std;

class TextureFactory : public Singleton<TextureFactory>
{
public:
	TextureFactory();
	virtual ~TextureFactory();

	void LoadTexture(string key, string filename);
	GLuint GetTexture(string key);

	static TextureFactory& getSingleton();
	static TextureFactory* getSingletonPtr();

private:
	map<string,GLuint> textureKeys;
	GLuint textures[NR_TEXTURES];

	int counter;
};

#endif