/*************************************************************************** 
  alienBlaster 
  Copyright (C) 2004 
  Paul Grathwohl, Arne Hormann, Daniel Kuehn, Soenke Schwardt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
***************************************************************************/
using namespace std;

#include "surfaceDB.h"
#include <fstream>
#include <iostream>
#include <SDL_image.h>
#ifdef ANDROID
#include <android/log.h>
#endif

SurfaceDB surfaceDB;

SurfaceDB::SurfaceDB( Uint8 transparentR,
		      Uint8 transparentG,
		      Uint8 transparentB ) {
  transR = transparentR;
  transG = transparentG;
  transB = transparentB;
}

SurfaceDB::~SurfaceDB() {
  StringSurfaceMap::iterator pos;
  // free all surfaces
  for ( pos = surfaceDB.begin(); pos != surfaceDB.end(); ++pos ) {
    SDL_FreeSurface( pos->second );
  }
}

SDL_Surface *SurfaceDB::loadSurface( string fn, bool alpha ) {

  SDL_Surface *searchResult = getSurface( fn );
  if ( searchResult ) {
    return searchResult;
  }

  bool isPNG = false;
  // Check if file exist
  FILE * inputFile = fopen( fn.c_str(), "rb");
  if (!inputFile) {
    if( fn.size() > 4 && fn.find(".bmp") != string::npos ) {
      isPNG = true;
      fn = fn.substr( 0, fn.size() - 4 ) + ".png";
      inputFile = fopen( fn.c_str(), "rb");
    }
    if (!inputFile) {
      cout << "ERROR: file " << fn << " does not exist!" << endl;
#ifdef ANDROID
      __android_log_print(ANDROID_LOG_ERROR, "Alien Blaster", (string( "Cannot load image " ) + fn).c_str() );
#endif
      exit(1);
    }
  }
  fclose(inputFile);

  SDL_Surface *newSurface = isPNG ? IMG_Load( fn.c_str() ) : SDL_LoadBMP( fn.c_str() );
  if( newSurface == NULL )
  {
    cout << "ERROR: Cannot load image " << fn << endl;
#ifdef ANDROID
    __android_log_print(ANDROID_LOG_ERROR, "Alien Blaster", (string( "Cannot load image " ) + fn).c_str() );
#endif
    exit(1);
  }
  SDL_SetColorKey( newSurface, SDL_SRCCOLORKEY, 
		   SDL_MapRGB(newSurface->format, transR, transG, transB) );

  SDL_Surface * hwSurface = SDL_DisplayFormat(newSurface);

  if( hwSurface ) {
    SDL_FreeSurface(newSurface);
    newSurface = hwSurface;
  }

  if ( alpha ) {
    hwSurface = SDL_DisplayFormatAlpha(newSurface);
    if( hwSurface ) {
      SDL_FreeSurface(newSurface);
      newSurface = hwSurface;
    }
    SDL_SetAlpha( newSurface, SDL_SRCALPHA, 128 );
    SDL_SetColorKey( newSurface, SDL_SRCCOLORKEY, 
		   SDL_MapRGB(newSurface->format, transR, transG, transB) );
  }

  surfaceDB[ fn ] = newSurface;
  return newSurface;
}

SDL_Surface *SurfaceDB::getSurface( string fn ) {
  if ( surfaceDB.empty() ) {
    return 0;
  } else {
    StringSurfaceMap::iterator pos = surfaceDB.find( fn );
    if ( pos == surfaceDB.end() ) {
      return 0;
    } else {
      return pos->second;
    }
  }
}

