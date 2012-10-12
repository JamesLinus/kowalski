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

#import "RootViewController.h"

#import "DemoViewController.h"

#import "DemoBase.h"
#import "PianoDemo.h"

@implementation RootViewController

@synthesize aboutViewController;

#pragma mark -
#pragma mark View lifecycle


- (void)viewDidLoad {
    [super viewDidLoad];
    [self createDemos];
}

- (void)createDemos {

    demos = [[NSMutableArray alloc] initWithCapacity:10];
    
    [self addDemo:@"AudioMeteringDemo"];
    [self addDemo:@"AudioTaperDemo"];
    [self addDemo:@"BalanceAndPanDemo"];
    [self addDemo:@"ComplexSoundsDemo"];
    [self addDemo:@"ConeAttenuationDemo"];
    [self addDemo:@"DecodersDemo"];
    [self addDemo:@"DopplerShiftDemo"];
    [self addDemo:@"DSPDemo"];
    [self addDemo:@"EmitterSwarmDemo"];
    [self addDemo:@"EventCallbackDemo"];
    [self addDemo:@"FreeformEventsDemo"];
    [self addDemo:@"InputDemo"];
    [self addDemo:@"MixPresetDemo"];
    [self addDemo:@"PianoDemo"];
    [self addDemo:@"SafeWaveBankUnloadingDemo"];
    [self addDemo:@"SampleClockDemo"];
    [self addDemo:@"SeamlessPitchDemo"];
    
}

-(void)addDemo:(NSString*)name {
    NSArray* nibViews = [[NSBundle mainBundle] loadNibNamed:name
                                                      owner:self
                                                    options:nil];
    
    DemoBase* demo = [nibViews objectAtIndex:0];
    [demos addObject:demo];
}

#pragma mark -
#pragma mark Table view data source

// Customize the number of sections in the table view.
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    
    if (section == DEMO_SECTION) {
        return [demos count];
    }
    else if (section == ABOUT_SECTION) {
        return 1;
    }
        
    return 0;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	
    // Configure the cell.
    if (indexPath.section == DEMO_SECTION) {
        int demoIdx = indexPath.row;
        assert(demoIdx >= 0 && demoIdx < [demos count]);
        
        DemoBase* demo = [demos objectAtIndex:demoIdx];
        
        cell.textLabel.text = [demo getName];
    }
    else if (indexPath.section == ABOUT_SECTION) {
        cell.textLabel.text = @"About";
    }
    
    

    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    
    if(section == DEMO_SECTION) {
        return @"Demos";
    }
    
    return nil;
}


#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (indexPath.section == DEMO_SECTION)
    {
        int demoIdx = indexPath.row;
        assert(demoIdx >= 0 && demoIdx < [demos count]);
        
        DemoViewController* demoController = [[DemoViewController alloc] initWithDemo:[demos objectAtIndex:demoIdx]];
        [self.navigationController pushViewController:demoController animated:YES];
        [demoController release];
        
    }
    else if (indexPath.section == ABOUT_SECTION)
    {
        [self.navigationController pushViewController:aboutViewController animated:YES];
    }
}


#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
    
    for (id obj in demos)
    {
        [obj release];
    }
    
    [demos release];
}


@end

