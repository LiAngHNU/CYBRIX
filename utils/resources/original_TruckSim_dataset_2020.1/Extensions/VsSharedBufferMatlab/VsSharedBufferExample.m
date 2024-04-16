% This example uses the VS Shared Buffer mex file to lock the "Left" camera buffer
% and take thousands of samples (configurable below), then create a surface plot
% of the sampled data using the color samples to color the plot.

% Close any open connections
VsSharedBufferMex close

% Connect & lock the left buffer
VsSharedBufferMex connect EBS

% setup loop constants
aspectRatio = 1.6; % example sensor buffer is 320x200.
loopStepsEle = int32(64);
loopStepsAzi = int32(loopStepsEle * aspectRatio);
elRange = 0.35; % radians for elevation scan.
azRange = elRange * aspectRatio;
halfStepAzi = double(loopStepsAzi / 2.0);
halfStepEle = double(loopStepsEle / 2.0);
azHalf = azRange / 2.0;
elHalf = elRange / 2.0;
invalidDepth = -3e+37; % VSSB invalid is -3.402823466385289e+38
defaultPlotFloor = 0;

% pre-allocate big buffers gathered in loop.
depth = zeros(loopStepsAzi, loopStepsEle);
color = zeros(loopStepsAzi, loopStepsEle, 3);

% sample data
VsSharedBufferMex lock EBS
for ia = 1:loopStepsAzi
    for ie = 1:loopStepsEle
        aPortion = (double(ia) - halfStepAzi) * (1.0 / halfStepAzi);
        ePortion = (double(ie) - halfStepEle) * (1.0 / halfStepEle);
        azimuth = azHalf * aPortion;
        elevation = elHalf * ePortion;
        [frame, frameT, bColor, bDepth, bNormal, color(ia, ie, :), depth(ia, ie), normal, camOrigin, intersect] = VsSharedBufferMex('query', 'EBS', azimuth, elevation);
        % Clamp out-of-range samples to the default floor, else the
        % plot will be deranged.
        if (depth(ia, ie) <= invalidDepth)
            depth(ia, ie) = defaultPlotFloor;
        end
    end
end
VsSharedBufferMex unlock EBS

% Create surface plot with sampled depth, colored with sampled color data.
hChart = surf(depth, color);
title('Depth and Color samples taken from Shared Buffer');
xlabel('Elevation Samples');
ylabel('Azimuth Samples');
zlabel('Distance from Sensor (m)');
set(gca, 'zdir', 'reverse');
set(gca, 'ZScale', 'log');

% cleanup
VsSharedBufferMex close EBS
