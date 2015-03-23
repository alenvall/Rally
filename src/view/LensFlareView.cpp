#include "view/LensFlareView.h"

namespace Rally { namespace View {

	LensFlareView::LensFlareView() {
		reset();
	}

	LensFlareView::~LensFlareView() { } 

	void LensFlareView::init(Ogre::SceneManager* sceneManager,
		Ogre::Camera* camera, 
		Ogre::Real width, Ogre::Real height,
		Ogre::Real lensFlareSourceSize,
		Ogre::Real lensFlareBaseSize,
		Ogre::String strHaloMaterialName,
		Ogre::String strCircleMaterialName,
		Ogre::String strSourceMaterialName) {

			LensFlareView::sceneManager = sceneManager;
			LensFlareView::camera = camera;
			LensFlareView::width = width;
			LensFlareView::height = height;
			LensFlareView::lensFlareBaseSize = lensFlareBaseSize;

			haloSet = sceneManager->createBillboardSet();
			haloSet->setMaterialName(strHaloMaterialName);
			haloSet->setCullIndividually(true);

			circleSet = sceneManager->createBillboardSet();
			circleSet->setMaterialName(strCircleMaterialName);
			circleSet->setCullIndividually(true);

			burstSet= sceneManager->createBillboardSet();
			burstSet->setMaterialName(strSourceMaterialName);
			burstSet->setCullIndividually(true);

			node = sceneManager->getRootSceneNode()->createChildSceneNode();
			node->attachObject(burstSet);
			node->attachObject(circleSet);
			node->attachObject(haloSet);

			numHalos = 3;
			for(int i = 0; i < numHalos; i++)
				haloSet->createBillboard(0, 0, 0);

			numCircles = 12;
			for(int i = 0; i < numCircles; i++)
				circleSet->createBillboard(0, 0, 0);

			// this will represent the source of the light (i.e. the sun)
			burstSet->createBillboard(0, 0, 0);
			burstSet->getBillboard(0)->setDimensions(lensFlareSourceSize, lensFlareSourceSize);
			burstSet->getBillboard(0)->setColour(Ogre::ColourValue(1, 1, 1, 1));

			haloSet->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
			circleSet->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
			burstSet->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);

			scale = 1;
	}

	void LensFlareView::end() {
		node->detachObject(haloSet);
		node->detachObject(circleSet);
		node->detachObject(burstSet);
		sceneManager->destroyBillboardSet(haloSet);
		sceneManager->destroyBillboardSet(circleSet);
		sceneManager->destroyBillboardSet(burstSet);
		sceneManager->destroySceneNode(node);

		reset();
	}

	void LensFlareView::reset() {
		sceneManager = NULL;
		camera = NULL;
		node = NULL;
		haloSet = NULL;
		circleSet = NULL;
		burstSet = NULL;
		lightPosition = Ogre::Vector3::ZERO;
		scale = 0;
		width = 0;
		height = 0;
		numHalos = 0;
		numCircles = 0;
		lightLocationX = 0;
		lightLocationY = 0;
		lensFlareBaseSize = 0;
		baseHaloColor = Ogre::ColourValue(1, 1, 1, 1);
		baseCircleColor = Ogre::ColourValue(1, 1, 1, 1);
		visible = false;
	}

	void LensFlareView::update() {
		// if the Light is out of the camera field of view, the lensflare is hidden.
		if (!camera->isVisible(lightPosition)) {
			setVisible(false);
			scale = 0;
			return;
		}

		setVisible(true);

		calculateLensFlareScreenLocation();
		setLensFlareLocations(getLensFlareDirectionVector());
		calculateLensFlareScale(); // mScale will be 0 - 1
		setLensFlareAlpha();
		setLensFlareSize();
		rotateLensFlareSource();
	}

	Ogre::Vector3 LensFlareView::getLensFlareDirectionVector() {
		Ogre::Vector3 camPos = camera->getDerivedPosition();
		Ogre::Real lightDistance = (camPos - lightPosition).length();
		Ogre::Vector3 cameraVect = camPos + (lightDistance * camera->getDirection());
		return cameraVect - lightPosition;
	}

	void LensFlareView::setLensFlareLocations(Ogre::Vector3 dir) {
		// flares behind the Sun
		circleSet->getBillboard(0)->setPosition(dir * -0.4f); 
		haloSet->getBillboard(0)->setPosition(dir * -0.3f);

		// sun
		burstSet->getBillboard(0)->setPosition(Ogre::Vector3::ZERO);

		// five circles and a gap
		circleSet->getBillboard(1)->setPosition(dir * 0.1f);
		circleSet->getBillboard(2)->setPosition(dir * 0.2f);
		circleSet->getBillboard(3)->setPosition(dir * 0.4f);
		circleSet->getBillboard(4)->setPosition(dir * 0.4f);
		circleSet->getBillboard(5)->setPosition(dir * 0.5f);

		circleSet->getBillboard(6)->setPosition(dir * 0.8f);
		circleSet->getBillboard(7)->setPosition(dir * 0.9f);
		circleSet->getBillboard(8)->setPosition(dir * 1.0f);
		circleSet->getBillboard(9)->setPosition(dir * 1.1f);
		circleSet->getBillboard(10)->setPosition(dir * 1.2f);
		circleSet->getBillboard(11)->setPosition(dir * 1.3f);

		//Two Halos
		haloSet->getBillboard(1)->setPosition(dir * 1.8f);
		haloSet->getBillboard(2)->setPosition(dir * 2.1f);
	}

