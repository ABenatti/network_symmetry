//
//  CVMotifs.c
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 18/04/14.
//  Copyright (c) 2014 Filipi Nascimento Silva. All rights reserved.
//
#include "CVNetwork.h"
#include "CVConcentricStructure.h"
#include "CVNetworkSymmetry.h"
#include <getopt.h>
#include "CVNetworkCentrality.h"
#include "CVSymmetryApplication.h"


#define CVSymmetryApplicationVersion "0.2b"


static void CVSymmetryApplicationPrintProgress(CVOperationControl* thisControl) {
	fprintf(stderr,"\r# Progress: %4"CVIntegerScan"/%"CVIntegerScan,thisControl->currentProgress,thisControl->maxProgress);
}

static void CVSymmetryApplicationPrintCentralityProgress(CVOperationControl* thisControl) {
	fprintf(stderr,"\r# Progress: %4"CVIntegerScan"/%"CVIntegerScan" (Calculating Centrality)",thisControl->currentProgress,thisControl->maxProgress);
}

static void CVSymmetryApplicationPrintStressProgress(CVOperationControl* thisControl) {
	fprintf(stderr,"\r# Progress: %4"CVIntegerScan"/%"CVIntegerScan" (Calculating Stress)",thisControl->currentProgress,thisControl->maxProgress);
}

static void CVSymmetryApplicationPrintLiveStream(CVOperationControl* thisControl, CVIndex index, const char* format, ...) {
	FILE* outputFile = thisControl->defaultStreamFile;
	if(!outputFile){
		outputFile = stdout;
	}
	fprintf(outputFile,"%"CVIntegerScan"\t",index);
	va_list args;
    va_start( args, format);
    vfprintf(outputFile, format, args);
    va_end( args );
	//fprintf(outputFile,"\n");
}

