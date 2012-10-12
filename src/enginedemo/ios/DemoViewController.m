/*
 Copyright (c) 2010-2012 Per Gantelius
 
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source
 distribution.
 */

#import "DemoViewController.h"

const float timeStep = 0.02f;

@implementation DemoViewController

-(id)initWithDemo:(DemoBase*)d {
    self = [super initWithNibName:nil
                           bundle:nil];
    
    demo = d;

    //put the demo in a scroll view
    CGRect demoFrame = demo.frame;
    CGRect screenBounds = [UIScreen mainScreen].bounds;
    UIScrollView* sw = [[UIScrollView alloc] initWithFrame:screenBounds];
    sw.contentSize = CGSizeMake(demoFrame.size.width, 
                                demoFrame.size.height);
    [sw addSubview:demo];
    
    self.view = sw;
    [sw release];
    
    //fire up an update timer.
    updateTimer = [NSTimer scheduledTimerWithTimeInterval:timeStep
                                                   target:self
                                                 selector:@selector(update)
                                                 userInfo:nil
                                                  repeats:YES];
    
    return self;
}

-(void)viewWillAppear:(BOOL)animated {
    
    [super viewWillAppear:animated];
    
    //the same engine data is being used in all the demos for now
    kwlEngineDataLoad([DemoBase getResourcePath:@"demoproject.kwl"]);
    [demo initialize];
    kwlError error = kwlGetError();
    assert(error == KWL_NO_ERROR);
    
}

-(void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    [updateTimer invalidate];
    
    [demo deinitialize];
    kwlEngineDataUnload();
    kwlError error = kwlGetError();
    assert(error == KWL_NO_ERROR);
}

-(void)viewDidDisappear:(BOOL)animated
{
    self.view = nil;
}

-(void)update {

    kwlUpdate(timeStep);
    [demo update:timeStep];
    kwlError error = kwlGetError();
    assert(error == KWL_NO_ERROR);
}

-(void)dealloc {
    [super dealloc];
    
    
}

@end
