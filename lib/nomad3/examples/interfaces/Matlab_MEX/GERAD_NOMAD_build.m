%% GERAD NOMAD Build for Matlab

% This file will help you compile NOMAD for use with MATLAB. 

% To recompile you will need to get / do the following:

% 1) Get NOMAD
% NOMAD is available from https://www.gerad.ca/nomad
% Complete the download form then download the latest version. Define the
% $NOMAD_HOME environment variable.

% 2) Start Matlab and go into $NOMAD_HOME/examples/interfaces/Matlab_Mex
% The NOMAD MEX Interface is a simple MEX interface written to use NOMAD.

% 3) Compile the MEX File by executing this file in Matlab
%
%
% The code below will build the NOMAD MEX file and set the Matlab path. 

clear nomad

% Current directory
cdir = cd;

% Check and set nomad_home and create variables for path
clear nomad_home nomad_src nomad_src_sgtelib;

% Default values
nameLibNomad = '';
updateLDFLAGS= '';
install_name_tool='';

if ( strcmp(computer,'PCWIN64') == 1 || strcmp(computer,'PCWIN32') == 1 )

    nomad_local_home = getenv('NOMAD_HOME_PERSONAL');

    if ( length(nomad_local_home) > 1)
        warning('The NOMAD_HOME_PERSONAL variable for Matlab is set to %s. The default can be replaced by using the command setenv(''NOMAD_HOME_PERSONAL'',ARG1)! before running the GERAD_NOMAD_build command.',nomad_local_home);
        if ( ~isempty( find(isspace(nomad_local_home),1) ) )
            error('The compilation of Nomad for Matlab must be performed in the NOMAD_HOME_PERSONAL directory. The NOMAD_HOME_PERSONAL directory should not contain empty space. Please consider moving the NOMAD_HOME_PERSONAL directory and reset the NOMAD_HOME_PERSONAL environment variable accordingly.');
        end
        
    else
        cd ..
        cd .. 
        cd ..  
        nomad_local_home = cd; 
        

    end
    nomad_src=[nomad_local_home filesep 'src' filesep];
    nomad_src_sgtelib=[nomad_local_home filesep 'ext\sgtelib\src' filesep ];
    nomad_bin=[nomad_local_home filesep 'bin' filesep];
    nomad_lib= nomad_bin;
    if ( ~ exist(nomad_src,'dir') )
            error('The default NOMAD_HOME_PERSONAL directory does not exist. Please make sure that it exists.');
    end
    
    % Return to base dir
    cd(cdir);
    
    % Nomad library name
    nameLibNomad = 'nomad.lib';
    tmpMexLibName = 'nomad_tmp';
    finalMexLibName = 'nomad';
    
    %Compile & Move (Windows) ---> recompile Nomad and sgtelib
    post = [' -I.  -I' nomad_src ' -I' nomad_src_sgtelib ' -L' nomad_lib ' -lut -output ' nomad_bin filesep tmpMexLibName '.' mexext];
    pre = ['mex -v -largeArrayDims nomadmex.cpp ' nameLibNomad ];
        
