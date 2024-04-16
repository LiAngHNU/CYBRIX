% Launch TruckSim Server and go to the Home screen (Run Control)
h = actxserver('TruckSim.Application');
h.GoHome()

% h.invoke() % Display methods (comment out if not wanted)

% Look for dataset "Baseline COM" in category "External Control of Runs"
ok = h.DataSetExists('','Baseline COM','External Control of Runs');
if ~ok
  return;  % Error: Dataset does not exist so stop
end

% delete new dataset if it already exists to avoid a name conflict
h.DeleteDataSet('','New Run Made with COM','External Control of Runs')

% Make a new dataset based on the baseline COM
h.Gotolibrary('','Baseline COM','External Control of Runs')
h.CreateNew() % duplicate the baseline and rename
h.DatasetCategory('New Run Made with COM','External Control of Runs')

% Set up run control to use a different speed as in the Quick Start Guide
h.Checkbox('#Checkbox8','1') % enable checkbox to show more options
h.Checkbox('#Checkbox3','1') % enable checkbox to show driver controls
h.Ring('#RingCtrl0','1') % select constant target speed
h.Yellow('*SPEED', '90') % set the target speed

% Run the Simulation and view results
h.Run('','')
h.Checkbox('#Checkbox1','0') % disable overlays
h.LaunchPlot()

%h.release % Release the server (comment out to keep active)