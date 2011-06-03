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

#ifndef H_QtRenderer_H
#define H_QtRenderer_H

#include "Renderer.h"
#include <QtGui/QApplication>
#include <QtGui/QtGui>

namespace Pixy {
	
	/*	\class QtRenderer
	 *	\brief
	 *	
	 */
	class QtRenderer : public Renderer {
		
	public:
	  QtRenderer();
		virtual ~QtRenderer();
		
		/* \brief
		 *
		 */
		virtual bool setup(int argc, char** argv);
		
		/* \brief
		 *
		 */
		virtual bool deferredSetup();

		/* \brief
		 *
		 */		
		virtual void update(unsigned long lTimeElapsed);
		
		/* \brief
		 *
		 */
 		virtual bool cleanup();

		virtual void getWindowHandle(size_t *windowHnd);
		virtual void getWindowExtents(int *width, int *height);

		virtual bool keyPressed( const OIS::KeyEvent &e );
		virtual bool keyReleased( const OIS::KeyEvent &e );		
		virtual bool mouseMoved( const OIS::MouseEvent &e );
		virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
		QWidget* getWindow();
		void setupWidgets();
		virtual void go();
	protected:
		
		static void startQt(int argc, char** argv);
		
		virtual bool evtUnableToConnect(Event* inEvt);
		virtual bool evtValidateStarted(Event* inEvt);
		virtual bool evtValidateComplete(Event* inEvt);
		virtual bool evtPatchStarted(Event* inEvt);
		virtual bool evtPatchProgress(Event* inEvt);
		virtual bool evtPatchFailed(Event* inEvt);
		virtual bool evtPatchComplete(Event* inEvt);
		virtual bool evtApplicationPatched(Event* inEvt);
		
		int margc;
		char **margv;
		
		QApplication* mApp;
		QDialog *mYesNoDlg;
		QVBoxLayout *mLayout;
		QPushButton *mQuitButton;
		QWidget *mWindow;
	private:
		QtRenderer(const QtRenderer& src);
		QtRenderer& operator=(const QtRenderer& rhs);
	};
}
#endif