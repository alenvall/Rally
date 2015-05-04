//-----------------------------------------------------------------------------
// Lonewolff
//
// Filename:    OgreText.h
// Description: Class for simple text in Ogre (Version 040507:18.30)
//-----------------------------------------------------------------------------
 

#ifndef __OgreText_H__
#define __OgreText_H__

#include <OgreTextAreaOverlayElement.h>
#include <OgreStringConverter.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreFontManager.h>
 
using namespace Ogre;

#define FONT_FILE_NAME "bebas.ttf"
#define FONT_FOLDER "../../media/fonts"

namespace Rally { namespace View {

class OgreText
{
public:
    OgreText()
    {
		// get the resource manager
	ResourceGroupManager &resGroupMgr = ResourceGroupManager::getSingleton();
	// tell it to look at this location
	resGroupMgr.addResourceLocation(FONT_FOLDER, "FileSystem");
	// get the font manager
	FontManager &fontMgr = FontManager::getSingleton();
	// create a font resource
	ResourcePtr font = fontMgr.create("MyFont","General");
	// set as truetype
	font->setParameter("type","truetype");
	// set the .ttf file name
	font->setParameter("source",FONT_FILE_NAME);
	// set the size
	font->setParameter("size","70");
	// set the dpi
	font->setParameter("resolution","96");
	// load the ttf
	font->load();

        olm=OverlayManager::getSingletonPtr();
        if(init==0)
        {
        panel=static_cast<OverlayContainer*>(olm->createOverlayElement("Panel","GUI"));
            panel->setMetricsMode(Ogre::GMM_PIXELS);
            panel->setPosition(0,0);
            panel->setDimensions(1.0f,1.0f);
            overlay=olm->create("GUI_OVERLAY");
            overlay->add2D(panel);
        }
        ++(this->init);
        szElement="element_"+StringConverter::toString(init);
        overlay=olm->getByName("GUI_OVERLAY");
        panel=static_cast<OverlayContainer*>(olm->getOverlayElement("GUI"));
        textArea=static_cast<TextAreaOverlayElement*>(olm->createOverlayElement("TextArea",szElement));
        panel->addChild(textArea);
        overlay->show();
    }
    ~OgreText()
    {
        szElement="element_"+StringConverter::toString(init);
        olm->destroyOverlayElement(szElement);
        --(this->init);
        if(init==0)
        {
            olm->destroyOverlayElement("GUI");
            olm->destroy("GUI_OVERLAY");
        }
    }
    void setText(char *szString)
    {
        textArea->setCaption(szString);
        textArea->setDimensions(1.0f,1.0f);
        textArea->setMetricsMode(Ogre::GMM_RELATIVE);
        textArea->setFontName("MyFont");
        //textArea->setCharHeight(0.6f);
    }
    void setText(String szString) // now You can use Ogre::String as text
    {
        textArea->setCaption(szString);
        textArea->setDimensions(1.0f,1.0f);
        textArea->setMetricsMode(Ogre::GMM_RELATIVE);
        textArea->setFontName("MyFont");
        //textArea->setCharHeight(0.6f);
    }
    void setPos(float x,float y)
    {
        textArea->setPosition(x,y);
    }
    void setCol(float R,float G,float B,float I)
    {
        textArea->setColour(Ogre::ColourValue(R,G,B,I));
    }
	void setTextSize(float textSize){
		textArea->setCharHeight(textSize);
	}
private:
    OverlayManager *olm;
    OverlayContainer *panel ;
    Overlay *overlay;
    TextAreaOverlayElement *textArea;
    static int init;
    String szElement;
};
 


} }

#endif