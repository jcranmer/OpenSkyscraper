#ifndef OSS_CLASSIC_SIMTOWER_H
#define OSS_CLASSIC_SIMTOWER_H

#include "external.h"

#include "application.h"


namespace OSS {
	class SimTower : public Object, public Singleton<SimTower> {
	private:
		//Resources
		class Resource : public Object {
		public:
			unsigned short id;
			unsigned short type;
			unsigned int length;
			void * data;
			string getName();
			string getDumpName();
		};
		
		typedef vector< Pointer<Resource> > ResourceVector;
		ResourceVector resources;
		Resource * getResource(unsigned short type, unsigned short id);
		
		//Resource Types
		enum {
			kCursorResource         = 0x1,
			kBitmapResource			= 0x2,
			kIconResource           = 0x3,
			kMenuResource           = 0x4,
			kDialogBoxResource      = 0x5,
			kStringTableResource    = 0x6,
			kAcceleratorResource    = 0x9,
			kGroupCursorResource    = 0xC,
			kGroupIconResource      = 0xE
		};

		static unsigned int kPercellBitmapResource; // CGPK
		static unsigned int kSoundResource; // WAVE
		static unsigned int kReplacePaletteResource; // CLUT
		
		//Resource Names
		typedef struct {
			unsigned short resourceID;
			string name;
		} ResourceName;
		static ResourceName bitmapResourceNames[];
		static ResourceName soundResourceNames[];
		static string getNameForResource(Resource * resource);
		
		//Some default colors for later use
	private:
		static const color3d skyThroughWindow[];
		
	public:
		const Pointer<Application> application;
		SimTower(Application * application) : application(application) {}
		
		//Loads the resources of the SimTower executable
		void loadResources();
		
		//Extracts various components from the loaded resources
		void extractAll();
		void extractTextures();
		void extractSounds();
		
		//Creates one or more textures from the given information
		void postprocessTexture(string textureName, const void * buffer, unsigned int bufferLength);
		void applyReplacementPalette(unsigned short id);
		void spawnSkyTextures(string textureName, ILuint image);
		void spawnCloudTextures(string textureName, ILuint image);
		void spawnLobbyTextures(string textureName, ILuint image);
		void spawnElevatorTextures(string textureName, ILuint image);
		void spawnFloordigitTextures(string textureName, ILuint image);
		
		//Dumping textures to disk for debugging purposes
		static string getDumpPath(string type, string name);
		static void dumpTexture(Texture * texture);
		static void dumpSound(string name, const void * buffer, unsigned int bufferLength);
	};
}


#endif
