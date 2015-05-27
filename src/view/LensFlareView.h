#ifndef _LENSFLARE_VIEW_H_
#define _LENSFLARE_VIEW_H_

// This file is provided by BHawk3D and placed into the Public Domain
// More info: http://www.ogre3d.org/forums/viewtopic.php?f=11&t=65630

#include "Ogre.h"
namespace Rally { namespace View {

	class LensFlareView {
	private:
		Ogre::SceneManager* sceneManager;
		Ogre::Camera* camera;
		Ogre::SceneNode* node;

		Ogre::BillboardSet* haloSet;
		Ogre::BillboardSet* circleSet;
		Ogre::BillboardSet* burstSet;
		Ogre::Vector3 lightPosition;

		Ogre::Real scale;
		Ogre::Real width;
		Ogre::Real height;

		int numHalos;
		int numCircles;
		Ogre::Real lightLocationX;
		Ogre::Real lightLocationY;

		Ogre::Real lensFlareBaseSize;
		bool visible;
		Ogre::ColourValue   baseHaloColor;
		Ogre::ColourValue   baseCircleColor;

		void reset();
		Ogre::Vector3 getLensFlareDirectionVector();
		void setLensFlareLocations(Ogre::Vector3 dir);
		void calculateLensFlareScreenLocation();
		void calculateLensFlareScale();
		void setLensFlareAlpha();
		void setLensFlareSize();
		void rotateLensFlareSource();
		Ogre::Real getScale();

	public:
		LensFlareView();
		~LensFlareView();
		void init(Ogre::SceneManager* sceneManager,
				Ogre::Camera* camera,
				Ogre::Real width, Ogre::Real height,
				Ogre::Real lensFlareSourceSize,
				Ogre::Real lensFlareBaseSize,
				Ogre::String strHaloMaterialName,
				Ogre::String strCircleMaterialName,
				Ogre::String strSourceMaterialName);

		void update();
		void end();
		void setPosition(Ogre::Vector3 pos);
		void setVisible(bool visible);
		bool getVisible();
		void setHaloColour(Ogre::ColourValue color);
		void setCircleColour(Ogre::ColourValue color);
		void setBurstColour(Ogre::ColourValue color);
	};
}}

#endif
