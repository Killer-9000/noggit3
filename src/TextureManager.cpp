#include <algorithm>

#include "TextureManager.h"
#include "Log.h"
#include "mpq.h"
#include "video.h"

#pragma pack(push,1)
struct BLPHeader 
{
	int magix;
	int version;
	char attr_0_compression;
	char attr_1_alphadepth;
	char attr_2_alphatype;
	char attr_3_mipmaplevels;
	int resx;
	int resy;
	int offsets[16];
	int sizes[16];
};
#pragma pack(pop)

GLuint TextureManager::get(std::string name)
{
	std::transform (name.begin(), name.end(), name.begin(), ::tolower );
	return names[name];
}

GLuint TextureManager::add(std::string name)
{
	GLuint id;
	std::string originalName = name;
	std::transform( name.begin(), name.end(), name.begin(), ::tolower );
	if( names.find( name ) != names.end( ) ) 
	{
		id = names[name];
		items[id]->addref( );
		return id;
	}
		
	glGenTextures( 1, &id );

	Texture *tex = new Texture( name );
	tex->originalName = originalName;
	tex->id = id;
	
	LoadBLP(id, tex);

	do_add(name, id, tex);

	return id;
}

void TextureManager::reload()
{
	LogDebug << "Reloading textures.." << std::endl;
	for( std::map<std::string, GLuint>::iterator it = names.begin( ); it != names.end( ); ++it )
	{
		LoadBLP( it->second, reinterpret_cast<Texture*>( items[it->second] ) );
	}
	Log << "Finished reloading textures." << std::endl;
}

bool TextureManager::LoadBLP(GLuint id, Texture *tex)
{
	BLPHeader * lHeader;
	int w, h;
	uint8_t * lData;

	glBindTexture( GL_TEXTURE_2D, id );
	
	MPQFile f( tex->originalName.c_str( ) );
	if ( f.isEof( ) ) 
	{
		tex->id = 0;
		return false;
	}


	lData = f.getPointer( );
	lHeader = reinterpret_cast<BLPHeader*>( lData );
	tex->w = w = lHeader->resx;
	tex->h = h = lHeader->resy;

	switch( lHeader->attr_0_compression )
	{
	case 1:
		{
			// uncompressed
			unsigned int * pal = reinterpret_cast<unsigned int*>( lData + sizeof( BLPHeader ) );

			unsigned char *buf = new unsigned char[lHeader->sizes[0]];
			unsigned int *buf2 = new unsigned int[w*h];
			unsigned int *p;
			unsigned char *c, *a;

			int alphabits = lHeader->attr_1_alphadepth;
			bool hasalpha = alphabits != 0;

			for (int i=0; i<16; i++) {
				if (w==0) w = 1;
				if (h==0) h = 1;
				if (lHeader->offsets[i] && lHeader->sizes[i]) {
					f.seek(lHeader->offsets[i]);
					f.read(buf,lHeader->sizes[i]);

					int cnt = 0;
					p = buf2;
					c = buf;
					a = buf + w*h;
					for (int y=0; y<h; y++) {
						for (int x=0; x<w; x++) {
							unsigned int k = pal[*c++];
							k = ((k&0x00FF0000)>>16) | ((k&0x0000FF00)) | ((k& 0x000000FF)<<16);
							int alpha;
							if (hasalpha) {
								if (alphabits == 8) {
									alpha = (*a++);
								} else if (alphabits == 1) {
									alpha = (*a & (1 << cnt++)) ? 0xff : 0;
									if (cnt == 8) {
										cnt = 0;
										a++;
									}
								}
							} else alpha = 0xff;

							k |= alpha << 24;
							*p++ = k;
						}
					}

					glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf2);

				} else break;
				w >>= 1;
				h >>= 1;
			}

			delete[] buf2;
			delete[] buf;

			break;
		}
	case 2:
		{
			// compressed

			//                         0 (0000) & 3 == 0                1 (0001) & 3 == 1                    7 (0111) & 3 == 3
			const int alphatypes[] = { GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT };
			const int blocksizes[] = { 8,                               16,                               0, 16 };
			
			int lTempAlphatype = lHeader->attr_2_alphatype & 3;
			GLint format = alphatypes[lTempAlphatype];
			int blocksize = blocksizes[lTempAlphatype];
			format = format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ? ( lHeader->attr_1_alphadepth == 1 ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT ) : format;

			// do every mipmap level
			for( int i = 0; i < 16; i++ ) 
			{
				w = w == 0 ? 1 : w;
				h = h == 0 ? 1 : h;

				if( lHeader->offsets[i] && lHeader->sizes[i] ) 
					glCompressedTexImage2D( GL_TEXTURE_2D, i, format, w, h, 0, ( (w + 3) / 4) * ( (h + 3 ) / 4 ) * blocksize, reinterpret_cast<char*>( lData + lHeader->offsets[i] ) );
				else 
					break;

				w >>= 1;
				h >>= 1;
			}

			break;
		}
	}

	f.close();

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	return f.isExternal();
}

void TextureManager::doDelete(GLuint id)
{
	glDeleteTextures(1, &id);
}