	void LensFlareView::calculateLensFlareScreenLocation() {
		Ogre::Vector2 r;
		Ogre::Vector3 point = camera->getProjectionMatrix() * 
			(camera->getViewMatrix() * lightPosition);
		r.x = (point.x / 2) + 0.5f;
		r.y = 1 - ((point.y / 2) + 0.5f);
		lightLocationX = r.x * width;
		lightLocationY = r.y * height;
	}

	void LensFlareView::calculateLensFlareScale() {
		// 2D coordinates of the source of the lens flare
		Ogre::Vector2 currentLocation;
		// 2D coordinates of the center of the screen
		Ogre::Vector2 centerLocation;

		//Claculete the distance between the center of the screen and a corner
		currentLocation.x = width - 1;
		currentLocation.y = height - 1;
		centerLocation.x = width / 2;
		centerLocation.y = height / 2;
		Ogre::Real maxDist = (currentLocation - centerLocation).length();

		//Calculate the distance from the center of the screen to the
		//Lens Flare source location
		currentLocation.x = lightLocationX;
		currentLocation.y = lightLocationY;
		Ogre::Real dist = (currentLocation - centerLocation).length();

		scale = (1 - (dist / maxDist));
	}

	void LensFlareView::setLensFlareAlpha() {
		Ogre::Real total = Ogre::Real(haloSet->getNumBillboards());
		for(int i = 0; i < total; i++) {
			haloSet->getBillboard(i)->setColour(baseHaloColor * scale);
		}

		total = Ogre::Real(circleSet->getNumBillboards());
		for(int i = 0; i < total; i++) {
			circleSet->getBillboard(i)->setColour(baseCircleColor * scale);
		}
	}

	void LensFlareView::setLensFlareSize() {
		Ogre::Real size = lensFlareBaseSize * scale; 

		// flares behind the Sun
		circleSet->getBillboard(0)->setDimensions(size * 2, size * 2); 
		haloSet->getBillboard(0)->setDimensions(size * 3, size * 3);

		// five Circles and a gap
		circleSet->getBillboard(1)->setDimensions(size * 2, size * 2);
		circleSet->getBillboard(2)->setDimensions(size * 3, size * 3);
		circleSet->getBillboard(3)->setDimensions(size * 2.2f, size * 2.2f);
		circleSet->getBillboard(4)->setDimensions(size * 0.5f, size * 0.5f);
		circleSet->getBillboard(5)->setDimensions(size, size);

		circleSet->getBillboard(6)->setDimensions(size, size);
		circleSet->getBillboard(7)->setDimensions(size * 4, size * 4);
		circleSet->getBillboard(8)->setDimensions(size * 2, size * 2);
		circleSet->getBillboard(9)->setDimensions(size * 3, size * 3);
		circleSet->getBillboard(10)->setDimensions(size * 2, size * 2);
		circleSet->getBillboard(11)->setDimensions(size * 1.5f, size * 1.5f);

		// two Halos
		haloSet->getBillboard(1)->setDimensions(size * 7, size * 7);
		haloSet->getBillboard(2)->setDimensions(size * 12, size * 12);
	}

	void LensFlareView::rotateLensFlareSource() {
		Ogre::Vector2 source(lightLocationX, lightLocationY);
		Ogre::Vector2 dest(width - 1, height - 1);
		Ogre::Real rotMaxDist = dest.length();
		Ogre::Real rotDist = (dest - source).length();

		Ogre::Real rotScale = (1 - (rotDist / rotMaxDist));

		burstSet->getBillboard(0)->setRotation(Ogre::Radian(Ogre::Degree(90 * rotScale)));
	}

	void LensFlareView::setVisible(bool visible) {
		haloSet->setVisible(visible);
		circleSet->setVisible(visible);
		//mBurstSet->setVisible(visible);
		visible = visible;
	}

	bool LensFlareView::getVisible() {
		return visible;
	}

	void LensFlareView::setHaloColour(Ogre::ColourValue color) { 
		baseHaloColor = color;
	}

	void LensFlareView::setCircleColour(Ogre::ColourValue color) { 
		baseCircleColor = color; 
	}

	void LensFlareView::setBurstColour(Ogre::ColourValue color) {
		Ogre::Real total = Ogre::Real(burstSet->getNumBillboards());
		for(int i = 0; i < total; i++) {
			burstSet->getBillboard(i)->setColour(color);
		}
	} 

	void LensFlareView::setPosition(Ogre::Vector3 pos) { 
		lightPosition = pos;
		node->setPosition(lightPosition); 
	}

	Ogre::Real LensFlareView::getScale() { 
		return scale;
	}
}}
