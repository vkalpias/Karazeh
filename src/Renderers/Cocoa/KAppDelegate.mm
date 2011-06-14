//
//  CAppAppDelegate.m
//  CApp
//
//  Created by Ahmad Amireh on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Renderers/Cocoa/KAppDelegate.h"
#import "Renderers/Cocoa/CocoaRenderer.h"
#import "Launcher.h"

@implementation KAppDelegate

@synthesize window, btnLaunch, txtStatus, txtLatestChanges, progressBar;

// used for identifying dialog contexts
const int PATCH_AVAILABLE = 1;
const int PATCH_FAILED = 2;

enum DIALOG_TYPE {
  DIALOG_ERROR,
  DIALOG_PROMPT
};

-(void) applicationDidFinishLaunching:(NSNotification *)aNotification {
  static_cast<Pixy::CocoaRenderer*>(Pixy::Launcher::getSingleton().getRenderer())->assignRenderer();
  [txtLatestChanges setEditable: false];
}

-(void) applicationWillTerminate:(NSNotification *)notification {
  // if the launcher is going to launch the external app now, there's no
  // need to shut it down; it's already shutting down
  if (!Pixy::Launcher::getSingleton().isLaunching())
    Pixy::Launcher::shutdown();
}

-(void) showDialog: (DIALOG_TYPE) dtype : (NSString*) dcaption : (NSString*) dmsg : (void*)dcontext {
  NSAlert *alert = [[[NSAlert alloc] init] autorelease];
  [alert addButtonWithTitle: (dtype == DIALOG_ERROR) ? @"Quit" : @"OK"];
  if (dtype == DIALOG_PROMPT)
    [alert addButtonWithTitle:@"Cancel"];
  [alert setMessageText:dcaption];
  [alert setInformativeText:dmsg];
  [alert setAlertStyle: (dtype == DIALOG_PROMPT) ? NSInformationalAlertStyle : NSCriticalAlertStyle];
  
  [alert beginSheetModalForWindow:window 
                   modalDelegate:self 
                   didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) 
                   contextInfo:(void*)dcontext];
  
}

-(void) alertDidEnd:(NSWindow *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo {
  int dialogId = *(int*)contextInfo;
  if (dialogId == PATCH_FAILED) {
    [[NSApplication sharedApplication] terminate:self];
    return;
  }
  
  // patch or quit?
  if (returnCode == NSAlertFirstButtonReturn)
    Pixy::Launcher::getSingleton().updateApplication();
  else
    [[NSApplication sharedApplication] terminate:self];
  
}

-(void) unableToConnect {
  [txtStatus setStringValue: @"Unable to resolve patch server, please verify your internet connectivity."];
}

-(void) validateStarted {
  [txtStatus setStringValue: @"Validating version..."];
}

-(void) validateComplete: (BOOL) inNeedUpdate : (Version&) inVersion {
  if (inNeedUpdate) {
    [self showDialog:DIALOG_PROMPT 
                    : @"Updates are available." 
                    : @"Would you like to install them now?" 
                    : (void*)&PATCH_AVAILABLE];
    
    [txtStatus setStringValue:@"Update available"];
  } else {
    [txtStatus setStringValue:@"Application is up to date"];
    [btnLaunch setEnabled: true];
  }
}

-(void) patchStarted: (Version&) inVersion {
  [txtStatus setStringValue: 
   [NSString stringWithFormat:@"Updating to version %s", inVersion.toNumber().c_str()]];

}

-(void) patchProgress: (double)inPercent {
  [progressBar setDoubleValue: inPercent];
}

-(void) patchFailed: (NSString*)inMsg : (Version&) inVersion {

  [self showDialog:DIALOG_ERROR : @"Update failed."  : inMsg : (void*)&PATCH_FAILED];
  
}

-(void) patchComplete: (Version&) inVersion {
  [txtStatus setStringValue: 
   [NSString stringWithFormat:@"Successfully updated to version %s", inVersion.toNumber().c_str()]];
}

-(void) applicationPatched: (Version&) inVersion {
  [txtStatus setStringValue:[NSString stringWithFormat: @"Application is up to date. Version %s", inVersion.toNumber().c_str()]];
  [btnLaunch setEnabled: true];
}

-(IBAction) clickLaunch: (id) sender {
  NSLog(@"Moo");
  Pixy::Launcher::getSingleton().launchExternalApp();
}

@end