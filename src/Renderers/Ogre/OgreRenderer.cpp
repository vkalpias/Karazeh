/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE. 
 *
 */
 
#include "Renderers/Ogre/OgreRenderer.h"
#include "Launcher.h"

using namespace Ogre;
using namespace OgreBites;
namespace Pixy {
	
	OgreRenderer::OgreRenderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "OgreRenderer");
		mLog->infoStream() << "firing up";
		
		mName = "OgreRenderer";
  }
	
	OgreRenderer::~OgreRenderer() {
		
		mLog->infoStream() << "shutting down";
		
		if (fSetup)
		  cleanup();
		  
		if( mRoot )
		    delete mRoot;
		    
		if (mLog)
		  delete mLog;
	}

  void OgreRenderer::loadRenderSystems() 
  {
    bool rendererInstalled = false;
    // try using Direct3D on Windows
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
    try { 
      mRoot->loadPlugin("RenderSystem_Direct3D9"); 
      mRoot->setRenderSystem(mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem"));
      rendererInstalled = true;
    } catch (Ogre::Exception& e) {
      mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.getFullDescription();
    } catch(std::exception& e) { 
      mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.what(); 
    }
#endif

    std::string lPluginsPath;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    lPluginsPath = macBundlePath() + "/Contents/Plugins/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    lPluginsPath = ".\\";
#else
    lPluginsPath = "./";
#endif
    // try using OpenGL
    try {
      mRoot->loadPlugin(lPluginsPath + "RenderSystem_GL");
      if (!rendererInstalled) {
        mRoot->setRenderSystem(mRoot->getRenderSystemByName("OpenGL Rendering Subsystem"));
        rendererInstalled = true;
      }
    } 
    catch(Ogre::Exception& e) { 
      mLog->errorStream() << "Unable to create OpenGL RenderSystem: " << e.getFullDescription(); 
    } 

  }

	bool OgreRenderer::configureGame() {
		
		mLog->infoStream() << "configuring video settings";
		// Load config settings from ogre.cfg
		//if( !mRoot->restoreConfig() ) {
		  Ogre::RenderSystem* mRS = mRoot->getRenderSystem();
		  mRS->setConfigOption("Full Screen", "No");
		  mRS->setConfigOption("Video Mode", "640 x 480");
		  //mRS->setConfigOption("VSync", "Yes");
		    // If there is no config file, show the configuration dialog
		    /*if( !mRoot->showConfigDialog() ) {
		        return false;
		    }*/
		//}
		
		// Initialise and create a default rendering window
		mRenderWindow = mRoot->initialise( true, PIXY_APP_NAME );
		
		// Initialise resources
		//ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
		// Create needed scenemanagers
		Ogre::SceneManager *mSceneMgr = mRoot->createSceneManager( "DefaultSceneManager", "KarazehScene" );
		Ogre::Camera *mCamera = mSceneMgr->createCamera("KarazehCamera");
		mViewport = mRenderWindow->addViewport(mCamera);
		mCamera->setAspectRatio((Ogre::Real)mViewport->getActualWidth() / (Ogre::Real)mViewport->getActualHeight());
		mCamera = 0; mSceneMgr = 0;
		
		return true;
	}
	
	void OgreRenderer::setupResources(std::string inPath) {

		// Load resource paths from config file
		ConfigFile cf;
		cf.load( inPath );
		
		// Go through all settings in the file
		ConfigFile::SectionIterator itSection = cf.getSectionIterator();
		
		String sSection, sType, sArch;
		while( itSection.hasMoreElements() ) {
      sSection = itSection.peekNextKey();
	
      ConfigFile::SettingsMultiMap *mapSettings = itSection.getNext();
      ConfigFile::SettingsMultiMap::iterator itSetting = mapSettings->begin();
      while( itSetting != mapSettings->end() ) {
        sType = itSetting->first;
        sArch = itSetting->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			  ResourceGroupManager::getSingleton().addResourceLocation( String(macBundlePath() + "/" + sArch), sType, sSection);
#else
			  ResourceGroupManager::getSingleton().addResourceLocation( sArch, sType, sSection);
#endif
				
        ++itSetting;
	    }
		}
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
	  
	void OgreRenderer::getWindowHandle(size_t *windowHnd) {
	  mRenderWindow->getCustomAttribute( "WINDOW", windowHnd );
	};
	
	void OgreRenderer::getWindowExtents(int *width, int *height) {
	  (*width) = 640;
	  (*height) = 480;
	};
			  
	bool OgreRenderer::setup() {
    if (fSetup)
      return true;
      
		// setup paths
	  using std::ostringstream;
	  ostringstream lPathResources, lPathPlugins, lPathCfg, lPathOgreCfg, lPathLog;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
	  lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\resources_win32.cfg";
    lPathPlugins << "..\\resources\\config\\plugins.cfg";
    lPathCfg << "..\\resources\\config\\";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
	  lPathLog << PROJECT_LOG_DIR << "\\Ogre.log";	
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	  lPathResources << macBundlePath() << "/Contents/Resources/config/resources_osx.cfg";
	  lPathPlugins << macBundlePath() << "/Contents/Resources/config/plugins.cfg";
	  lPathCfg << macBundlePath() << "/Contents/Resources/config/";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
	  lPathLog << macBundlePath() << "/Contents/Logs/Ogre.log";    
#else
	  lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "/config/resources_linux.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "/config/plugins.cfg";
    lPathCfg << PROJECT_ROOT << PROJECT_RESOURCES << "/config/";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
	  lPathLog << PROJECT_LOG_DIR << "/Ogre.log";
#endif
        
		mRoot = OGRE_NEW Root(lPathPlugins.str(), lPathOgreCfg.str(), lPathLog.str());
		if (!mRoot) {
			throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR, 
								  "Error - Couldn't initalize OGRE!", 
								  std::string(PIXY_APP_NAME) + " - Error");
		}
		loadRenderSystems();
	  
		// Setup and configure game
		if( !this->configureGame() ) {
		    // If game can't be configured, throw exception and quit application
		    throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR,
								  "Error - Couldn't Configure Renderwindow",
								  std::string(PIXY_APP_NAME) + " - Error" );
		    return false;
		}
		
		//mRenderWindow = mRoot->getAutoCreatedWindow();
		WindowEventUtilities::addWindowEventListener( mRenderWindow, this );
		
		mOverlayMgr = Ogre::OverlayManager::getSingletonPtr();
		
		this->setupResources(lPathResources.str());
		
	  return true;
	};
	
	bool OgreRenderer::deferredSetup() {
		mTrayMgr = 
		new SdkTrayManager(
		  "TrayManager", 
		  mRenderWindow, 
		  InputManager::getSingletonPtr()->getMouse(), this);
		
		//mTrayMgr->getTrayContainer(TL_NONE)->hide();
	  //mTrayMgr->setTrayPadding(10);	
	  
	  mOverlayMgr->getByName("Karazeh/UI")->show();
	  mTrayMgr->createLabel(TL_LEFT, "Status", "Status: ");
	  
	  mTrayMgr->createProgressBar(TL_LEFT, "Progess", "Progress", 320, 20);
	  mTrayMgr->adjustTrays();
	  
	};
	
	bool OgreRenderer::cleanup() {
	  delete mTrayMgr;
	};
	
	void OgreRenderer::update(unsigned long lTimeElapsed) {
    
		WindowEventUtilities::messagePump();

    mFrameEvt.timeSinceLastFrame = mFrameEvt.timeSinceLastEvent = lTimeElapsed;		
    mTrayMgr->frameRenderingQueued(mFrameEvt);
    
		// render next frame
	  mRoot->renderOneFrame();
	}
	
	void OgreRenderer::injectError(PATCHERROR errorCode, std::string inMsg) {
	}
  void OgreRenderer::injectNotice(PATCHNOTICE noticeCode, std::string inMsg) {
    mTrayMgr->showOkDialog("Notice", inMsg);
  }
  
  bool OgreRenderer::injectPrompt(std::string inMsg) {
    //mTrayMgr->showYesNoDialog("Notice", inMsg);
    mTrayMgr->showOkDialog("Notice", inMsg);
  }
  void OgreRenderer::injectStatus(std::string inMsg) {
    static_cast<Label*>(mTrayMgr->getWidget("Status"))->setCaption(inMsg);
  }
  void OgreRenderer::injectProgress(int progress) {
  
  }
  
	bool OgreRenderer::keyPressed( const OIS::KeyEvent &e ) {
		return true;
	}
	
	bool OgreRenderer::keyReleased( const OIS::KeyEvent &e ) {
		if (e.key == OIS::KC_ESCAPE)
		  Launcher::getSingleton().requestShutdown();
		  
		return true;
	}
	
	bool OgreRenderer::mouseMoved( const OIS::MouseEvent &e ) {
		OIS::MouseState state = e.state;
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
		if (mTrayMgr->injectMouseMove(e)) return true;
		
		return true;
	}
	
	bool OgreRenderer::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  if (mTrayMgr->injectMouseDown(orientedEvt, id)) return true;
		
		return true;
	}
	
	bool OgreRenderer::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  
	  if (mTrayMgr->injectMouseUp(orientedEvt, id)) return true;
		
		return true;
	}
	
  void OgreRenderer::buttonHit(OgreBites::Button* b) {
    /*
		if (b->getName() == "TrayManager/YesButton")   // enter configuration screen
		{
		  Patcher::getSingleton().doPatch(0);
		}*/
  };	
};
