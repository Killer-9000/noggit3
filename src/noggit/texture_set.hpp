// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/MPQ.h>
#include <noggit/alphamap.hpp>
#include <stdint.h>

namespace OpenGL
{
	class Texture;
}

class Brush;
class MapTile;

class TextureSet
{
public:
	TextureSet();
	~TextureSet();

	void initTextures(MPQFile* f, MapTile *maintile, uint32_t size);
	void initAlphamaps(MPQFile* f, size_t nLayers, bool mBigAlpha, bool doNotFixAlpha);

	void start2DAnim(int id);
	void stop2DAnim(int id);
	void startAnim(int id);
	void stopAnim(int id);

	void bindTexture(size_t id, size_t activeTexture);
	void bindAlphamap(size_t id, size_t activeTexture);

	int addTexture(OpenGL::Texture *texture);
  void eraseTexture(size_t id);
	void eraseTextures();
  // return true if at least 1 texture has been erased
  bool eraseUnusedTextures();
  void swapTexture(int id1, int id2);
	void switchTexture(OpenGL::Texture* oldTexture, OpenGL::Texture* newTexture);
	bool paintTexture(float xbase, float zbase, float x, float z, Brush* brush, float strength, float pressure, OpenGL::Texture* texture);
  bool canPaintTexture(OpenGL::Texture* texture);

	const std::string& filename(size_t id);

	const size_t num();
	const unsigned int flag(size_t id);
	const unsigned int effect(size_t id);

	void setAlpha(size_t id, size_t offset, unsigned char value);
	void setAlpha(size_t id, unsigned char *amap);

	const unsigned char getAlpha(size_t id, size_t offset);
	const unsigned char *getAlpha(size_t id);

  void convertToBigAlpha();
  void convertToOldAlpha();

  void mergeAlpha(size_t id1, size_t id2);
  bool removeDuplicate();

	OpenGL::Texture* texture(size_t id);

private:
	OpenGL::Texture* textures[4];
	Alphamap* alphamaps[3];
	size_t nTextures;

	int tex[4];
	int animated[4];

	unsigned int texFlags[4];
	unsigned int effectID[4];
	unsigned int MCALoffset[4];
};