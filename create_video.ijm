inputDir = getDirectory('Choose Input Directory'); 
outputDir = getDirectory('Choose Output Directory'); 

listdir = getFileList(inputDir);

for(i=0; i< listdir.length; i++) {
    print("Processing: " + inputDir + listdir[i]);
    //File.makeDirectory(outputDir +listdir[i]);
    inputFolder = inputDir + listdir[i];
    outputFolder = outputDir + listdir[i];
    //inputFiles = getFileList(inputFolder);
    //inputFile = inputFolder + inputFiles[0];
    
    
    File.openSequence(inputFolder, "virtual");
    run("AVI... ", "compression=JPEG frame=50 save=[outputFolder]");    
    close();
}