else
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % LINUX AND OSX  ---> use dynamic libraries
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % Default library names
    nameLibNomad = 'libnomad.so';

    % Default update LDFLAGS (linux only)
    updateLDFLAGS= '';
    % Post compilation tool for path to library (osx only)
    install_name_tool='';

    nomad_local_home = getenv('NOMAD_HOME');

    if ( length(nomad_local_home) < 1 )
        % Get a default directory for NOMAD_HOME
        cd ..
        cd .. 
        cd ..
        nomad_local_home = cd; 
        if ( ~ exist(nomad_local_home,'dir') )
            error('The default NOMAD_HOME directory does not exist. Please provide a correct value for the NOMAD_HOME variables with the command setenv(''NOMAD_HOME'',ARG1)');
        end
        warning('The NOMAD_HOME variable for Matlab is set with its default value %s. The default can be replaced by using the command setenv(''NOMAD_HOME'',ARG1)! before running the GERAD_NOMAD_build command.',nomad_home);
        cd(cdir);
    else
        if ( ~isempty( find(isspace(nomad_local_home),1) ) )
            error('The compilation of Nomad for Matlab uses the sources located in the NOMAD_HOME directory. The NOMAD_HOME directory should not contain empty space. Please consider moving the NOMAD_HOME directory and reset the NOMAD_HOME environment variable accordingly.');
        end
    end
      
    nomad_src=[nomad_local_home filesep 'src' filesep];
    sgtelib_src=[nomad_local_home filesep 'ext' filesep 'sgtelib' filesep 'src'];
    nomad_lib=[nomad_local_home filesep 'lib' filesep];
    nomad_bin=[nomad_local_home filesep 'bin' filesep]; 
    
    switch(computer)
        case 'GLNX86'
            updateLDFLAGS = 'LDFLAGS=''$LDFLAGS -Wl,-rpath,''''$ORIGIN/../lib/'''' -Wl,-rpath-link,''''../lib/'''' '' ';
        case 'GLNXA64'
            updateLDFLAGS = 'LDFLAGS=''$LDFLAGS -Wl,-rpath,''''$ORIGIN/../lib/'''' -Wl,-rpath-link,''''../lib/'''' '' ';
        case 'MACI64'
            install_name_tool=['install_name_tool -change ' nameLibNomad ' @loader_path/../lib/' nameLibNomad ' ' nomad_bin filesep 'nomad.' mexext];
    end
   
    %Compile & Move (Default) --> use shared object library
    post = [' -I.  -I' nomad_src ' -I' sgtelib_src ' -lut -lnomad -L' nomad_lib ' -output ' nomad_bin filesep 'nomad.' mexext ];
    pre =[ 'mex -g -v -largeArrayDims nomadmex.cpp ' updateLDFLAGS ];
    
    if ( ~ exist([nomad_lib filesep nameLibNomad],'file') )
        error('The Nomad library file %s is not available. Please perform Nomad project compilation before proceeding.',nameLibNomad);      
    end
    
end
    

fprintf('\n------------------------------------------------\n');
fprintf('NOMAD MEX FILE BUILD --- GERAD VERSION \n\n');

%CD to Source Directory
cd 'Source';

try

    if ( ~ exist([nomad_lib filesep nameLibNomad],'file') )
        error('The Nomad library file %s is not available. Please perform Nomad project compilation before proceeding.',nameLibNomad);      
    end

    eval([pre post])
    
    if ( strcmp(computer,'MACI64') == 1 )
        system(install_name_tool);
    end
    
    if ( strcmp(computer,'PCWIN64') == 1 || strcmp(computer,'PCWIN32') == 1 )
        movefile([nomad_bin filesep tmpMexLibName '.' mexext],[nomad_lib filesep finalMexLibName '.' mexext]);
    end

    
    cd(cdir);
    fprintf('Compilation done!\n');
    fprintf('\n----------------------------------------------------------------------------------------------\n');
    fprintf(' To be able to use the nomad functions, you may need to modify the Matlab path \n');
    qstring = 'To be able to use the nomad functions, you may need to modify the Matlab path. Do you want to update the Matlab path?';
    choice = questdlg(qstring,'Set path','Yes','No','Yes');
    if ( strcmp(choice,'Yes') )
        addpath([ cdir filesep 'Functions']);
        addpath(nomad_bin);
        addpath(nomad_lib);
        fprintf('  ---> The Matlab path has been modified but not saved.\n');
    end
    clear nomad_home nomad_lib nomad_bin nomad_src nomad_src_sgtelib cdir post pre updateLDFLAGS qstring choice install_name_tool nameLibNomad;
catch ME
    cd(cdir);
	clear nomad_home nomad_lib nomad_bin nomad_src nomad_src_sgtelib cdir post pre updateLDFLAGS qstring choice install_name_tool nameLibNomad;
    error('Error Compiling NOMAD!\n%s',ME.message);
end