int CVSymmetryApplication(CVNetwork* theNetwork, int argc, char** argv, CVSymmetryOutputParameters** outputArray, CVInteger *level_out){
	static struct option options[] = {
		{"level",             required_argument, NULL, 'l'},
		{"path-diversity",    no_argument,       NULL, 'p'},
		{"merge-last-level",  no_argument,       NULL, 'M'},
		{"output-probs",      no_argument,       NULL, 'P'},
		{"output-reachable",  no_argument,       NULL, 'r'},
		{"output-accessed",   no_argument,       NULL, 'a'},
		{"output-degree",     no_argument,       NULL, 'd'},
		{"output-avg-degree", no_argument,       NULL, 'D'},
		{"output-cluscoeff",  no_argument,       NULL, 'C'},
		{"output-betweenness",no_argument,       NULL, 'B'},
		{"output-stress",     no_argument,       NULL, 'T'},
		{"export-csv",        no_argument,       NULL, 'c'},
		{"export-multiple",   no_argument,       NULL, 'm'},
		{"live-stream",       no_argument,       NULL, 's'},
		{"parallel-jobs",     required_argument, NULL, 'j'},
		{"verbose",           no_argument,       NULL, 'v'},
		{"quiet",             no_argument,       NULL, 'q'},
		{"show-status",       no_argument,       NULL, 'S'},
		{0, 0, 0, 0}
	};
	
	// printf("l (in 1) = %d\n", *level_out);

	int optionCode;
	int optionIndex = 0;
	
	CVInteger level = 4;
	CVBool verbose = CVFalse;
	CVBool quiet = CVFalse;
	CVBool liveStream = CVFalse;
	CVBool forceShowStatus = CVFalse;
	CVBool inputSTDIN = CVFalse;
	
	CVBool usePathDiversity = CVFalse;
	CVBool mergeLastLevel = CVFalse;
	
	CVBool outputProbabilities = CVFalse;
	CVBool outputReachable = CVFalse;
	CVBool outputAccessed = CVFalse;
	
	CVBool outputDegree = CVFalse;
	CVBool outputPatternDegree = CVFalse;
	CVBool outputClusteringCoeff = CVFalse;
	CVBool outputBetweenness = CVFalse;
	CVBool outputStress = CVFalse;
	
	CVBool exportCSV = CVFalse;
	CVBool exportMultiple = CVFalse;
	
	CVInteger maxParallelJobs = 0;
	
	CVSize filesCount = 0;
	FILE * verboseStream = NULL;
	
	const char* outputFilename = "<STDOUT>";
	FILE* networkFile = stdin;
	FILE* outputFile = stdout;
	
	while ((optionCode=getopt_long(argc, argv, "l:pPracdDCBTmMsj:hVvqSi",options, &optionIndex))>=0){
		switch (optionCode){
			case 'l':
				if(!sscanf(optarg, "%"CVIntegerScan,&level)||level<=0){
					printf("Error: Parameter \"%s\" for option --level (-l) is invalid. Use a integer > 0.\n", optarg);
					return EXIT_FAILURE;
				}
				break;
			case 's':
				liveStream = CVTrue;
				break;
			case 'a':
				outputAccessed = CVTrue;
				break;
			case 'p':
				usePathDiversity = CVTrue;
				break;
			case 'M':
				mergeLastLevel = CVTrue;
				break;
			case 'P':
				outputProbabilities = CVTrue;
				break;
			case 'd':
				outputDegree = CVTrue;
				break;
			case 'D':
				outputPatternDegree = CVTrue;
				break;
			case 'B':
				outputBetweenness = CVTrue;
			case 'T':
				outputStress = CVTrue;
				break;
			case 'C':
				outputClusteringCoeff = CVTrue;
				break;
			case 'r':
				outputReachable = CVTrue;
				break;
			case 'c':
				exportCSV = CVTrue;
				break;
			case 'm':
				exportMultiple = CVTrue;
				break;
			case 'j':
				if(!sscanf(optarg, "%"CVIntegerScan,&maxParallelJobs)||maxParallelJobs<=0){
					printf("Error: Parameter \"%s\" for option --parallel-jobs (-j) is invalid. Use a integer > 0.\n",optarg);
					return EXIT_FAILURE;
				}
				break;
			case 'v':
				verbose = CVTrue;
				break;
			case 'q':
				quiet = CVTrue;
				break;
			case 'S':
				forceShowStatus = CVTrue;
				break;
			case 'i':
				inputSTDIN = CVTrue;
				break;
			default:
				return EXIT_FAILURE;
		}
	}
	
	if(exportCSV && exportMultiple){
		printf("Error: Export to CSV and export to multiple files can not be used at same time.\n");
		return EXIT_FAILURE;
		
	}
	
	verboseStream = stderr;
	filesCount = argc-optind;
	
	if(inputSTDIN){
		if(filesCount>1){
			printf("Error: Found extra %"CVSizeScan" file parameters, please provide only the output file if --input-stdin is enabled.\n", filesCount- 1);
			return EXIT_FAILURE;
		}else if(filesCount>0){
			outputFilename = argv[optind];
			outputFile = fopen(outputFilename, "w");
		}
	}else{
		if(filesCount > 1){
			printf("Error: Found extra %"CVSizeScan" file parameters, please provide only one filename.\n", filesCount - 2);
			return EXIT_FAILURE;
		}else{// filesCount == 1
			outputFilename = argv[optind];
			outputFile = fopen(outputFilename, "w");
		}
	}
	
	if(quiet || outputFile==stdout){
		verboseStream = stderr;
	}else if(verbose){
		verboseStream = stdout;
	}
	
	if(liveStream && outputFile==stdout){
		quiet = CVTrue;
	}
	
	if(forceShowStatus){
		quiet = CVFalse;
	}
	
	
	if(!outputFile){
		fprintf(stderr, "Error: %s: No such file or directory, or user does not have write permissions.\n",outputFilename);
		return EXIT_FAILURE;
	}
	
	if(verbose){
		fprintf(verboseStream,"# Output File: %s\n",outputFilename);
		fprintf(verboseStream,"# Parsing network file.\n");
	}
	// CVNetwork* theNetwork = CVNewNetworkFromXNETFile(networkFile);
	
	if(!theNetwork||theNetwork->verticesCount==0){
		fprintf(stderr, "Error: Network is malformed. \n");
		return EXIT_FAILURE;
	}
	
	
	CVOperationControl* operationControl = CVOperationControlCreate();
	
	if(maxParallelJobs>0){
		operationControl->maxParallelBlocks = maxParallelJobs;
	}
	
	if(!quiet){
		operationControl->updateCallback = CVSymmetryApplicationPrintProgress;
	}
	
	if(liveStream){
		operationControl->streamCallback = CVSymmetryApplicationPrintLiveStream;
		operationControl->defaultStreamFile = outputFile;
	}
	
	if(verbose){
		fprintf(verboseStream,"# Begining symmetric accessibility calculation with parameters:\n");
		fprintf(verboseStream,"#   Network: \n");
		fprintf(verboseStream,"#      Vertices: %"CVIntegerScan"\n",theNetwork->verticesCount);
		fprintf(verboseStream,"#      Edges: %"CVIntegerScan"\n",theNetwork->edgesCount);
		fprintf(verboseStream,"#      Type: ");
		fprintf(verboseStream,"%s",theNetwork->directed?"<Directed> ":"<Undirected> ");
		fprintf(verboseStream,"%s",theNetwork->vertexWeighted?"<Vertex Weighted> ":"");
		fprintf(verboseStream,"%s",theNetwork->edgeWeighted?"<Edge Weighted> ":"");
		fprintf(verboseStream,"\n");
		fprintf(verboseStream,"#   Level: %"CVIntegerScan"\n", level);
		fprintf(verboseStream,"#   Live Stream: %s\n", liveStream?"yes":"no");
		fprintf(verboseStream,"#\n# Starting Calculations now...\n");
	}
	
	CVFloatArray centrality;
	if(outputBetweenness){
		CVFloatArrayInitWithCapacity(theNetwork->verticesCount, &centrality);
		CVOperationControl* centralityOperationControl = CVOperationControlCreate();
		
		if(maxParallelJobs>0){
			centralityOperationControl->maxParallelBlocks = maxParallelJobs;
		}

		if(verbose){
			fprintf(verboseStream,"#\n# Calculating Centrality now...\n");
		}
		
		if(!quiet){
			centralityOperationControl->updateCallback = CVSymmetryApplicationPrintCentralityProgress;
		}
		
		CVNetworkCalculateCentrality(theNetwork, &centrality, centralityOperationControl);
		
		CVOperationControlDestroy(centralityOperationControl);
		if(!quiet){
			fprintf(stderr,"\n");
		}
	}
	CVFloatArray stress;
	if(outputStress){
		CVFloatArrayInitWithCapacity(theNetwork->verticesCount, &stress);
		CVOperationControl* stressOperationControl = CVOperationControlCreate();
		
		if(maxParallelJobs>0){
			stressOperationControl->maxParallelBlocks = maxParallelJobs;
		}
		
		if(verbose){
			fprintf(verboseStream,"#\n# Calculating Stress Centrality now...\n");
		}
		
		if(!quiet){
			stressOperationControl->updateCallback = CVSymmetryApplicationPrintStressProgress;
		}
		
		CVNetworkCalculateStressCentrality(theNetwork, &stress, stressOperationControl);
		
		CVOperationControlDestroy(stressOperationControl);
		
		if(!quiet){
			fprintf(stderr,"\n");
		}
		
	}
	
	if(verbose){
		fprintf(verboseStream,"#\n# Calculating Symmetry now...\n");
	}
	
	CVNetworkCalculateSymmetry(theNetwork, level, (outputAccessed||outputReachable)?CVTrue:CVFalse, (outputProbabilities)?CVTrue:CVFalse,mergeLastLevel, outputArray, operationControl);
	
	if(!quiet){
		fprintf(stderr,"\n");
	}
	if(verbose){
		fprintf(verboseStream,"# Finished accessibility calculation%s\n",liveStream?".":", now exporting to output...");
	}
	
	
	CVIndex i;
	if(!liveStream){
		if(!exportCSV&&!exportMultiple){
			CVIndex l;
			for (l=2; l<=level; l++) {
				fprintf(outputFile, "#v \"Backbone Symmetry h=%"CVIndexScan"\" n\n",l);
				for(i=0;i<theNetwork->verticesCount;i++){
					CVSymmetryOutputParameters* vertexOutput = outputArray[i];
					fprintf(outputFile,"%.6g\n",vertexOutput->normalizedBackboneAccessibility[l]);
				}
				
				fprintf(outputFile, "#v \"Merged Symmetry h=%"CVIndexScan"\" n\n",l);
				for(i=0;i<theNetwork->verticesCount;i++){
					CVSymmetryOutputParameters* vertexOutput = outputArray[i];
					fprintf(outputFile,"%.6g\n",vertexOutput->normalizedMergedAccessibility[l]);
				}
				if(outputAccessed){
					fprintf(outputFile, "#v \"Accessibility h=%"CVIndexScan"\" n\n",l);
					for(i=0;i<theNetwork->verticesCount;i++){
						CVSymmetryOutputParameters* vertexOutput = outputArray[i];
						fprintf(outputFile,"%.6g\n",vertexOutput->backboneAccessibility[l]);
					}
				}
				if(outputReachable){
					fprintf(outputFile, "#v \"Reachable Nodes h=%"CVIndexScan"\" n\n",l);
					for(i=0;i<theNetwork->verticesCount;i++){
						CVSymmetryOutputParameters* vertexOutput = outputArray[i];
						fprintf(outputFile,"%"CVSizeScan"\n",vertexOutput->accessedVertices[l]);
					}
				}
				
				if(outputProbabilities){
					fprintf(outputFile, "#v \"Backbone Probabilities h=%"CVIndexScan"\" s\n",l);
					for(i=0;i<theNetwork->verticesCount;i++){
						CVSymmetryOutputParameters* vertexOutput = outputArray[i];
						CVIndex concentricIndex;
						fprintf(outputFile,"\"");
						for (concentricIndex=0; concentricIndex<vertexOutput->accessedVertices[l]; concentricIndex++) {
							if(concentricIndex){
								fprintf(outputFile,", ");
							}
							fprintf(outputFile,"\"%"CVSizeScan":%.6g\"",vertexOutput->lastLevelIndices[l].data[concentricIndex], vertexOutput->backboneProbabilities[l].data[concentricIndex]);
						}
						fprintf(outputFile,"\"\n");
					}
					fprintf(outputFile, "#v \"Merged Probabilities h=%"CVIndexScan"\" s\n",l);
					for(i=0;i<theNetwork->verticesCount;i++){
						CVSymmetryOutputParameters* vertexOutput = outputArray[i];
						CVIndex concentricIndex;
						fprintf(outputFile,"\"");
						for (concentricIndex=0; concentricIndex<vertexOutput->accessedVertices[l]; concentricIndex++) {
							if(concentricIndex){
								fprintf(outputFile,", ");
							}
							fprintf(outputFile,"\"%"CVSizeScan":%.6g\"",vertexOutput->lastLevelIndices[l].data[concentricIndex], vertexOutput->mergedProbabilities[l].data[concentricIndex]);
						}
						fprintf(outputFile,"\"\n");
					}
				}
				
				
				if(outputBetweenness){
					fprintf(outputFile, "#v \"Betweenness Centrality\"\n");
					for(i=0;i<theNetwork->verticesCount;i++){
						fprintf(outputFile, "%.6g\n",centrality.data[i]);
					}
				}
				
				if(outputStress){
					fprintf(outputFile, "#v \"Stress Centrality\"\n");
					for(i=0;i<theNetwork->verticesCount;i++){
						fprintf(outputFile, "%.6g\n",stress.data[i]);
					}
				}
				
				if(outputDegree){
					fprintf(outputFile, "#v \"Node Degree\"\n");
					for(i=0;i<theNetwork->verticesCount;i++){
						fprintf(outputFile, "%"CVSizeScan"\n",theNetwork->vertexNumOfEdges[i]);
					}
				}
				
				if(outputClusteringCoeff){
					fprintf(outputFile, "#v \"Clustering Coefficient\"\n");
					for(i=0;i<theNetwork->verticesCount;i++){
						fprintf(outputFile, "%.6g\n",CVNetworkClusteringCoefficient(theNetwork,i));
					}
				}
				
				if(outputPatternDegree){
					fprintf(outputFile, "#v \"Motif Average Degree\"\n");
					for(i=0;i<theNetwork->verticesCount;i++){
						fprintf(outputFile, "1.0f\n");
					}
				}
				
			}
		}else if(exportCSV){
			CVIndex l;
			for (l=2; l<=level; l++) {
				if(l>2){
					fprintf(outputFile,"\t");
				}
				
				fprintf(outputFile, "Backbone Symmetry h=%"CVIndexScan"\t",l);
				fprintf(outputFile, "Merged Symmetry h=%"CVIndexScan,l);
				
				if(outputAccessed){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "Accessed Nodes h=%"CVIndexScan,l);
				}
				if(outputReachable){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "Reachable Nodes h=%"CVIndexScan,l);
				}
				
				if(outputProbabilities){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "Backbone Probabilities h=%"CVIndexScan,l);
					fprintf(outputFile,"\t");
					fprintf(outputFile, "Merged Probabilities h=%"CVIndexScan,l);
				}
			}
			
			if(outputBetweenness){
				fprintf(outputFile,"\t");
				fprintf(outputFile, "Betweenness Centrality");
			}
			
			if(outputStress){
				fprintf(outputFile,"\t");
				fprintf(outputFile, "Stress Centrality");
			}
			
			if(outputDegree){
				fprintf(outputFile,"\t");
				fprintf(outputFile, "Node Degree");
			}
			
			if(outputClusteringCoeff){
				fprintf(outputFile,"\t");
				fprintf(outputFile, "Clustering Coefficient");
			}
			
			if(outputPatternDegree){
				fprintf(outputFile,"\t");
				fprintf(outputFile, "Motif Average Degree");
			}
			
			fprintf(outputFile,"\n");
			
			
			for(i=0;i<theNetwork->verticesCount;i++){
				CVSymmetryOutputParameters* vertexOutput = outputArray[i];
				for (l=2; l<=level; l++) {
					if(l>2){
						fprintf(outputFile,"\t");
					}
					fprintf(outputFile,"%.6g\t",vertexOutput->normalizedBackboneAccessibility[l]);
					fprintf(outputFile,"%.6g",vertexOutput->normalizedMergedAccessibility[l]);
					if(outputAccessed){
						fprintf(outputFile,"\t");
						fprintf(outputFile,"%.6g",vertexOutput->backboneAccessibility[l]);
					}
					if(outputReachable){
						fprintf(outputFile,"\t");
						fprintf(outputFile,"%"CVSizeScan,vertexOutput->accessedVertices[l]);
					}
					
					if(outputProbabilities){
						fprintf(outputFile,"\t\"");
						CVIndex concentricIndex;
						//fprintf(outputFile,"");
						for (concentricIndex=0; concentricIndex<vertexOutput->accessedVertices[l]; concentricIndex++) {
							if(concentricIndex){
								fprintf(outputFile,", ");
							}
							fprintf(outputFile,"%"CVSizeScan":%.6g",vertexOutput->lastLevelIndices[l].data[concentricIndex], vertexOutput->backboneProbabilities[l].data[concentricIndex]);
						}
						fprintf(outputFile,"\"\t\"");
						//fprintf(outputFile,"");
						for (concentricIndex=0; concentricIndex<vertexOutput->mergedAccessedVertices[l]; concentricIndex++) {
							if(concentricIndex){
								fprintf(outputFile,", ");
							}
							fprintf(outputFile,"%"CVSizeScan":%.6g",vertexOutput->lastLevelIndices[l].data[concentricIndex], vertexOutput->mergedProbabilities[l].data[concentricIndex]);
						}
						fprintf(outputFile,"\"");
					}
				}
				
				if(outputBetweenness){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "%.6g",centrality.data[i]);
				}
				
				if(outputStress){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "%.6g",stress.data[i]);
				}
				
				if(outputDegree){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "%"CVSizeScan,theNetwork->vertexNumOfEdges[i]);
				}
				
				if(outputClusteringCoeff){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "%.6g",CVNetworkClusteringCoefficient(theNetwork,i));
				}
				
				if(outputPatternDegree){
					fprintf(outputFile,"\t");
					fprintf(outputFile, "1.0f");
				}
				
				fprintf(outputFile,"\n");
			}
		}
	}
	
	if(verbose){
		fprintf(verboseStream,"# Cleaning Up...\n");
	}
	
	if(verbose){
		fprintf(verboseStream,"# Closing files...\n");
	}
	if(outputFile!=stdout){
		fclose(outputFile);
	}
	if(networkFile!=stdin){
		fclose(networkFile);
	}
	if(verbose){
		fprintf(verboseStream,"# Done!\n");
	}

	*level_out = level;
	return EXIT_SUCCESS;
}
