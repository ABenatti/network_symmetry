//
//  CVNetwork.c
//
//
//  Created by Filipi Nascimento Silva on 9/22/12.
//
//
#include "CVNetwork.h"
#include "uthash.h"
#include "CVGridUtilities.h"


CVBool CVNetworkAddNewEdges(CVNetwork* network, CVIndex* fromIndices, CVIndex* toIndices, CVFloat* weights, CVSize count){
	CVIndex initialEdge = network->edgesCount;
	CVSize verticesCount = network->verticesCount;
	CVIndex i;

	CVNetworkGrowEdgesCount(network,count);

	for(i=0; i<count; i++){
		CVIndex toIndex   = toIndices[i];
		CVIndex fromIndex = fromIndices[i];
		if (toIndex >= verticesCount||fromIndex >= verticesCount) {
			return CVFalse;
		}
		network->edgeFromList[i+initialEdge]=fromIndex;
		network->edgeToList[i+initialEdge]=toIndex;

		CVNetworkGrowVertexSetEdgeForVertex(network,i+initialEdge,fromIndex,toIndex);

		if(network->edgeWeighted&&weights==NULL){
			network->edgesWeights[i+initialEdge]=1.0f;
		}else if(network->edgeWeighted){
			network->edgesWeights[i+initialEdge]=weights[i];
		}
		if(!network->directed){
			CVNetworkGrowVertexSetEdgeForVertex(network,i+initialEdge,toIndex,fromIndex);
		}else{
			CVNetworkGrowVertexSetInEdgeForVertex(network,i+initialEdge,toIndex,fromIndex);
		}
	}
	return CVTrue;
}



void CVNetworkPrint(const CVNetwork* network){
	printf("Vertices:" "%" CVSizeScan "\n", network->verticesCount);
	printf("Edges: " "%" CVSizeScan "\n", network->edgesCount);
	CVIndex fromVertex;
	for (fromVertex=0; fromVertex<=network->verticesCount; fromVertex++) {
		CVIndex i;
		CVSize toVerticesCount = network->vertexNumOfEdges[fromVertex];
		CVIndex* toVertices = network->vertexEdgesLists[fromVertex];
		printf("%"CVIndexScan"\t:",fromVertex);
		for (i=0; i<toVerticesCount; i++) {
			CVIndex toVertex = toVertices[i];
			printf("\t" "%"CVIndexScan,toVertex);
		}
		printf("\n");
	}
}

CVNetwork* CV_NewAllocationNetwork(CVSize verticesCount){
	CVNetwork* newNetwork = malloc(sizeof(CVNetwork));
	newNetwork->vertexNumOfEdges = calloc(verticesCount, sizeof(CVSize));
	newNetwork->vertexCapacityOfEdges = calloc(verticesCount, sizeof(CVSize));
	newNetwork->vertexEdgesLists = calloc(verticesCount, sizeof(CVSize*));
	newNetwork->vertexEdgesIndices = calloc(verticesCount, sizeof(CVSize*));

	newNetwork->vertexNumOfInEdges = calloc(verticesCount, sizeof(CVSize));
	newNetwork->vertexCapacityOfInEdges = calloc(verticesCount, sizeof(CVSize));
	newNetwork->vertexInEdgesLists = calloc(verticesCount, sizeof(CVSize*));
	newNetwork->vertexInEdgesIndices = calloc(verticesCount, sizeof(CVSize*));
	newNetwork->verticesWeights = calloc(verticesCount, sizeof(CVFloat));
	newNetwork->verticesEnabled = calloc(verticesCount, sizeof(CVBool));
	CVIndex i;
	for(i=0;i<verticesCount;i++){
		newNetwork->verticesWeights[i] = 1.0f;
		newNetwork->verticesEnabled[i] = CVTrue;
	}

	newNetwork->edgeFromList = NULL;
	newNetwork->edgeToList = NULL;

	newNetwork->edgesWeights = NULL;

	newNetwork->vertexCapacity = verticesCount;
	newNetwork->edgesCapacity = 0;

	newNetwork->edgesCount = 0;
	newNetwork->verticesCount = verticesCount;

	newNetwork->editable = CVFalse;
	newNetwork->directed = CVFalse;
	newNetwork->edgeWeighted = CVFalse;
	newNetwork->vertexWeighted = CVFalse;

	newNetwork->vertexNames = NULL;
	newNetwork->propertiesData = NULL;
	newNetwork->propertiesTypes = NULL;
	newNetwork->propertiesCount = 0;
	newNetwork->propertiesNames = NULL;
	
	return newNetwork;
}



void CV_NetworkDestroyProperties(CVNetwork* theNetwork){
	CVIndex i;
	for (i=0; i<theNetwork->propertiesCount; i++) {
		CVPropertyType type = theNetwork->propertiesTypes[i];
		
		if(type==CVStringPropertyType){
			CVIndex j;
			CVString* values = theNetwork->propertiesData[i];
			for (j=0; j<theNetwork->verticesCount; j++) {
				free(values[j]);
			}
		}
		free(theNetwork->propertiesData[i]);
		free(theNetwork->propertiesNames[i]);
	}
	free(theNetwork->propertiesData);
	free(theNetwork->propertiesNames);
	free(theNetwork->propertiesTypes);
	theNetwork->propertiesCount = 0;
	theNetwork->propertiesData= NULL;
	theNetwork->propertiesNames= NULL;
	theNetwork->propertiesTypes= NULL;
}

void CV_NetworkDestroyNames(CVNetwork* theNetwork){
	CVIndex i;
	if(theNetwork->vertexNames){
		for (i=0; i<theNetwork->verticesCount; i++) {
			free(theNetwork->vertexNames[i]);
		}
		free(theNetwork->vertexNames);
		theNetwork->vertexNames = NULL;
	}
}

void CVNetworkAppendProperty(CVNetwork* theNetwork, CVString name, CVPropertyType type, void* data){
	CVIndex newIndex = 0;
	CVBool isNameProperty = CVFalse;
	
	if(strcmp(name, "name")==0){
		if(type==CVStringPropertyType){
			isNameProperty = CVTrue;
		}else{
			return;
			//ERROR DO NOTHING
		}
	}
	
	CVIndex currentPropertyIndex;
	void* currentData = NULL;
	CVPropertyType currentType = CVUnknownPropertyType;
	for(currentPropertyIndex=0;currentPropertyIndex<theNetwork->propertiesCount;currentPropertyIndex++){
		CVPropertyType theType = theNetwork->propertiesTypes[currentPropertyIndex];
		if(strcmp(theNetwork->propertiesNames[currentPropertyIndex], name)==0){
			currentType = theType;
			currentData = theNetwork->propertiesData[currentPropertyIndex];
		}
	}
	CVBool replacingProperty = CVFalse;
	if(isNameProperty){
	}else if(currentPropertyIndex!=theNetwork->propertiesCount && currentType==type){
		newIndex = currentPropertyIndex;
		replacingProperty = CVTrue;
	}else if(currentPropertyIndex!=theNetwork->propertiesCount && currentType!=type){
		newIndex = currentPropertyIndex;
		free(theNetwork->propertiesData[newIndex]);
		replacingProperty = CVTrue;
	}else{
		theNetwork->propertiesCount++;
		newIndex = theNetwork->propertiesCount-1;
		theNetwork->propertiesData  = realloc(theNetwork->propertiesData, sizeof(void*)*theNetwork->propertiesCount);
		theNetwork->propertiesNames = realloc(theNetwork->propertiesNames, sizeof(CVString)*theNetwork->propertiesCount);
		theNetwork->propertiesTypes = realloc(theNetwork->propertiesTypes, sizeof(CVPropertyType)*theNetwork->propertiesCount);
	
		theNetwork->propertiesTypes[newIndex] =type;
		theNetwork->propertiesNames[newIndex] = calloc(strlen(name)+1, sizeof(CVChar));
		strncpy(theNetwork->propertiesNames[newIndex], name, strlen(name));
	}
	
	
	switch (type) {
		case CVStringPropertyType:{
			CVString* values = calloc(theNetwork->verticesCount, sizeof(CVString));
			CVString* stringValues = data;
			CVIndex i;
			for(i=0; i<theNetwork->verticesCount; i++) {
				values[i] = calloc(strlen(stringValues[i])+1, sizeof(CVChar));
				strncpy(values[i], stringValues[i], strlen(stringValues[i]));
			}
			if(isNameProperty) {
				CV_NetworkDestroyNames(theNetwork);
				theNetwork->vertexNames = values;
			}else{
				if(replacingProperty){
					CVIndex j;
					for (j=0; j<theNetwork->verticesCount; j++) {
						free(((CVString*)theNetwork->propertiesData[newIndex])[j]);
					}
					free(theNetwork->propertiesData[newIndex]);
				}
				theNetwork->propertiesData[newIndex]=values;
			}
			break;
		}
		case CVNumberPropertyType:{
			if(replacingProperty || currentType!=type){
				theNetwork->propertiesData[newIndex] = calloc(theNetwork->verticesCount, sizeof(CVFloat));
			}
			memcpy(theNetwork->propertiesData[newIndex], data, sizeof(CVFloat)*theNetwork->verticesCount);
			break;
		}
		case CVVector2DPropertyType:{
			if(replacingProperty || currentType!=type){
				theNetwork->propertiesData[newIndex] = calloc(theNetwork->verticesCount*2, sizeof(CVFloat));
			}
			memcpy(theNetwork->propertiesData[newIndex], data, sizeof(CVFloat)*theNetwork->verticesCount*2);
			break;
		}
		case CVVector3DPropertyType:{
			if(replacingProperty || currentType!=type){
				theNetwork->propertiesData[newIndex] = calloc(theNetwork->verticesCount*3, sizeof(CVFloat));
			}
			memcpy(theNetwork->propertiesData[newIndex], data, sizeof(CVFloat)*theNetwork->verticesCount*3);
			break;
		}
		default:
			break;
	}
}

void* CVNetworkPropertyWithName(const CVNetwork* network, CVString name, CVPropertyType* outType){
	CVIndex propertyIndex;
	void* data = NULL;
	for(propertyIndex=0;propertyIndex<network->propertiesCount;propertyIndex++){
		CVPropertyType type = network->propertiesTypes[propertyIndex];
		
		if(strcmp(network->propertiesNames[propertyIndex], name)==0){
			if(outType){
				*outType = type;
			}
			data = network->propertiesData[propertyIndex];
		}
	}
	return data;
}



/*
 if((token = strsep(&lineSegment, " ")) != NULL){
 }
 */

CVNetwork* CVNewNetwork(CVSize verticesCount, CVBool edgeWeighted, CVBool directed){
	CVNetwork * theNetwork = NULL;
	theNetwork = CV_NewAllocationNetwork(verticesCount);
	theNetwork->vertexWeighted = CVFalse;
	theNetwork->edgeWeighted = edgeWeighted;
	theNetwork->directed = directed;
	return theNetwork;
}

CVNetwork* CVNewNetworkWithNetwork(CVNetwork* originalNetwork, CVBool edgeWeighted, CVBool directed){
	CVNetwork * theNetwork = NULL;
	theNetwork = CV_NewAllocationNetwork(originalNetwork->verticesCount);
	theNetwork->vertexWeighted = CVFalse;
	theNetwork->edgeWeighted = edgeWeighted;
	theNetwork->directed = directed;
	CVIndex i;
	for(i=0;i<originalNetwork->edgesCount;i++){
		CVIndex from,to;
		from = originalNetwork->edgeFromList[i];
		to = originalNetwork->edgeToList[i];
		CVFloat weight = 1.0f;
		if(originalNetwork->edgeWeighted){
			weight = originalNetwork->edgesWeights[i];
		}
		CVNetworkAddNewEdge(theNetwork, from, to, weight);
	}
	CVIndex propertyIndex;
	for(propertyIndex=0;propertyIndex<originalNetwork->propertiesCount;propertyIndex++){
		CVPropertyType type = originalNetwork->propertiesTypes[propertyIndex];
		void* data = originalNetwork->propertiesData[propertyIndex];
		CVString name = originalNetwork->propertiesNames[propertyIndex];
		CVNetworkAppendProperty(theNetwork, name, type, data);
	}
	
	return theNetwork;
}

void CVNetworkWriteToFile(CVNetwork* theNetwork, FILE* networkFile){
	fprintf(networkFile,"#vertices ""%"CVSizeScan" nonweighted\n",theNetwork->verticesCount);
	if(theNetwork->vertexNames){
		CVIndex i;
		for(i=0;i<theNetwork->verticesCount;i++){
			fprintf(networkFile,"\"%s\"\n",theNetwork->vertexNames[i]);
		}
	}
	fprintf(networkFile,"#edges %s %s\n",theNetwork->edgeWeighted?"weighted":"nonweighted",theNetwork->directed?"directed":"undirected");
	CVIndex edgeIndex;
	
	CVIndex* edgesFrom = theNetwork->edgeFromList;
	CVIndex* edgesTo = theNetwork->edgeToList;
	CVFloat* edgesWeights = theNetwork->edgesWeights;
	for (edgeIndex=0; edgeIndex<theNetwork->edgesCount; edgeIndex++) {
		CVIndex fromVertex = edgesFrom[edgeIndex];
		CVIndex toVertex = edgesTo[edgeIndex];
		if(theNetwork->edgeWeighted){
			CVFloat weight = edgesWeights[edgeIndex];
			fprintf(networkFile,"%"CVIndexScan" ""%"CVIndexScan" ""%"CVFloatScan"\n",fromVertex,toVertex,weight);
		}else{
			fprintf(networkFile,"%"CVIndexScan" ""%"CVIndexScan"\n",fromVertex,toVertex);
		}
	}
	CVIndex propertyIndex;
	for(propertyIndex=0;propertyIndex<theNetwork->propertiesCount;propertyIndex++){
		CVPropertyType type = theNetwork->propertiesTypes[propertyIndex];
		void* data = theNetwork->propertiesData[propertyIndex];
		CVString name = theNetwork->propertiesNames[propertyIndex];
		switch (type) {
			case CVNumberPropertyType:{
				CVFloat* floatData = data;
				fprintf(networkFile,"#v \"%s\" n\n",name);
				CVIndex i;
				for(i=0;i<theNetwork->verticesCount;i++){
					fprintf(networkFile,"%"CVFloatScan"\n",floatData[i]);
				}
				break;
			}
			case CVStringPropertyType:{
				CVString* stringData = data;
				fprintf(networkFile,"#v \"%s\" s\n",name);
				CVIndex i;
				for(i=0;i<theNetwork->verticesCount;i++){
					fprintf(networkFile,"\"%s\"\n",stringData[i]);
				}
				break;
			}
			case CVVector2DPropertyType:{
				CVFloat* floatData = data;
				fprintf(networkFile,"#v \"%s\" v2\n",name);
				CVIndex i;
				for(i=0;i<theNetwork->verticesCount;i++){
					fprintf(networkFile,"%"CVFloatScan" %"CVFloatScan"\n",floatData[i*2],floatData[i*2+1]);
				}
				break;
			}
			case CVVector3DPropertyType:{
				CVFloat* floatData = data;
				fprintf(networkFile,"#v \"%s\" v3\n",name);
				CVIndex i;
				for(i=0;i<theNetwork->verticesCount;i++){
					fprintf(networkFile,"%"CVFloatScan" %"CVFloatScan" %"CVFloatScan"\n",floatData[i*3],floatData[i*3+1],floatData[i*3+2]);
				}
				break;
			}
			default:{
			}
		}
	}
}



void CVNetworkWriteToPajekFile(CVNetwork* theNetwork, FILE* networkFile){
	fprintf(networkFile,"*vertices ""%"CVSizeScan"\n",theNetwork->verticesCount);
	if(theNetwork->vertexNames){
		CVIndex i;
		for(i=0;i<theNetwork->verticesCount;i++){
			fprintf(networkFile,"%"CVIndexScan" \"%s\"\n",i+1,theNetwork->vertexNames[i]);
		}
	}
	if(theNetwork->directed){
		fprintf(networkFile,"*arcs\n");
	}else{
		fprintf(networkFile,"*edges\n");
	}
	CVIndex edgeIndex;
	
	CVIndex* edgesFrom = theNetwork->edgeFromList;
	CVIndex* edgesTo = theNetwork->edgeToList;
	CVFloat* edgesWeights = theNetwork->edgesWeights;
	for (edgeIndex=0; edgeIndex<theNetwork->edgesCount; edgeIndex++) {
		CVIndex fromVertex = edgesFrom[edgeIndex];
		CVIndex toVertex = edgesTo[edgeIndex];
		if(theNetwork->edgeWeighted){
			CVFloat weight = edgesWeights[edgeIndex];
			fprintf(networkFile,"%"CVIndexScan" ""%"CVIndexScan" ""%"CVFloatScan"\n",fromVertex+1,toVertex+1,weight);
		}else{
			fprintf(networkFile,"%"CVIndexScan" ""%"CVIndexScan"\n",fromVertex+1,toVertex+1);
		}
	}
}


void CVNetworkWriteToEdgesFile(CVNetwork* theNetwork, FILE* networkFile){
	CVIndex edgeIndex;
	
	CVIndex* edgesFrom = theNetwork->edgeFromList;
	CVIndex* edgesTo = theNetwork->edgeToList;
	CVFloat* edgesWeights = theNetwork->edgesWeights;
	for (edgeIndex=0; edgeIndex<theNetwork->edgesCount; edgeIndex++) {
		CVIndex fromVertex = edgesFrom[edgeIndex];
		CVIndex toVertex = edgesTo[edgeIndex];
		if(theNetwork->edgeWeighted){
			CVFloat weight = edgesWeights[edgeIndex];
			fprintf(networkFile,"%"CVIndexScan" ""%"CVIndexScan" ""%"CVFloatScan"\n",fromVertex,toVertex,weight);
		}else{
			fprintf(networkFile,"%"CVIndexScan" ""%"CVIndexScan"\n",fromVertex,toVertex);
		}
	}
}



CVNetwork* CVNewNetworkFromXNETFile(FILE* networkFile){
	CVSize verticesCount = 0;

	CVBool isReadingVertices = CVFalse;
	CVBool isReadingEdges = CVFalse;
	CVBool isReadingProperty = CVFalse;

	CVNetwork * theNetwork = NULL;
	CVSize* fromIndices = NULL;
	CVSize* toIndices = NULL;
	CVFloat* edgesWeights = NULL;
	CVSize edgesCount = 0;
	CVSize edgesCapacity = 0;
	CVIndex currentVertex = 0;
	CVString propertyName = NULL;
	CVPropertyType propertyType = CVUnknownPropertyType;
	CVIndex propertyVertexIndex = 0;
	void* propertyData = NULL;
	CVString parsingError = NULL;

	CVIndex currentLine = 0;

	CVString lineBuffer;
	while((lineBuffer = CVNewStringReadingLine(networkFile))){
		CVString lineSegment = lineBuffer;
		currentLine++;
		if(lineSegment&&CVStringScanCharacters(&lineSegment, '#')){
			//printf("Reading Line: %s\n",lineSegment);
			if(CVStringScan(&lineSegment, "vertices")){
				CVStringScanCharacters(&lineSegment, ' ');
				if(CVStringScanIndex(&lineSegment, &verticesCount)){
					CVStringScanCharacters(&lineSegment, ' ');
					//printf("VerticesCount: %ld\n", verticesCount);
					theNetwork = CV_NewAllocationNetwork(verticesCount);
				}
				if(CVStringScan(&lineSegment, "weighted")){
					theNetwork->vertexWeighted = CVTrue;
				}
				isReadingVertices=CVTrue;
				isReadingEdges=CVFalse;
				isReadingProperty = CVFalse;
				currentVertex = 0;
			}else if(CVStringScan(&lineSegment, "edges")){
				CVStringScanCharacters(&lineSegment, ' ');
				if(CVStringScan(&lineSegment, "weighted")){
					theNetwork->edgeWeighted = CVTrue;
				}
				CVStringScanCharacters(&lineSegment, ' ');
				if(CVStringScan(&lineSegment, "directed")){
					theNetwork->directed = CVTrue;
				}
				CVStringScanCharacters(&lineSegment, ' ');
				if(CVStringScan(&lineSegment, "weighted")){
					theNetwork->edgeWeighted = CVTrue;
				}
				isReadingVertices=CVFalse;
				isReadingEdges=CVTrue;
				isReadingProperty = CVFalse;
			}else if(CVStringScan(&lineSegment,"v")){
				CVStringScanCharacters(&lineSegment, ' ');
				CVStringScanCharacters(&lineSegment, '\"');
				free(propertyName);
				propertyName = CVNewStringScanningUpToCharacter(&lineSegment, '\"');
				propertyVertexIndex=0;
				CVStringScanCharacters(&lineSegment, '\"');
				CVStringScanCharacters(&lineSegment, ' ');
				if(CVStringScan(&lineSegment, "n")){
					propertyType=CVNumberPropertyType;
					isReadingProperty = CVTrue;
					propertyData = calloc(verticesCount,sizeof(CVFloat));
				}else if(CVStringScan(&lineSegment, "v2")){
					propertyType=CVVector2DPropertyType;
					isReadingProperty = CVTrue;
					propertyData = calloc(verticesCount*2,sizeof(CVFloat));
				}else if(CVStringScan(&lineSegment, "v3")){
					propertyType=CVVector3DPropertyType;
					isReadingProperty = CVTrue;
					propertyData = calloc(verticesCount*3,sizeof(CVFloat));
				}else if(CVStringScan(&lineSegment, "s")){
					propertyType=CVStringPropertyType;
					isReadingProperty = CVTrue;
					propertyData = calloc(verticesCount,sizeof(CVString));
				}else{
					isReadingProperty = CVFalse;
					//printf("Unnalocationg %s\n",propertyName);
					free(propertyName);
					parsingError = "Error reading header for property.";
					break;
				}
				isReadingVertices=CVFalse;
				isReadingEdges=CVFalse;
			}else{
				isReadingVertices=CVFalse;
				isReadingEdges=CVFalse;
				isReadingProperty = CVFalse;
			}
		}else{
			if(isReadingVertices){
				if(currentVertex<verticesCount){
					if(!theNetwork->vertexNames){
						theNetwork->vertexNames = calloc(verticesCount, sizeof(CVString));
					}
					CVStringTrim(lineSegment, "\"\n \t");
					CVSize lineLength = strlen(lineSegment);
					theNetwork->vertexNames[currentVertex] = calloc(lineLength+1, sizeof(CVChar));
					strncpy(theNetwork->vertexNames[currentVertex], lineSegment, lineLength);
					currentVertex++;
				}else{
					isReadingVertices=CVFalse;
				}
			}else if(isReadingEdges){
				unsigned long _longFromIndex = 0;
				unsigned long _longToIndex = 0;
				CVFloat _doubleWeight = 1.0;
				if(sscanf(lineSegment, "%ld %ld " "%"CVFloatScan,&_longFromIndex,&_longToIndex,&_doubleWeight)>=2){
					edgesCount++;
					if(CVUnlikely(edgesCapacity < edgesCount)){
						edgesCapacity = CVCapacityGrow(edgesCount);
						fromIndices = realloc(fromIndices, sizeof(CVIndex)*edgesCapacity);
						toIndices = realloc(toIndices, sizeof(CVIndex)*edgesCapacity);
						if(theNetwork->edgeWeighted){
							edgesWeights = realloc(edgesWeights, sizeof(CVFloat)*edgesCapacity);
						}
					}
					fromIndices[edgesCount-1]=_longFromIndex;
					toIndices[edgesCount-1]=_longToIndex;
					if(theNetwork->edgeWeighted){
						edgesWeights[edgesCount-1] = _doubleWeight;
					}
				}
			}else if(isReadingProperty){
				CVStringTrim(lineSegment, "\"\n \t");
				switch (propertyType) {
					case CVNumberPropertyType:{
						CVFloat currentValue = 0.0f;
						if(sscanf(lineSegment, "%"CVFloatScan,&currentValue)>0&&propertyVertexIndex<verticesCount){
							CVFloat* currentData = propertyData;
							currentData[propertyVertexIndex] = currentValue;
							propertyVertexIndex++;
							if(propertyVertexIndex==verticesCount){
								CVNetworkAppendProperty(theNetwork, propertyName, propertyType, currentData);
							}
						}
						break;
					}
					case CVStringPropertyType:{
						CVStringScanCharacters(&lineSegment, '\"');
						CVString currentString = CVNewStringScanningUpToCharacter(&lineSegment, '\"');
						
						CVString* currentData = propertyData;
						currentData[propertyVertexIndex] = currentString;
						propertyVertexIndex++;
						if(propertyVertexIndex==verticesCount){
							CVNetworkAppendProperty(theNetwork, propertyName, propertyType, currentData);
						}
						break;
					}
					case CVVector2DPropertyType:{
						CVFloat currentValue1 = 0.0f;
						CVFloat currentValue2 = 0.0f;
						if(sscanf(lineSegment, "%"CVFloatScan" %"CVFloatScan,&currentValue1,&currentValue2)>0&&propertyVertexIndex<verticesCount){
							CVFloat* currentData = propertyData;
							currentData[propertyVertexIndex*2] = currentValue1;
							currentData[propertyVertexIndex*2+1] = currentValue2;
							propertyVertexIndex++;
							if(propertyVertexIndex==verticesCount){
								CVNetworkAppendProperty(theNetwork, propertyName, propertyType, currentData);
							}
						}
						break;
					}
					case CVVector3DPropertyType:{
						CVFloat currentValue1 = 0.0f;
						CVFloat currentValue2 = 0.0f;
						CVFloat currentValue3 = 0.0f;
						if(sscanf(lineSegment, "%"CVFloatScan" %"CVFloatScan" %"CVFloatScan,&currentValue1,&currentValue2,&currentValue3)>0&&propertyVertexIndex<verticesCount){
							CVFloat* currentData = propertyData;
							currentData[propertyVertexIndex*3] = currentValue1;
							currentData[propertyVertexIndex*3+1] = currentValue2;
							currentData[propertyVertexIndex*3+2] = currentValue3;
							propertyVertexIndex++;
							if(propertyVertexIndex==verticesCount){
								CVNetworkAppendProperty(theNetwork, propertyName, propertyType, currentData);
							}
						}
						break;
					}
					default:
						break;
				}
			}

		}
		free(lineBuffer);
	}
	
	if(parsingError==NULL){
		if(theNetwork && theNetwork->verticesCount>0){
			CVNetworkAddNewEdges(theNetwork, fromIndices,toIndices,edgesWeights, edgesCount);
		}
	}else{
		fprintf(stderr, "Parsing error occurred[at line %"CVIndexScan"]: %s\n", currentLine, parsingError);
		CVNetworkDestroy(theNetwork);
		theNetwork = NULL;
	}
	free(fromIndices);
	free(toIndices);
	free(edgesWeights);

	return theNetwork;
}

void CVNetworkDestroy(CVNetwork* network){
	CVIndex i;
	CV_NetworkDestroyProperties(network);
	CV_NetworkDestroyNames(network);
	
	
	
	
	for(i=0;i<network->verticesCount;i++){
		free(network->vertexEdgesLists[i]);
		free(network->vertexEdgesIndices[i]);

		//if(network->directed){
			free(network->vertexInEdgesLists[i]);
			free(network->vertexInEdgesIndices[i]);
			//}
	}

	free(network->vertexCapacityOfEdges);
	free(network->vertexNumOfEdges);
	free(network->vertexEdgesLists);
	free(network->vertexEdgesIndices);

	//if(network->directed){
		free(network->vertexNumOfInEdges);
		free(network->vertexInEdgesLists);
		free(network->vertexInEdgesIndices);
		free(network->vertexCapacityOfInEdges);
	//}

	free(network->edgeFromList);
	free(network->edgeToList);

	//if(network->edgeWeighted)
	free(network->edgesWeights);

	free(network->verticesWeights);
	free(network->verticesEnabled);
	
	free(network);
}

void CVNetworkGetDegree(const CVNetwork* network, CVIntegerArray* degrees){
	CVSize verticesCount = network->verticesCount;
	CVIntegerArrayReallocToCapacity(verticesCount, degrees);
	CVIntegerArraySetCount(verticesCount, degrees);
	
	CVIndex vertexIndex;
	for (vertexIndex=0; vertexIndex<verticesCount; vertexIndex++) {
		degrees->data[vertexIndex] = network->vertexNumOfEdges[vertexIndex];
	}
}

CVNetwork* CVNewRegularNetwork(CVSize* gridSize, CVSize dimension, CVFloat connectionRadius, CVBool toroidal){
	CVFloat maxGridSize = 0;
	CVIndex curDim;
	CVSize verticesCount = 1;
	for(curDim = 0; curDim<dimension;curDim++){
		maxGridSize = CVMAX(maxGridSize,gridSize[curDim]);
		verticesCount*=gridSize[curDim];
	}
	
	CVFloat maxRadius = CVMIN(connectionRadius, maxGridSize);
	
	CVInteger maxRadiusSize = ceil(maxRadius);
	CVSize inRadiusMaxCount = (CVSize)ipow(2*(int)maxRadiusSize+1,dimension);
	CVInteger* inRadiusIndices = calloc(inRadiusMaxCount*dimension, sizeof(CVInteger));
	CVFloat* inRadiusDistances = calloc(inRadiusMaxCount, sizeof(CVFloat));
	CVSize inRadiusCount = 0;
	
	CVIndex i;
	CVInteger* coordinateVector = calloc(dimension, sizeof(CVInteger));
	CVInteger* coordinateCenter = calloc(dimension, sizeof(CVInteger));
	CVSize* coordinateRadiusBox = calloc(dimension, sizeof(CVSize));
	
	for(i=0;i<dimension;i++){
		coordinateCenter[i] = maxRadiusSize;
		coordinateRadiusBox[i] = maxRadiusSize*2+1;
	}
	for (i=0; i<inRadiusMaxCount; i++) {
		CVGridGetCoordinatesFromLinearIndex(i, coordinateRadiusBox, dimension, coordinateVector);
		CVFloat distance = CVGridCalcIntegerDistance(coordinateCenter,coordinateVector, dimension);
		if(distance<=maxRadius){
			memcpy(inRadiusIndices+(inRadiusCount*dimension), coordinateVector, dimension*sizeof(CVInteger));
			inRadiusDistances[inRadiusCount] = distance;
			inRadiusCount++;
		}
	}
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	
	CVInteger cellCount = verticesCount;
	
	CVInteger* newCoordinateVector = calloc(dimension, sizeof(CVInteger));
	CVIndex cellIndex;
	for(cellIndex=0;cellIndex<cellCount;cellIndex++){
		CVGridGetCoordinatesFromLinearIndex(cellIndex, gridSize, dimension, coordinateVector);
		CVIndex rIndex;
		for (rIndex=0; rIndex<inRadiusCount; rIndex++) {
			CVGridGetCoordinatesFromLinearIndex(cellIndex, gridSize, dimension, coordinateVector);
			//CVFloat distance = inRadiusDistances[rIndex];
			if(toroidal
			   ?CVGridGetDisplacedToroidalCoordinate(coordinateVector, inRadiusIndices+(rIndex*dimension),  coordinateCenter, gridSize, dimension,newCoordinateVector)
			   :CVGridGetDisplacedCoordinate(coordinateVector, inRadiusIndices+(rIndex*dimension),  coordinateCenter, gridSize, dimension,newCoordinateVector)){
				CVIndex displacedIndex = CVGridLinearIndexFromCoordinates(newCoordinateVector, gridSize, dimension);
				if(cellIndex<displacedIndex){
					CVNetworkAddNewEdge(theNetwork, cellIndex, displacedIndex, 1.0f);
				}
			}
			
		}
	}
	free(coordinateCenter);
	free(coordinateVector);
	free(inRadiusDistances);
	free(inRadiusIndices);
	return theNetwork;
}

CVNetwork* CVNewRegular2DNetwork(CVSize rows, CVSize columns, CVBool toroidal){
	CVSize verticesCount = rows*columns;
	CVSize maximumEdgesCount = verticesCount*2;
	CVSize edgesCount = 0;
	CVIndex* fromList = calloc(maximumEdgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(maximumEdgesCount, sizeof(CVIndex));
	CVFloat* positions = calloc(verticesCount*3, sizeof(CVFloat));
	CVIndex i,j;
	for (i=0; i<rows; i++) {
		for (j=0; j<columns; j++) {
			positions[(i*columns+j)*3+0]=(i-rows*0.5f)*200.0f/CVMAX(rows, columns);
			positions[(i*columns+j)*3+1]=(j-columns*0.5f)*200.0f/CVMAX(rows, columns);
			positions[(i*columns+j)*3+2]=0.0f;
			if(toroidal){
				fromList[edgesCount] = i*columns+j;
				toList[edgesCount] =(i)*columns+((j+1)%columns);
				edgesCount++;
				
				fromList[edgesCount] = i*columns+j;
				toList[edgesCount] =((i+1)%rows)*columns+(j);
				edgesCount++;
			}else{
				if(j+1<columns){
					fromList[edgesCount] = i*columns+j;
					toList[edgesCount] =(i)*columns+(j+1);
					edgesCount++;
				}
				if(i+1<rows){
					fromList[edgesCount] = i*columns+j;
					toList[edgesCount] =(i+1)*columns+(j);
					edgesCount++;
				}
			}
		}
	}
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	CVNetworkAppendProperty(theNetwork, "Position", CVVector3DPropertyType, positions);
	free(positions);
	free(fromList);
	free(toList);
	return theNetwork;
}


CVNetwork* CVNewRandomNetwork(CVSize verticesCount, CVFloat degree){
	CVSize averageEdgesCount = floorf(verticesCount*degree)+1;
	CVSize edgesCapacity = averageEdgesCount;
	CVIndex* fromList = calloc(edgesCapacity, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCapacity, sizeof(CVIndex));
	CVSize edgesCount = 0;
	
	double probability = degree/(double)verticesCount;
	
	CVIndex toIndex,fromIndex;
	for (fromIndex=0; fromIndex<verticesCount; fromIndex++) {
		for (toIndex=fromIndex+1; toIndex<verticesCount; toIndex++) {
			if (CVRandomFloat()<probability){
				if(edgesCapacity<edgesCount+1){
					edgesCapacity = CVCapacityGrow(edgesCount+1);
					fromList = realloc(fromList, sizeof(CVIndex)*edgesCapacity);
					toList = realloc(toList, sizeof(CVIndex)*edgesCapacity);
				}
				fromList[edgesCount] = fromIndex;
				toList[edgesCount] = toIndex;
				edgesCount++;
			}
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	free(fromList);
	free(toList);
	return theNetwork;
}

CVNetwork* CVNewFastRandomNetwork(CVSize verticesCount, CVFloat degree){
	CVSize edgesCount = roundf(verticesCount*degree*0.5);
	CVIndex* fromList = calloc(edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCount, sizeof(CVIndex));
	
	CVIndex edgeIndex;
	for (edgeIndex=0; edgeIndex<edgesCount; edgeIndex++) {
		fromList[edgeIndex] = CVRandomInRange(0, verticesCount);
		toList[edgeIndex] = CVRandomInRange(0, verticesCount);
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	free(fromList);
	free(toList);
	return theNetwork;
}



CVNetwork* CVNewWaxmanNetwork(CVSize verticesCount,CVFloat alpha, CVFloat beta, CVSize dimension){
	CVSize edgesCapacity = verticesCount*3;
	CVIndex* fromList = calloc(edgesCapacity, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCapacity, sizeof(CVIndex));
	CVSize edgesCount = 0;
	CVFloat* positions = calloc(verticesCount*dimension, sizeof(CVFloat));
	CVFloat* positions3D = calloc(verticesCount*3, sizeof(CVFloat));
	
	CVIndex vertexIndex;
	for (vertexIndex=0; vertexIndex<verticesCount; vertexIndex++) {
		CVIndex dimensionIndex;
		for (dimensionIndex=0; dimensionIndex<dimension; dimensionIndex++) {
			positions[dimension*vertexIndex+dimensionIndex] = CVRandomFloat();
			if(dimensionIndex<3){
				positions3D[3*vertexIndex+dimensionIndex] = (positions[dimension*vertexIndex+dimensionIndex] - 0.5)*200;
			}
		}
	}
	
	CVIndex toIndex,fromIndex;
	for (fromIndex=0; fromIndex<verticesCount; fromIndex++) {
		printf("Oe %"CVUIntegerScan"      \r",fromIndex);
		fflush(stdout);
		for (toIndex=fromIndex+1; toIndex<verticesCount; toIndex++) {
			double distanceSquared = 0.0;
			CVIndex dimensionIndex;
			for (dimensionIndex=0; dimensionIndex<dimension; dimensionIndex++) {
				double positionFrom = positions[dimension*fromIndex+dimensionIndex];
				double positionTo = positions[dimension*toIndex+dimensionIndex];
				distanceSquared += (positionFrom-positionTo)*(positionFrom-positionTo);
			}
			double probability = alpha*exp(-sqrt(distanceSquared)/(beta*sqrt(dimension)));
			if (CVRandomFloat()<probability){
				if(edgesCapacity<edgesCount+1){
					edgesCapacity = CVCapacityGrow(edgesCount+1);
					fromList = realloc(fromList, sizeof(CVIndex)*edgesCapacity);
					toList = realloc(toList, sizeof(CVIndex)*edgesCapacity);
				}
				fromList[edgesCount] = fromIndex;
				toList[edgesCount] = toIndex;
				edgesCount++;
			}
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	CVNetworkAppendProperty(theNetwork, "Position", CVVector3DPropertyType, positions3D);
	free(fromList);
	free(toList);
	free(positions);
	free(positions3D);
	return theNetwork;
}



CVNetwork* CVNewRandomGeographicNetwork(CVSize verticesCount, CVFloat maximumDistance, CVSize dimension){
	CVSize edgesCapacity = verticesCount*3;
	CVIndex* fromList = calloc(edgesCapacity, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCapacity, sizeof(CVIndex));
	CVSize edgesCount = 0;
	CVFloat* positions = calloc(verticesCount*dimension, sizeof(CVFloat));
	CVFloat* positions3D = calloc(verticesCount*3, sizeof(CVFloat));
	
	CVIndex vertexIndex;
	for (vertexIndex=0; vertexIndex<verticesCount; vertexIndex++) {
		CVIndex dimensionIndex;
		for (dimensionIndex=0; dimensionIndex<dimension; dimensionIndex++) {
			positions[dimension*vertexIndex+dimensionIndex] = CVRandomFloat();
			if(dimensionIndex<3){
				positions3D[3*vertexIndex+dimensionIndex] = (positions[dimension*vertexIndex+dimensionIndex] - 0.5)*200;
			}
		}
	}
	
	CVIndex toIndex,fromIndex;
	for (fromIndex=0; fromIndex<verticesCount; fromIndex++) {
		for (toIndex=fromIndex+1; toIndex<verticesCount; toIndex++) {
			double distanceSquared = 0.0;
			CVIndex dimensionIndex;
			for (dimensionIndex=0; dimensionIndex<dimension; dimensionIndex++) {
				double positionFrom = positions[dimension*fromIndex+dimensionIndex];
				double positionTo = positions[dimension*toIndex+dimensionIndex];
				distanceSquared += (positionFrom-positionTo)*(positionFrom-positionTo);
			}
			if (sqrt(distanceSquared)<maximumDistance){
				if(edgesCapacity<edgesCount+1){
					edgesCapacity = CVCapacityGrow(edgesCount+1);
					fromList = realloc(fromList, sizeof(CVIndex)*edgesCapacity);
					toList = realloc(toList, sizeof(CVIndex)*edgesCapacity);
				}
				fromList[edgesCount] = fromIndex;
				toList[edgesCount] = toIndex;
				edgesCount++;
			}
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	CVNetworkAppendProperty(theNetwork, "Position", CVVector3DPropertyType, positions3D);
	free(fromList);
	free(toList);
	free(positions);
	free(positions3D);
	return theNetwork;
}



CVNetwork* CVNewRandomProbabilisticGeographicNetwork(CVSize verticesCount,CVFloat connectionProbability, CVFloat maximumDistance, CVSize dimension){
	CVSize edgesCapacity = verticesCount*3;
	CVIndex* fromList = calloc(edgesCapacity, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCapacity, sizeof(CVIndex));
	CVSize edgesCount = 0;
	CVFloat* positions = calloc(verticesCount*dimension, sizeof(CVFloat));
	CVFloat* positions3D = calloc(verticesCount*3, sizeof(CVFloat));
	
	CVIndex vertexIndex;
	for (vertexIndex=0; vertexIndex<verticesCount; vertexIndex++) {
		CVIndex dimensionIndex;
		for (dimensionIndex=0; dimensionIndex<dimension; dimensionIndex++) {
			positions[dimension*vertexIndex+dimensionIndex] = CVRandomFloat();
			if(dimensionIndex<3){
				positions3D[3*vertexIndex+dimensionIndex] = (positions[dimension*vertexIndex+dimensionIndex] - 0.5)*200;
			}
		}
	}
	
	CVIndex toIndex,fromIndex;
	for (fromIndex=0; fromIndex<verticesCount; fromIndex++) {
		for (toIndex=fromIndex+1; toIndex<verticesCount; toIndex++) {
			double distanceSquared = 0.0;
			CVIndex dimensionIndex;
			for (dimensionIndex=0; dimensionIndex<dimension; dimensionIndex++) {
				double positionFrom = positions[dimension*fromIndex+dimensionIndex];
				double positionTo = positions[dimension*toIndex+dimensionIndex];
				distanceSquared += (positionFrom-positionTo)*(positionFrom-positionTo);
			}
			if (sqrt(distanceSquared)<maximumDistance && CVRandomFloat()<=connectionProbability){
				if(edgesCapacity<edgesCount+1){
					edgesCapacity = CVCapacityGrow(edgesCount+1);
					fromList = realloc(fromList, sizeof(CVIndex)*edgesCapacity);
					toList = realloc(toList, sizeof(CVIndex)*edgesCapacity);
				}
				fromList[edgesCount] = fromIndex;
				toList[edgesCount] = toIndex;
				edgesCount++;
			}
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	CVNetworkAppendProperty(theNetwork, "Position", CVVector3DPropertyType, positions3D);
	free(fromList);
	free(toList);
	free(positions);
	free(positions3D);
	return theNetwork;
}



CVNetwork* CVNewNetworkFromRandomRewiringEdgeList(CVIndex* fromList,CVIndex* toList, CVSize edgesCount, CVSize verticesCount, CVBool directed, CVFloat rewireProbability){
	CVIndex edgeIndex = 0;
	struct __cv_edge{
		CVIndex from;
		CVIndex to;
	};
	struct __cv_edge_element{
		struct __cv_edge edge;
		UT_hash_handle hh;
	};
	
	struct __cv_edge_element* edgesHash = NULL;
	
	//	HASH_ADD_KEYPTR(hh, edgesHash, edgesHash->edge, sizeof(struct __cv_edge), edgesHash);
	
	for (edgeIndex=0; edgeIndex<edgesCount; edgeIndex++) {
		struct __cv_edge_element* edgeElement = calloc(1, sizeof(struct __cv_edge_element));
		if(!directed){
			edgeElement->edge.from = CVMIN(fromList[edgeIndex],toList[edgeIndex]);
			edgeElement->edge.to = CVMAX(fromList[edgeIndex],toList[edgeIndex]);
		}else{
			edgeElement->edge.from = fromList[edgeIndex];
			edgeElement->edge.to = toList[edgeIndex];
		}
		HASH_ADD_KEYPTR(hh, edgesHash, (&(edgeElement->edge)), sizeof(struct __cv_edge), edgeElement);
	}
	
	for (edgeIndex=0; edgeIndex<edgesCount; edgeIndex++) {
		if(CVRandomFloat()<rewireProbability){
			CVBool edgeExists;
			do{
				struct __cv_edge_element* edgeElement = NULL;
				struct __cv_edge edgesKey;
				edgesKey.from = CVRandomInRange(0, verticesCount);//fromList[edgeIndex];
				edgesKey.to = CVRandomInRange(0, verticesCount);
				fromList[edgeIndex] = edgesKey.from;
				toList[edgeIndex] = edgesKey.to;
				HASH_FIND(hh, edgesHash, &edgesKey, sizeof(struct __cv_edge), edgeElement);
				if(edgeElement || edgesKey.from==edgesKey.to){
					edgeExists=CVTrue;
				}else{
					edgeExists=CVFalse;
				}
			}while(edgeExists);
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, directed);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	
	struct __cv_edge_element* edgeElement, *tempElement;
	
	HASH_ITER(hh, edgesHash, edgeElement, tempElement) {
		HASH_DEL(edgesHash, edgeElement);
		free(edgeElement);
	}
	
	return theNetwork;
}



CVNetwork* CVNewNetworkFromRandomRemovingEdgeList(CVIndex* fromList,CVIndex* toList, CVSize edgesCount, CVSize verticesCount, CVBool directed, CVFloat rewireProbability){
	CVIndex edgeIndex = 0;
	CVSize newEdgesCount = 0;
	
	for (edgeIndex=0; edgeIndex<edgesCount; edgeIndex++) {
		if(CVRandomFloat()>=rewireProbability){ //not remove
			fromList[newEdgesCount] = fromList[edgeIndex];
			toList[newEdgesCount] = toList[edgeIndex];
			newEdgesCount++;
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, directed);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, newEdgesCount);
	
	return theNetwork;
}

CVNetwork* CVNewNetworkFromRandomRemoving(const CVNetwork* originalNetwork, CVFloat removingProbability){
	CVIndex* fromList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	
	memcpy(fromList, originalNetwork->edgeFromList, sizeof(CVSize)*originalNetwork->edgesCount);
	memcpy(toList, originalNetwork->edgeToList, sizeof(CVSize)*originalNetwork->edgesCount);
	
	CVNetwork* theNetwork = CVNewNetworkFromRandomRemovingEdgeList(fromList, toList, originalNetwork->edgesCount, originalNetwork->verticesCount, originalNetwork->directed,removingProbability);
	
	free(fromList);
	free(toList);
	return theNetwork;
}




CVNetwork* CVNewNetworkFromRectangleRemovingEdgeList(CVIndex* fromList,CVIndex* toList, CVSize edgesCount, CVSize verticesCount, CVBool directed, CVFloat* positions,CVFloat minRectangleSize, CVFloat maxRectangleSize,CVSize rectangleCount,CVFloat removeProbability){
	CVIndex edgeIndex = 0;
	CVSize newEdgesCount = 0;
	
	CVIndex i;
	CVFloat positionXMax = CVFloatMIN;
	CVFloat positionXMin = CVFloatMAX;
	CVFloat positionYMax = CVFloatMIN;
	CVFloat positionYMin = CVFloatMAX;
	
	for (i=0; i<verticesCount; i++) {
		CVFloat x = positions[3*i];
		CVFloat y = positions[3*i+1];
		positionXMax = CVMAX(positionXMax, x);
		positionXMin = CVMIN(positionXMin, x);
		positionYMax = CVMAX(positionYMax, y);
		positionYMin = CVMIN(positionYMin, y);
	}
	CVFloat* rectangles = calloc(rectangleCount*4, sizeof(CVFloat));
	
	for (i=0; i<rectangleCount; i++) {
		CVFloat cx = positionXMin + CVRandomFloat()*(positionXMax-positionXMin);
		CVFloat cy = positionYMin + CVRandomFloat()*(positionYMax-positionYMin);
		
		CVFloat sizex = (minRectangleSize + (maxRectangleSize-minRectangleSize)*CVRandomFloat())*(positionXMax-positionXMin);
		CVFloat sizey = (minRectangleSize + (maxRectangleSize-minRectangleSize)*CVRandomFloat())*(positionYMax-positionYMin);
		
		rectangles[i*4 + 0] = cx;
		rectangles[i*4 + 1] = cy;
		rectangles[i*4 + 2] = sizex;
		rectangles[i*4 + 3] = sizey;
		printf("xy = (%f, %f)    rect = [%f, %f, %f, %f]\n",0.1,1.2,cx-sizex*0.5f,cy-sizey*0.5f,cx+sizex*0.5f,cy+sizey*0.5f);
	}
	
	double* probabilityVertices = calloc(verticesCount,sizeof(double));
	CVIndex j;
	for (j=0; j<verticesCount; j++) {
		probabilityVertices[j] = 1.0;
		CVFloat x = positions[3*j];
		CVFloat y = positions[3*j+1];
		for (i=0; i<rectangleCount; i++) {
			CVFloat cx = rectangles[i*4 + 0];
			CVFloat cy = rectangles[i*4 + 1];
			CVFloat sizex = rectangles[i*4 + 2];
			CVFloat sizey = rectangles[i*4 + 3];
			//printf("xy = (%f, %f)    rect = [%f, %f, %f, %f]\n",x,y,cx-sizex*0.5f,cy-sizey*0.5f,cx+sizex*0.5f,cy+sizey*0.5f);
			
			if(cx-sizex*0.5f <= x && x <= cx+sizex*0.5f && cy-sizey*0.5f <= y && y <= cy+sizey*0.5f){
				probabilityVertices[j] *= (1.0-removeProbability);
			}
		}
	}
	
	for (edgeIndex=0; edgeIndex<edgesCount; edgeIndex++) {
		double prob = (probabilityVertices[fromList[edgeIndex]] * probabilityVertices[toList[edgeIndex]]);
		if(CVRandomFloat() < prob){ //not remove
			fromList[newEdgesCount] = fromList[edgeIndex];
			toList[newEdgesCount] = toList[edgeIndex];
			newEdgesCount++;
		}
	}
	
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, directed);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, newEdgesCount);
	
	free(rectangles);
	free(probabilityVertices);
	return theNetwork;
}

CVNetwork* CVNewNetworkFromRectangleRemoving(const CVNetwork* originalNetwork,CVFloat minRectangleSize, CVFloat maxRectangleSize, CVSize rectangleCount,CVFloat removeProbability){
	CVNetwork* theNetwork;
	CVFloat* positions = NULL;
	CVPropertyType positionType = CVUnknownPropertyType;
	positions = CVNetworkPropertyWithName(originalNetwork, "Position", &positionType);
	if(positions && positionType==CVVector3DPropertyType){
		CVIndex* fromList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
		CVIndex* toList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
		memcpy(fromList, originalNetwork->edgeFromList, sizeof(CVSize)*originalNetwork->edgesCount);
		memcpy(toList, originalNetwork->edgeToList, sizeof(CVSize)*originalNetwork->edgesCount);
		theNetwork = CVNewNetworkFromRectangleRemovingEdgeList(fromList, toList, originalNetwork->edgesCount, originalNetwork->verticesCount, originalNetwork->directed, positions,minRectangleSize,maxRectangleSize,rectangleCount, removeProbability);
		CVNetworkAppendProperty(theNetwork, "Position", CVVector3DPropertyType, positions);
		free(fromList);
		free(toList);
	}else{
		theNetwork = NULL;
	}
	
	return theNetwork;
}


CVNetwork* CVNewNetworkFromRandomRewiring(const CVNetwork* originalNetwork, CVFloat rewiringProbability){
	CVIndex* fromList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	
	memcpy(fromList, originalNetwork->edgeFromList, sizeof(CVSize)*originalNetwork->edgesCount);
	memcpy(toList, originalNetwork->edgeToList, sizeof(CVSize)*originalNetwork->edgesCount);
	
	CVNetwork* theNetwork = CVNewNetworkFromRandomRewiringEdgeList(fromList, toList, originalNetwork->edgesCount, originalNetwork->verticesCount, originalNetwork->directed,rewiringProbability);
	
	free(fromList);
	free(toList);
	return theNetwork;
}


CVNetwork* CVNewNetworkFromModularRandomRewiring(const CVNetwork* originalNetwork, CVIntegerArray modules, CVFloat rewiringProbability){
	CVIndex* fromList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	
	memcpy(fromList, originalNetwork->edgeFromList, sizeof(CVSize)*originalNetwork->edgesCount);
	memcpy(toList, originalNetwork->edgeToList, sizeof(CVSize)*originalNetwork->edgesCount);
	
	CVNetwork* theNetwork = CVNewNetworkFromRandomRewiringEdgeList(fromList, toList, originalNetwork->edgesCount, originalNetwork->verticesCount, originalNetwork->directed,rewiringProbability);
	
	free(fromList);
	free(toList);
	return theNetwork;
}


CVNetwork* CVNewWattsStrogatzNetwork(CVSize approximateNumberOfVertices, CVSize dimension, CVFloat connectionRadius, CVBool toroidal, CVFloat rewiringProbability){
	CVSize* gridSize = calloc(dimension, sizeof(CVSize));
	CVSize sizePerDimension = CVMAX(1,ceil(pow(approximateNumberOfVertices, 1.0/dimension)));
	CVIndex gridIndex;
	for (gridIndex=0; gridIndex<dimension; gridIndex++) {
		gridSize[gridIndex] = sizePerDimension;
	}
	CVNetwork* gridNetwork = CVNewRegularNetwork(gridSize, dimension, connectionRadius, toroidal);
	
	CVNetwork* wsNetwork = CVNewNetworkFromRandomRewiring(gridNetwork, rewiringProbability);
	free(gridSize);
	CVNetworkDestroy(gridNetwork);
	return wsNetwork;
}


CVNetwork* CVNewBarabasiAlbertNetwork(CVSize initialSize, CVSize degreeGrowth, CVSize iterations){
	CVSize edgesCount = iterations*degreeGrowth;
	CVIndex* fromList = calloc(edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCount, sizeof(CVIndex));
	CVSize verticesCount = iterations+initialSize;
	CVIndex currentLink=0;
	CVIndex currentVertex;
	
	CVSize* distribGenerator = calloc(initialSize+(degreeGrowth*2)*iterations, sizeof(CVSize));
	CVIndex distribPointer=0;
	CVIndex i;
	for(i=0;i<initialSize;i++){
		distribGenerator[distribPointer]=i;
		distribPointer++;
	}
	
	for(currentVertex=initialSize;currentVertex < iterations+initialSize;currentVertex++){
		CVIndex m;
		for(m=0;m<degreeGrowth;m++){
			CVIndex connectTo = 0;
			CVBool linkExist = CVFalse;
			do{
				linkExist = CVFalse;
				connectTo = distribGenerator[CVRandomInRange(0, distribPointer-1)];
				CVIndex curLink;
				for(curLink=0;curLink<currentLink;curLink++){
					if((fromList[curLink]==currentVertex&&toList[curLink]==connectTo) ||
					   (fromList[curLink]==connectTo&&toList[curLink]==currentVertex)    ){
						linkExist=CVTrue;
					}
				}
			}while(linkExist);
			fromList[currentLink] = currentVertex;
			toList[currentLink] = connectTo;
			currentLink++;
			distribGenerator[distribPointer]=connectTo;
			distribPointer++;
		}
		for(m=0;m<degreeGrowth;m++){
			distribGenerator[distribPointer]=currentVertex;
			distribPointer++;
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, CVFalse);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	free(fromList);
	free(toList);
	free(distribGenerator);
	return theNetwork;
}



CVNetwork** CVNewBarabasiAlbertNetworkOverTime(CVSize initialSize, CVSize degreeGrowth, CVSize* iterationsArray, CVSize iterationsCount){
	CVSize edgesCount = iterationsArray[iterationsCount-1]*degreeGrowth;
	CVIndex* fromList = calloc(edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCount, sizeof(CVIndex));
	CVSize verticesCount = iterationsArray[iterationsCount-1]+initialSize;
	CVIndex currentLink=0;
	CVIndex currentVertex;
	CVNetwork** networks = calloc(iterationsCount, sizeof(CVNetwork*));
	CVSize* distribGenerator = calloc(initialSize+(degreeGrowth*2)*iterationsArray[iterationsCount-1], sizeof(CVSize));
	CVIndex distribPointer=0;
	CVIndex i;
	for(i=0;i<initialSize;i++){
		distribGenerator[distribPointer]=i;
		distribPointer++;
	}
	currentVertex=initialSize;
	for (CVIndex iterationIndex = 0; iterationIndex < iterationsCount; iterationIndex++) {
		CVSize iterations = iterationsArray[iterationIndex];
		for(;currentVertex < iterations+initialSize;currentVertex++){
			CVIndex m;
			for(m=0;m<degreeGrowth;m++){
				CVIndex connectTo = 0;
				CVBool linkExist = CVFalse;
				do{
					linkExist = CVFalse;
					connectTo = distribGenerator[CVRandomInRange(0, distribPointer-1)];
					CVIndex curLink;
					for(curLink=0;curLink<currentLink;curLink++){
						if((fromList[curLink]==currentVertex&&toList[curLink]==connectTo) ||
							 (fromList[curLink]==connectTo&&toList[curLink]==currentVertex)    ){
							linkExist=CVTrue;
						}
					}
				}while(linkExist);
				fromList[currentLink] = currentVertex;
				toList[currentLink] = connectTo;
				currentLink++;
				distribGenerator[distribPointer]=connectTo;
				distribPointer++;
			}
			for(m=0;m<degreeGrowth;m++){
				distribGenerator[distribPointer]=currentVertex;
				distribPointer++;
			}
		}

		CVSize currentVerticesCount = iterations+initialSize;
		CVSize currentEdgesCount = iterations*degreeGrowth;

		CVNetwork* theNetwork = CVNewNetwork(currentVerticesCount, CVFalse, CVFalse);
		CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, currentEdgesCount);
		networks[iterationIndex] = theNetwork;
	}
	free(fromList);
	free(toList);
	free(distribGenerator);
	return networks;
}


CVBool CVNetworkCouldBeIsomorphic(const CVNetwork* aNetwork,const CVNetwork* bNetwork){
	if(aNetwork->directed||bNetwork->directed){ //DIRECTED NOT SUPPORTED
		return CVFalse;
	}
	if(aNetwork->vertexWeighted||bNetwork->vertexWeighted){ //DIRECTED NOT SUPPORTED
		return CVFalse;
	}
	if(aNetwork->verticesCount!=bNetwork->verticesCount){
		return CVFalse;
	}
	
	if(aNetwork->edgesCount!=bNetwork->edgesCount){
		return CVFalse;
	}
	
	CVIntegerArray aDegrees;
	CVIntegerArray bDegrees;
	
	CVIntegerArrayInitWithCapacity(1, &aDegrees);
	CVIntegerArrayInitWithCapacity(1, &bDegrees);
	
	CVNetworkGetDegree(aNetwork, &aDegrees);
	CVNetworkGetDegree(bNetwork, &bDegrees);
	
	CVIntegerArrayQuickSort3(&aDegrees);
	CVIntegerArrayQuickSort3(&bDegrees);
	
	CVIndex i;
	CVBool degreeOk = CVTrue;;
	for (i=0; i<aNetwork->verticesCount; i++) {
		if(aDegrees.data[i]!=bDegrees.data[i]){
			degreeOk = CVFalse;
		}
	}
	
	
	if(!degreeOk){
		CVIntegerArrayDestroy(&aDegrees);
		CVIntegerArrayDestroy(&bDegrees);
		return CVFalse;
	}
	
	
	//Neighbors
	CVIntegerArray aEdgeDegrees;
	CVIntegerArray bEdgeDegrees;
	
	CVNetworkGetDegree(aNetwork, &aDegrees);
	CVNetworkGetDegree(bNetwork, &bDegrees);
	
	CVIntegerArrayInitWithCapacity(1, &aEdgeDegrees);
	CVIntegerArrayInitWithCapacity(1, &bEdgeDegrees);
	
	CVBool edgesDegreeOk = CVTrue;
	for (i=0; i<aNetwork->edgesCount; i++) {
		CVInteger aEdgeDegree = aDegrees.data[aNetwork->edgeFromList[i]]+aDegrees.data[aNetwork->edgeToList[i]];
		CVInteger bEdgeDegree = bDegrees.data[bNetwork->edgeFromList[i]]+bDegrees.data[bNetwork->edgeToList[i]];
		
		CVIntegerArrayAdd(aEdgeDegree, &aEdgeDegrees);
		CVIntegerArrayAdd(bEdgeDegree, &bEdgeDegrees);
	}
	
	CVIntegerArrayQuickSort3(&aEdgeDegrees);
	CVIntegerArrayQuickSort3(&bEdgeDegrees);
	
	for (i=0; i<aNetwork->edgesCount; i++) {
		if(aEdgeDegrees.data[i]!=bEdgeDegrees.data[i]){
			edgesDegreeOk = CVFalse;
		}
	}
	
	CVIntegerArrayDestroy(&aDegrees);
	CVIntegerArrayDestroy(&bDegrees);
	CVIntegerArrayDestroy(&aEdgeDegrees);
	CVIntegerArrayDestroy(&bEdgeDegrees);
	
	if(!edgesDegreeOk){
		return CVFalse;
	}
	
	
	
	return CVTrue;
}





CVBool CVNetworkAddNewEdge(CVNetwork* network, CVIndex fromIndex, CVIndex toIndex, CVFloat weight){
	CVIndex initialEdge = network->edgesCount;
	CVSize verticesCount = network->verticesCount;
	if (toIndex >= verticesCount||fromIndex >= verticesCount) {
		return CVFalse;
	}
	CVNetworkGrowEdgesCapacity(network,1);
	network->edgeFromList[initialEdge]=fromIndex;
	network->edgeToList[initialEdge]=toIndex;
	
	CVNetworkGrowVertexSetEdgeForVertex(network,initialEdge,fromIndex,toIndex);
	
	if(network->edgeWeighted&&weight>=0){
		network->edgesWeights[initialEdge]=weight;
	}else if(network->edgeWeighted){
		network->edgesWeights[initialEdge]=1.0f;
	}
	if(!network->directed){
		//printf("Index: %lu toIndex:%lu fromIndex:%lu\n",i+initialEdge,toIndex,fromIndex);
		CVNetworkGrowVertexSetEdgeForVertex(network,initialEdge,toIndex,fromIndex);
		//printf("OK\n");
	}else{
		CVNetworkGrowVertexSetInEdgeForVertex(network,initialEdge,toIndex,fromIndex);
	}
	network->edgesCount++;
	return CVTrue;
}

CVBool CVNetworkAddNewEdgeAndIntegrateWeight(CVNetwork* network, CVIndex fromIndex, CVIndex toIndex, CVFloat weight){
	CVIndex initialEdge = network->edgesCount;
	CVSize verticesCount = network->verticesCount;
	if (toIndex >= verticesCount||fromIndex >= verticesCount) {
		return CVFalse;
	}
	
	CVBool edgeFound = CVFalse;
	CVIndex i;
	CVSize toVerticesCount = network->vertexNumOfEdges[fromIndex];
	CVIndex* toVertices = network->vertexEdgesLists[fromIndex];
	for (i=0; i<toVerticesCount; i++) {
		if(toVertices[i]==toIndex){
			edgeFound = CVTrue;
			break;
		}
	}
	
	if(edgeFound){
		if(network->edgeWeighted&&weight>0){
			network->edgesWeights[network->vertexEdgesIndices[fromIndex][i]]+=weight;
		}
	}else{
		CVNetworkGrowEdgesCapacity(network,1);
		network->edgeFromList[initialEdge]=fromIndex;
		network->edgeToList[initialEdge]=toIndex;
		
		CVNetworkGrowVertexSetEdgeForVertex(network,initialEdge,fromIndex,toIndex);
		
		if(network->edgeWeighted&&weight>=0){
			network->edgesWeights[initialEdge]=weight;
		}else if(network->edgeWeighted){
			network->edgesWeights[initialEdge]=1.0f;
		}
		if(!network->directed){
			//printf("Index: %lu toIndex:%lu fromIndex:%lu\n",i+initialEdge,toIndex,fromIndex);
			//FIXME: Directed Networks
			CVNetworkGrowVertexSetEdgeForVertex(network,initialEdge,toIndex,fromIndex);
			//printf("OK\n");
		}else{
			CVNetworkGrowVertexSetInEdgeForVertex(network,initialEdge,toIndex,fromIndex);
		}
		network->edgesCount++;
	}
	return CVTrue;
}


void CVNetworkWriteToGMLFile(CVNetwork* theNetwork, FILE* networkFile){
	fprintf(networkFile, "graph [\n");
	if(theNetwork->directed){
		fprintf(networkFile, "  directed 1\n");
	}
	
	CVIndex i;
	for(i=0;i<theNetwork->verticesCount;i++){
		fprintf(networkFile,"  node [\n");
		
		fprintf(networkFile,"    id %"CVIndexScan"\n",i);
		if(theNetwork->vertexNames){
			fprintf(networkFile,"    label \"%s\"\n",theNetwork->vertexNames[i]);
		}
		
		CVIndex propertyIndex;
		for(propertyIndex=0;propertyIndex<theNetwork->propertiesCount;propertyIndex++){
			CVPropertyType type = theNetwork->propertiesTypes[propertyIndex];
			void* data = theNetwork->propertiesData[propertyIndex];
			CVString name = theNetwork->propertiesNames[propertyIndex];
			switch (type) {
				case CVNumberPropertyType:{
					CVFloat* floatData = data;
					fprintf(networkFile,"    ");
					CVBool nextUpper = CVFalse;
					while (*name) {
						if(isalnum(*name)){
							fputc(nextUpper?toupper(*name):*name, networkFile);
							nextUpper = CVFalse;
						}else if(isspace(*name)){
							nextUpper=CVTrue;
						}
						name++;
					}
					fprintf(networkFile," ");
					fprintf(networkFile,"%"CVFloatScan"\n",floatData[i]);
					break;
				}
				case CVStringPropertyType:{
					CVString* stringData = data;
					fprintf(networkFile,"    ");
					CVBool nextUpper = CVFalse;
					while (*name) {
						if(isalnum(*name)){
							fputc(nextUpper?toupper(*name):*name, networkFile);
							nextUpper = CVFalse;
						}else if(isspace(*name)){
							nextUpper=CVTrue;
						}
						name++;
					}
					fprintf(networkFile," ");
					fprintf(networkFile,"\"%s\"\n",stringData[i]);
					break;
				}
				case CVVector2DPropertyType:{
					CVFloat* floatData = data;
					fprintf(networkFile,"    ");
					CVBool nextUpper = CVFalse;
					while (*name) {
						if(isalnum(*name)){
							fputc(nextUpper?toupper(*name):*name, networkFile);
							nextUpper = CVFalse;
						}else if(isspace(*name)){
							nextUpper=CVTrue;
						}
						name++;
					}
					fprintf(networkFile," ");
					fprintf(networkFile,"[ x %"CVFloatScan" y %"CVFloatScan" ]\n",floatData[i*2],floatData[i*2+1]);
					break;
				}
				case CVVector3DPropertyType:{
					CVFloat* floatData = data;
					if(strcmp(name, "Position")==0 || strcmp(name, "position")==0 ){
						fprintf(networkFile,"    graphics");
					}else{
						fprintf(networkFile,"    ");
						CVBool nextUpper = CVFalse;
						while (*name) {
							if(isalnum(*name)){
								fputc(nextUpper?toupper(*name):*name, networkFile);
								nextUpper = CVFalse;
							}else if(isspace(*name)){
								nextUpper=CVTrue;
							}
							name++;
						}
					}
					fprintf(networkFile," ");
					fprintf(networkFile,"[ x %"CVFloatScan" y %"CVFloatScan" z %"CVFloatScan" ]\n",floatData[i*3],floatData[i*3+1],floatData[i*3+2]);
					break;
				}
				default:{
				}
			}
		}
		
		fprintf(networkFile,"  ]\n");
	}
	CVIndex edgeIndex;
	
	CVIndex* edgesFrom = theNetwork->edgeFromList;
	CVIndex* edgesTo = theNetwork->edgeToList;
	CVFloat* edgesWeights = theNetwork->edgesWeights;
	for (edgeIndex=0; edgeIndex<theNetwork->edgesCount; edgeIndex++) {
		fprintf(networkFile,"  edge [\n");
		CVIndex fromVertex = edgesFrom[edgeIndex];
		CVIndex toVertex = edgesTo[edgeIndex];
		if(theNetwork->edgeWeighted){
			CVFloat weight = edgesWeights[edgeIndex];
			fprintf(networkFile,"    source %"CVIndexScan"\n    target %"CVIndexScan" \n    weight %"CVFloatScan"\n",fromVertex,toVertex,weight);
		}else{
			fprintf(networkFile,"    source %"CVIndexScan"\n    target %"CVIndexScan"\n",fromVertex,toVertex);
		}
		fprintf(networkFile,"  ]\n");
	}
	
	fprintf(networkFile,"]\n");
}


CVFloat CVNetworkClusteringCoefficient(const CVNetwork* aNetwork, CVIndex nodeIndex){
	CVSize vertexEdgesCount = aNetwork->vertexNumOfEdges[nodeIndex];
	CVIndex* vertexEdgesList = aNetwork->vertexEdgesLists[nodeIndex];
	CVSize inLevelConnections = 0;
	CVIndex ni;
	CVBitArray isNeighbor = CVNewBitArray(aNetwork->verticesCount);
	for(ni=0;ni<vertexEdgesCount;ni++){
		CVBitArraySet(isNeighbor, vertexEdgesList[ni]);
	}
	for(ni=0;ni<vertexEdgesCount;ni++){
		CVIndex neighborVertex = vertexEdgesList[ni];
		CVSize neighborEdgesCount = aNetwork->vertexNumOfEdges[neighborVertex];
		CVIndex* neighborEdgesList = aNetwork->vertexEdgesLists[neighborVertex];
		CVIndex nni;
		for(nni=0;nni<neighborEdgesCount;nni++){
			if(CVBitArrayTest(isNeighbor,neighborEdgesList[nni])){
				inLevelConnections++;
			}
		}
	}
	CVBitArrayDestroy(isNeighbor);
	if((vertexEdgesCount-1.0) > 0.0){
		return (inLevelConnections)/(CVFloat)(vertexEdgesCount*(vertexEdgesCount-1.0f));
	}else{
		return 0.0f;
	}
}


CVNetwork* CVNewNetworkHomogeneusModel(CVSize verticesCount,CVSize degree){
	
	CVIndex try = 0;
	CVSize maxTries = 1000;
	CVSize remaining = 0;
	CVNetwork* network = NULL;
	CVSize originalVerticesCount = verticesCount;
	for (try=0; try<maxTries; try++) {
		verticesCount = originalVerticesCount+(try/20);
		CVSize i;
		network = CVNewNetwork(verticesCount, CVFalse, CVFalse);

		CVUIntegerArray enabledVertices;
		CVUIntegerArrayInitWithCapacity(verticesCount, &enabledVertices);

		CVUIntegerArray verticesDegree;
		CVUIntegerArrayInitWithCapacity(verticesCount, &verticesDegree);
		CVUIntegerArraySetCount(verticesCount, &verticesDegree);

		for (i=0; i<verticesCount; i++) {
			CVUIntegerArrayAdd(i, &enabledVertices);
		}

		struct __cv_edge{
			CVIndex from;
			CVIndex to;
		};
		struct __cv_edge_element{
			struct __cv_edge edge;
			UT_hash_handle hh;
		};

		struct __cv_edge_element* edgesHash = NULL;


		for (i=0; i<verticesCount; i++) {
			//printf("Adding node: %"CVUIntegerScan"\n",i);
			while(verticesDegree.data[i]<degree){
				CVIndex choice = i;
				CVBool edgeExists = CVTrue;
				//printf("\tAdding edge (%"CVUIntegerScan"/%"CVUIntegerScan")\n",verticesDegree.data[i]+1,degree);
				while(choice==i || edgeExists){
					choice = enabledVertices.data[CVRandomInRange(0, enabledVertices.count)];
					//printf("\t\tEnabled: [");
					//CVIndex ii;
					//for(ii=0;ii<enabledVertices.count;ii++){
					//	if(ii){
							//printf(", ");
					//	}
						//printf("%"CVUIntegerScan,enabledVertices.data[ii]);
					//}
					//printf("]\n");
					//printf("\t\tTesting: %"CVUIntegerScan"\n",choice);
					
					struct __cv_edge edgesKey;
					struct __cv_edge_element* edgeElement = NULL;
					
					edgesKey.from = CVMIN(i, choice);
					edgesKey.to = CVMAX(i, choice);
					
					
					HASH_FIND(hh, edgesHash, &edgesKey, sizeof(struct __cv_edge), edgeElement);
					//printf("\t\t\tElement: %p\n",edgeElement);
					if(edgeElement || edgesKey.from==edgesKey.to){
						edgeExists=CVTrue;
					//	printf("\t\t\tFAIL: %"CVUIntegerScan"\n",choice);
					}else{
						edgeExists=CVFalse;
					//	printf("\t\t\tSUCCESS: %"CVUIntegerScan"\n",choice);
					}
					if(edgeExists&&enabledVertices.count<degree){
						choice=i;
						break;
					}
				}
				
				if(i==choice){
					break;
				}
				
				
				struct __cv_edge_element* edgeElement = calloc(1, sizeof(struct __cv_edge_element));
				
				edgeElement->edge.from = CVMIN(i, choice);
				edgeElement->edge.to = CVMAX(i, choice);
				
				HASH_ADD_KEYPTR(hh, edgesHash, (&(edgeElement->edge)), sizeof(struct __cv_edge), edgeElement);
				
				//printf("\t\t\tAdding: %"CVUIntegerScan"\n",choice);
				
				//printf("\t\t\tHash:[");
				
		//			CVBool oi = CVTrue;
		//			struct __cv_edge_element* s;
		//			for(s=edgesHash; s != NULL; s=s->hh.next) {
		//				if(oi){
		//					oi=CVFalse;
		//				}else{
		//					printf(", ");
		//				}
		//				printf("%"CVUIntegerScan"-%"CVUIntegerScan, s->edge.from, s->edge.to);
		//			}
		//			printf("]\n");
				
				CVNetworkAddNewEdge(network, i, choice, 1.0);
				
				verticesDegree.data[i]++;
				verticesDegree.data[choice]++;
				
				CVIndex j;
				CVIndex current = 0;
				for (j=0; j<enabledVertices.count; j++) {
					if(verticesDegree.data[enabledVertices.data[j]]<degree){
						enabledVertices.data[current] = enabledVertices.data[j];
						current++;
					}
				}
				enabledVertices.count = current;
				
				if(enabledVertices.count<degree){
					break;
				}
			}
		}
		remaining=enabledVertices.count;
		printf("Remaining\t%"CVUIntegerScan"\n",enabledVertices.count);

		struct __cv_edge_element* edgeElement, *tempElement;

		HASH_ITER(hh, edgesHash, edgeElement, tempElement) {
			HASH_DEL(edgesHash, edgeElement);
			free(edgeElement);
		}
			
		if(remaining==0){
			break;
		}
	}
	return network;
}





























CVNetwork* CVNewNetworkRemoveChains(CVIndex* fromList,CVIndex* toList, CVSize edgesCount, CVSize verticesCount, CVBool directed, CVFloat* positions,CVFloat minRectangleSize, CVFloat maxRectangleSize,CVSize rectangleCount,CVFloat removeProbability){
	CVIndex edgeIndex = 0;
	CVSize newEdgesCount = 0;
	
	CVIndex i;
	CVFloat positionXMax = CVFloatMIN;
	CVFloat positionXMin = CVFloatMAX;
	CVFloat positionYMax = CVFloatMIN;
	CVFloat positionYMin = CVFloatMAX;
	
	for (i=0; i<verticesCount; i++) {
		CVFloat x = positions[3*i];
		CVFloat y = positions[3*i+1];
		positionXMax = CVMAX(positionXMax, x);
		positionXMin = CVMIN(positionXMin, x);
		positionYMax = CVMAX(positionYMax, y);
		positionYMin = CVMIN(positionYMin, y);
	}
	CVFloat* rectangles = calloc(rectangleCount*4, sizeof(CVFloat));
	
	for (i=0; i<rectangleCount; i++) {
		CVFloat cx = positionXMin + CVRandomFloat()*(positionXMax-positionXMin);
		CVFloat cy = positionYMin + CVRandomFloat()*(positionYMax-positionYMin);
		
		CVFloat sizex = (minRectangleSize + (maxRectangleSize-minRectangleSize)*CVRandomFloat())*(positionXMax-positionXMin);
		CVFloat sizey = (minRectangleSize + (maxRectangleSize-minRectangleSize)*CVRandomFloat())*(positionYMax-positionYMin);
		
		rectangles[i*4 + 0] = cx;
		rectangles[i*4 + 1] = cy;
		rectangles[i*4 + 2] = sizex;
		rectangles[i*4 + 3] = sizey;
		//printf("xy = (%f, %f)    rect = [%f, %f, %f, %f]\n",0.1,1.2,cx-sizex*0.5f,cy-sizey*0.5f,cx+sizex*0.5f,cy+sizey*0.5f);
	}
	
	double* probabilityVertices = calloc(verticesCount,sizeof(double));
	CVIndex j;
	for (j=0; j<verticesCount; j++) {
		probabilityVertices[j] = 1.0;
		CVFloat x = positions[3*j];
		CVFloat y = positions[3*j+1];
		for (i=0; i<rectangleCount; i++) {
			CVFloat cx = rectangles[i*4 + 0];
			CVFloat cy = rectangles[i*4 + 1];
			CVFloat sizex = rectangles[i*4 + 2];
			CVFloat sizey = rectangles[i*4 + 3];
			//printf("xy = (%f, %f)    rect = [%f, %f, %f, %f]\n",x,y,cx-sizex*0.5f,cy-sizey*0.5f,cx+sizex*0.5f,cy+sizey*0.5f);
			
			if(cx-sizex*0.5f <= x && x <= cx+sizex*0.5f && cy-sizey*0.5f <= y && y <= cy+sizey*0.5f){
				probabilityVertices[j] *= (1.0-removeProbability);
			}
		}
	}
	
	for (edgeIndex=0; edgeIndex<edgesCount; edgeIndex++) {
		//printf("%f\n",probabilityVertices[fromList[edgeIndex]] * probabilityVertices[fromList[edgeIndex]]);
		if(CVRandomFloat() < probabilityVertices[fromList[edgeIndex]] * probabilityVertices[toList[newEdgesCount]]){ //not remove
			fromList[newEdgesCount] = fromList[edgeIndex];
			toList[newEdgesCount] = toList[edgeIndex];
			newEdgesCount++;
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, directed);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, newEdgesCount);
	
	free(rectangles);
	free(probabilityVertices);
	return theNetwork;
}

CVNetwork* CVNewNetworkRemovingChains(const CVNetwork* originalNetwork){
	CVNetwork* theNetwork =NULL;
	CVFloat* positions = NULL;
	CVSize verticesCount = originalNetwork->verticesCount;
	CVSize edgesCount = originalNetwork->edgesCount;
	CVPropertyType positionType = CVUnknownPropertyType;
	positions = CVNetworkPropertyWithName(originalNetwork, "Position", &positionType);
	
	CVIndex* fromList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	CVIndex* toList = calloc(originalNetwork->edgesCount, sizeof(CVIndex));
	
	CVSize chains = 0;
	CVBitArray removedEdges = CVNewBitArray(edgesCount);
	CVBitArray modifiedEdges = CVNewBitArray(edgesCount);
	do{
		chains = 0;
		CVBitArrayClearAll(removedEdges, edgesCount);
		CVBitArrayClearAll(modifiedEdges, edgesCount);
		CVIndex edgeIndex = 0;
		CVSize newEdgesCount = 0;
		const CVNetwork* currentNetwork;
		
		if(theNetwork){
			currentNetwork = theNetwork;
		}else{
			currentNetwork = originalNetwork;
		}
		
		memcpy(fromList, currentNetwork->edgeFromList, sizeof(CVSize)*currentNetwork->edgesCount);
		memcpy(toList, currentNetwork->edgeToList, sizeof(CVSize)*currentNetwork->edgesCount);
		
		CVIndex vi;
		for (vi=0; vi<verticesCount; vi++) {
			CVSize neighCount = currentNetwork->vertexNumOfEdges[vi];
			if(neighCount==2){
				CVIndex* neigh = currentNetwork->vertexEdgesLists[vi];
				CVIndex vertex1 = neigh[0];
				CVIndex vertex2 = neigh[1];
				CVIndex edgeIndex1 = currentNetwork->vertexEdgesIndices[vi][0];
				CVIndex edgeIndex2 = currentNetwork->vertexEdgesIndices[vi][1];
				if(!CVBitArrayTest(removedEdges, edgeIndex1) && !CVBitArrayTest(removedEdges, edgeIndex1) &&
				   !CVBitArrayTest(modifiedEdges, edgeIndex1) && !CVBitArrayTest(modifiedEdges, edgeIndex1) &&
				   !CVNetworkAreAdjacent(currentNetwork, vertex1, vertex2)){
					fromList[edgeIndex1] = vertex1;
					toList[edgeIndex1] = vertex2;
					CVBitArraySet(modifiedEdges, edgeIndex1);
					CVBitArraySet(removedEdges, edgeIndex2);
					chains++;
				}
			}
		}
		
		
		for (edgeIndex=0; edgeIndex<currentNetwork->edgesCount; edgeIndex++) {
			if(!CVBitArrayTest(removedEdges, edgeIndex)){
				fromList[newEdgesCount] = fromList[edgeIndex];
				toList[newEdgesCount] = toList[edgeIndex];
				newEdgesCount++;
			}
		}
		
		if(theNetwork){
			free(theNetwork);
		}
		theNetwork = CVNewNetwork(verticesCount, CVFalse, originalNetwork->directed);
		CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, newEdgesCount);
		edgesCount = theNetwork->edgesCount;
		printf("chains:%"CVSizeScan"\n",chains);
	}while(chains>0);
	
	if(positions && positionType==CVVector3DPropertyType && theNetwork){
		CVNetworkAppendProperty(theNetwork, "Position", CVVector3DPropertyType, positions);
	}
	
	free(fromList);
	free(toList);
	CVBitArrayDestroy(removedEdges);
	CVBitArrayDestroy(modifiedEdges);
	return theNetwork;
	
}



CVSize CVNetworkNumberOfConnectedComponents(const CVNetwork* theNetwork, CVGenericArray* connectedComponents){
	CVSize verticesCount = theNetwork->verticesCount;
	CVSize edgesCount = theNetwork->edgesCount;
	
	CVUIntegerArray groups;
	CVUIntegerArrayInitWithCapacity(verticesCount, &groups);
	CVUIntegerArraySetCount(verticesCount, &groups);
	
	CVUIntegerArray edgesGroups;
	CVUIntegerArrayInitWithCapacity(edgesCount, &edgesGroups);
	CVUIntegerArraySetCount(edgesCount, &edgesGroups);
	
	CVUIntegerArray vertexList;
	CVUIntegerArrayInitWithCapacity(verticesCount, &vertexList);
	CVUIntegerArraySetCount(verticesCount, &vertexList);
	
	CVBitArray visited = CVNewBitArray(verticesCount);
	CVBitArray inList = CVNewBitArray(verticesCount);
	
	
	CVSize numGroups = 0;
	CVIndex vIndex;
	CVSize largestGroup=0;
	CVSize largestGroupSize=0;
	CVSize currentGroupSize=0;
	
	for(vIndex=0;vIndex<verticesCount;vIndex++){
		if(!CVBitArrayTest(visited, vIndex)){
			numGroups++;
			groups.data[vIndex]=numGroups-1;
			currentGroupSize++;
			CVSize listSize=1;
			CVIndex currentVertex;
			vertexList.data[0]=vIndex;
			CVBitArrayClearAll(inList, verticesCount);
			CVBitArraySet(inList, vIndex);
			while(listSize>0){
				currentVertex=vertexList.data[listSize-1];
				if(!CVBitArrayTest(visited, currentVertex)){
					CVBitArraySet(visited, currentVertex);
					
					CVSize edgesCount = theNetwork->vertexNumOfEdges[currentVertex];
					CVIndex* verticesList = theNetwork->vertexEdgesLists[currentVertex];
					CVIndex* edgesIndices = theNetwork->vertexEdgesIndices[currentVertex];
					
					CVIndex edgeIndex;
					
					CVBool addedToGroup = CVFalse;
					for(edgeIndex=0;edgeIndex<edgesCount;edgeIndex++){
						CVIndex linkedVertex = verticesList[edgeIndex];
						
						edgesGroups.data[edgesIndices[edgeIndex]] = numGroups-1;
						
						if(!CVBitArrayTest(inList,linkedVertex) && !CVBitArrayTest(visited,linkedVertex)){
							vertexList.data[listSize]=linkedVertex;
							listSize++;
							
							CVBitArraySet(inList, linkedVertex);
							
							groups.data[linkedVertex]=numGroups-1;
							currentGroupSize++;
							
							addedToGroup = CVTrue;
						}
					}
					if(theNetwork->directed){
						CVSize edgesCount = theNetwork->vertexNumOfInEdges[currentVertex];
						CVIndex* verticesList = theNetwork->vertexInEdgesLists[currentVertex];
						CVIndex* edgesIndices = theNetwork->vertexInEdgesIndices[currentVertex];
						
						CVIndex edgeIndex;
						//NSLog(@" Links from node %d listsize %d",currentVertex,listSize);
						for(edgeIndex=0;edgeIndex<edgesCount;edgeIndex++){
							CVIndex linkedVertex = verticesList[edgeIndex];
							
							edgesGroups.data[edgesIndices[edgeIndex]] = numGroups-1;
							
							if(!CVBitArrayTest(inList,linkedVertex) && !CVBitArrayTest(visited,linkedVertex)){
								vertexList.data[listSize]=linkedVertex;
								listSize++;
								
								CVBitArraySet(inList, linkedVertex);
								
								groups.data[linkedVertex]=numGroups-1;
								currentGroupSize++;
								
								addedToGroup = CVTrue;
							}
						}
					}
					if(!addedToGroup){
						CVBitArrayClear(inList, vertexList.data[listSize-1]);
						listSize--;
					}
				}else{
					CVBitArrayClear(inList, vertexList.data[listSize-1]);
					listSize--;
				}
			}
			if(currentGroupSize>largestGroupSize){
				largestGroupSize=currentGroupSize;
				largestGroup=numGroups-1;
			}
		}
	}
	
	if(connectedComponents){
		if(!connectedComponents->data){
			CVGenericArrayInitWithCapacity(numGroups, connectedComponents);
		}
		connectedComponents->count = 0;
	
		
		CVFloatArray* subVerticesLists = calloc(numGroups, sizeof(CVFloatArray));
		CVUIntegerArray* subEdgesFrom = calloc(numGroups, sizeof(CVUIntegerArray));
		CVUIntegerArray* subEdgesTo = calloc(numGroups, sizeof(CVUIntegerArray));
		CVFloatArray* subEdgesWeight = NULL;
		
		if(theNetwork->edgeWeighted){
			subEdgesWeight = calloc(numGroups, sizeof(CVFloatArray));
		}
		
		CVUIntegerArray newVerticesIndices;
		CVUIntegerArrayInitWithCapacity(verticesCount, &newVerticesIndices);
		CVUIntegerArraySetCount(verticesCount, &newVerticesIndices);
		
		CVIndex i;
	
		for(i=0;i<numGroups;i++){
			CVFloatArrayInitWithCapacity(10, subVerticesLists+i);
			CVUIntegerArrayInitWithCapacity(10, subEdgesFrom+i);
			CVUIntegerArrayInitWithCapacity(10, subEdgesTo+i);
			if(theNetwork->edgeWeighted){
				CVFloatArrayInitWithCapacity(10, subEdgesWeight+i);
			}
		}
	
		
		for(i=0;i<verticesCount;i++){
			CVIndex group = groups.data[i];
			newVerticesIndices.data[i] = subVerticesLists[group].count;
			CVFloatArrayAdd(i, subVerticesLists+group);
		}
		
		for(i=0;i<edgesCount;i++){
			CVIndex fromVertex = theNetwork->edgeFromList[i];
			CVIndex toVertex = theNetwork->edgeToList[i];
			
			if(edgesGroups.data[i]==groups.data[fromVertex]&&edgesGroups.data[i]==groups.data[toVertex]){
				CVUIntegerArrayAdd(newVerticesIndices.data[fromVertex], subEdgesFrom + edgesGroups.data[i]);
				CVUIntegerArrayAdd(newVerticesIndices.data[toVertex], subEdgesTo + edgesGroups.data[i]);
			}
			if(theNetwork->edgeWeighted){
				CVFloatArrayAdd(theNetwork->edgesWeights[i], subEdgesWeight + edgesGroups.data[i]);
			}
		}
		
		for(i=0;i<numGroups;i++){
			CVSize groupVerticesCount = subVerticesLists[i].count;
			CVNetwork* groupNetwork = CVNewNetwork(groupVerticesCount, theNetwork->edgeWeighted, theNetwork->directed);
			if(!theNetwork->edgeWeighted){
				CVNetworkAddNewEdges(groupNetwork, subEdgesFrom[i].data, subEdgesTo[i].data, NULL, subEdgesFrom[i].count);
			}else{
				CVNetworkAddNewEdges(groupNetwork, subEdgesFrom[i].data, subEdgesTo[i].data, subEdgesWeight[i].data, subEdgesFrom[i].count);
			}
			CVNetworkAppendProperty(groupNetwork, "Original Index", CVNumberPropertyType, subVerticesLists[i].data);
			CVGenericArrayAdd(groupNetwork, connectedComponents);
		}
		
		for(i=0;i<numGroups;i++){
			CVFloatArrayDestroy(subVerticesLists+i);
			CVUIntegerArrayDestroy(subEdgesFrom+i);
			CVUIntegerArrayDestroy(subEdgesTo+i);
			if(theNetwork->edgeWeighted){
				CVFloatArrayDestroy(subEdgesWeight+i);
			}
		}
		CVUIntegerArrayDestroy(&newVerticesIndices);
		
		free(subVerticesLists);
		free(subEdgesFrom);
		free(subEdgesTo);
		if(theNetwork->edgeWeighted){
			free(subEdgesWeight);
		}
	}
	
	CVUIntegerArrayDestroy(&groups);
	CVUIntegerArrayDestroy(&edgesGroups);
	CVUIntegerArrayDestroy(&vertexList);
	
	CVBitArrayDestroy(visited);
	CVBitArrayDestroy(inList);
	
	
	return numGroups;
}

CVNetwork* CVNewSubNetworkFromNetwork(const CVNetwork* theNetwork, const CVUIntegerArray verticesIndices){
	CVSize verticesCount = theNetwork->verticesCount;
	CVSize edgesCount = theNetwork->edgesCount;
	
	
	CVFloatArray largestVerticesList;
	CVUIntegerArray largestEdgesFrom;
	CVUIntegerArray largestEdgesTo;
	CVFloatArray largestEdgesWeight;
	
	CVFloatArrayInitWithCapacity(10, &largestVerticesList);
	CVUIntegerArrayInitWithCapacity(10, &largestEdgesFrom);
	CVUIntegerArrayInitWithCapacity(10, &largestEdgesTo);
	
	if(theNetwork->edgeWeighted){
		CVFloatArrayInitWithCapacity(10, &largestEdgesWeight);
	}
	
	CVUIntegerArray newVerticesIndices;
	CVUIntegerArrayInitWithCapacity(verticesCount, &newVerticesIndices);
	CVUIntegerArraySetCount(verticesCount, &newVerticesIndices);
	
	
	CVBitArray inSelected = CVNewBitArray(verticesCount);
	
	CVIndex i;
	
	for(i=0;i<verticesIndices.count;i++){
		CVBitArraySet(inSelected, verticesIndices.data[i]);
		newVerticesIndices.data[verticesIndices.data[i]] = i;
		CVFloatArrayAdd(verticesIndices.data[i], &largestVerticesList);
	}
	
	for(i=0;i<edgesCount;i++){
		CVIndex fromVertex = theNetwork->edgeFromList[i];
		CVIndex toVertex = theNetwork->edgeToList[i];
		
		if(CVBitArrayTest(inSelected, fromVertex) && CVBitArrayTest(inSelected, toVertex)){
			CVUIntegerArrayAdd(newVerticesIndices.data[fromVertex], &largestEdgesFrom);
			CVUIntegerArrayAdd(newVerticesIndices.data[toVertex], &largestEdgesTo);
		}
		if(theNetwork->edgeWeighted){
			CVFloatArrayAdd(theNetwork->edgesWeights[i], &largestEdgesWeight);
		}
	}
	
	CVSize groupVerticesCount = verticesIndices.count;
	
	CVNetwork* groupNetwork = CVNewNetwork(groupVerticesCount, theNetwork->edgeWeighted, theNetwork->directed);
	
	if(!theNetwork->edgeWeighted){
		CVNetworkAddNewEdges(groupNetwork, largestEdgesFrom.data, largestEdgesTo.data, NULL, largestEdgesFrom.count);
	}else{
		CVNetworkAddNewEdges(groupNetwork, largestEdgesFrom.data, largestEdgesTo.data, largestEdgesWeight.data, largestEdgesFrom.count);
	}
	
	CVNetworkAppendProperty(groupNetwork, "Original Index", CVNumberPropertyType, largestVerticesList.data);
	
	if(theNetwork->vertexNames){
		CVString* names = calloc(groupVerticesCount, sizeof(CVString));
		CVString* propertyData = theNetwork->vertexNames;
		for (i=0; i<groupVerticesCount; i++) {
			names[i] = propertyData[verticesIndices.data[i]];
		}
		CVNetworkAppendProperty(groupNetwork, "name", CVStringPropertyType, names);
		free(names);
	}
	
	CVIndex propIndex = 0;
	for (propIndex=0; propIndex< theNetwork->propertiesCount; propIndex++) {
		CVPropertyType propertyType = theNetwork->propertiesTypes[propIndex];
		CVString propertyName = theNetwork->propertiesNames[propIndex];
		
		CVIndex i;
		switch (propertyType) {
			case CVStringPropertyType:{
				CVString* values = calloc(groupVerticesCount, sizeof(CVString));
				CVString* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount; i++) {
					values[i] = propertyData[verticesIndices.data[i]];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			case CVNumberPropertyType:{
				CVFloat* values = calloc(groupVerticesCount, sizeof(CVFloat));
				CVFloat* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount; i++) {
					values[i] = propertyData[verticesIndices.data[i]];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			case CVVector2DPropertyType:{
				CVFloat* values = calloc(groupVerticesCount*2, sizeof(CVFloat));
				CVFloat* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount; i++) {
					values[i*2] = propertyData[verticesIndices.data[i]*2];
					values[i*2+1] = propertyData[verticesIndices.data[i]*2+1];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			case CVVector3DPropertyType:{
				CVFloat* values = calloc(groupVerticesCount*3, sizeof(CVFloat));
				CVFloat* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount; i++) {
					CVIndex selectedIndex = verticesIndices.data[i];
					values[i*3] = propertyData[selectedIndex*3];
					values[i*3+1] = propertyData[selectedIndex*3+1];
					values[i*3+2] = propertyData[selectedIndex*3+2];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			default:
				break;
		}
	}
	
	CVFloatArrayDestroy(&largestVerticesList);
	CVUIntegerArrayDestroy(&largestEdgesFrom);
	CVUIntegerArrayDestroy(&largestEdgesTo);
	
	if(theNetwork->edgeWeighted){
		CVFloatArrayDestroy(&largestEdgesWeight);
	}
	
	CVUIntegerArrayDestroy(&newVerticesIndices);
	CVBitArrayDestroy(inSelected);

	return groupNetwork;
}

CVNetwork* CVNewNetworkFromLargestComponent(const CVNetwork* theNetwork){
	CVSize verticesCount = theNetwork->verticesCount;
	CVSize edgesCount = theNetwork->edgesCount;
	
	CVUIntegerArray groups;
	CVUIntegerArrayInitWithCapacity(verticesCount, &groups);
	CVUIntegerArraySetCount(verticesCount, &groups);
	
	CVUIntegerArray edgesGroups;
	CVUIntegerArrayInitWithCapacity(edgesCount, &edgesGroups);
	CVUIntegerArraySetCount(edgesCount, &edgesGroups);
	
	CVUIntegerArray vertexList;
	CVUIntegerArrayInitWithCapacity(verticesCount, &vertexList);
	CVUIntegerArraySetCount(verticesCount, &vertexList);
	
	CVBitArray visited = CVNewBitArray(verticesCount);
	CVBitArray inList = CVNewBitArray(verticesCount);
	
	
	CVSize numGroups = 0;
	CVIndex vIndex;
	CVSize largestGroup=0;
	CVSize largestGroupSize=0;
	CVSize currentGroupSize=0;
	
	for(vIndex=0;vIndex<verticesCount;vIndex++){
		if(!CVBitArrayTest(visited, vIndex)){
			numGroups++;
			groups.data[vIndex]=numGroups-1;
			currentGroupSize++;
			CVSize listSize=1;
			CVIndex currentVertex;
			vertexList.data[0]=vIndex;
			CVBitArrayClearAll(inList, verticesCount);
			CVBitArraySet(inList, vIndex);
			while(listSize>0){
				currentVertex=vertexList.data[listSize-1];
				if(!CVBitArrayTest(visited, currentVertex)){
					CVBitArraySet(visited, currentVertex);
					
					CVSize edgesCount = theNetwork->vertexNumOfEdges[currentVertex];
					CVIndex* verticesList = theNetwork->vertexEdgesLists[currentVertex];
					CVIndex* edgesIndices = theNetwork->vertexEdgesIndices[currentVertex];
					
					CVIndex edgeIndex;
					
					CVBool addedToGroup = CVFalse;
					for(edgeIndex=0;edgeIndex<edgesCount;edgeIndex++){
						CVIndex linkedVertex = verticesList[edgeIndex];
						
						edgesGroups.data[edgesIndices[edgeIndex]] = numGroups-1;
						
						if(!CVBitArrayTest(inList,linkedVertex) && !CVBitArrayTest(visited,linkedVertex)){
							vertexList.data[listSize]=linkedVertex;
							listSize++;
							
							CVBitArraySet(inList, linkedVertex);
							
							groups.data[linkedVertex]=numGroups-1;
							currentGroupSize++;
							
							addedToGroup = CVTrue;
						}
					}
					if(theNetwork->directed){
						CVSize edgesCount = theNetwork->vertexNumOfInEdges[currentVertex];
						CVIndex* verticesList = theNetwork->vertexInEdgesLists[currentVertex];
						CVIndex* edgesIndices = theNetwork->vertexInEdgesIndices[currentVertex];
						
						CVIndex edgeIndex;
						//NSLog(@" Links from node %d listsize %d",currentVertex,listSize);
						for(edgeIndex=0;edgeIndex<edgesCount;edgeIndex++){
							CVIndex linkedVertex = verticesList[edgeIndex];
							
							edgesGroups.data[edgesIndices[edgeIndex]] = numGroups-1;
							
							if(!CVBitArrayTest(inList,linkedVertex) && !CVBitArrayTest(visited,linkedVertex)){
								vertexList.data[listSize]=linkedVertex;
								listSize++;
								
								CVBitArraySet(inList, linkedVertex);
								
								groups.data[linkedVertex]=numGroups-1;
								currentGroupSize++;
								
								addedToGroup = CVTrue;
							}
						}
					}
					if(!addedToGroup){
						CVBitArrayClear(inList, vertexList.data[listSize-1]);
						listSize--;
					}
				}else{
					CVBitArrayClear(inList, vertexList.data[listSize-1]);
					listSize--;
				}
			}
			if(currentGroupSize>largestGroupSize){
				largestGroupSize=currentGroupSize;
				largestGroup=numGroups-1;
			}
		}
	}
	
	CVFloatArray largestVerticesList;
	CVUIntegerArray largestEdgesFrom;
	CVUIntegerArray largestEdgesTo;
	CVFloatArray largestEdgesWeight;
	
	CVFloatArrayInitWithCapacity(10, &largestVerticesList);
	CVUIntegerArrayInitWithCapacity(10, &largestEdgesFrom);
	CVUIntegerArrayInitWithCapacity(10, &largestEdgesTo);
	
	CVUIntegerArray verticesIndices;
	CVUIntegerArrayInitWithCapacity(10, &verticesIndices);
	
	if(theNetwork->edgeWeighted){
		CVFloatArrayInitWithCapacity(10, &largestEdgesWeight);
	}
	
	CVUIntegerArray newVerticesIndices;
	CVUIntegerArrayInitWithCapacity(verticesCount, &newVerticesIndices);
	CVUIntegerArraySetCount(verticesCount, &newVerticesIndices);
	
	
	
	CVIndex i;
	CVUIntegerArray groupSizes;
	CVUIntegerArrayInitWithCapacity(numGroups, &groupSizes);
	CVUIntegerArraySetCount(numGroups, &groupSizes);
	
	for(i=0;i<verticesCount;i++){
		CVIndex group = groups.data[i];
		newVerticesIndices.data[i] = groupSizes.data[group];
		groupSizes.data[group]++;
	}
	
	CVIndex largestComponentIndex = 0;
	CVSize largestComponentSize = 0;
	
	for(i=0;i<numGroups;i++){
		if(groupSizes.data[i]>largestComponentSize){
			largestComponentSize = groupSizes.data[i];
			largestComponentIndex = i;
		}
	}
	
	for(i=0;i<verticesCount;i++){
		CVIndex group = groups.data[i];
		if(group==largestComponentIndex){
			CVFloatArrayAdd(i, &largestVerticesList);
			CVUIntegerArrayAdd(i, &verticesIndices);
		}
	}
	
	for(i=0;i<edgesCount;i++){
		CVIndex fromVertex = theNetwork->edgeFromList[i];
		CVIndex toVertex = theNetwork->edgeToList[i];
		
		if(largestComponentIndex==groups.data[fromVertex] && largestComponentIndex==groups.data[toVertex]){
			CVUIntegerArrayAdd(newVerticesIndices.data[fromVertex], &largestEdgesFrom);
			CVUIntegerArrayAdd(newVerticesIndices.data[toVertex], &largestEdgesTo);
		}
		if(theNetwork->edgeWeighted){
			CVFloatArrayAdd(theNetwork->edgesWeights[i], &largestEdgesWeight);
		}
	}
	
	CVSize groupVerticesCount = groupSizes.data[largestComponentIndex];
	CVNetwork* groupNetwork = CVNewNetwork(groupVerticesCount, theNetwork->edgeWeighted, theNetwork->directed);
	
	if(!theNetwork->edgeWeighted){
		CVNetworkAddNewEdges(groupNetwork, largestEdgesFrom.data, largestEdgesTo.data, NULL, largestEdgesFrom.count);
	}else{
		CVNetworkAddNewEdges(groupNetwork, largestEdgesFrom.data, largestEdgesTo.data, largestEdgesWeight.data, largestEdgesFrom.count);
	}
	
	CVNetworkAppendProperty(groupNetwork, "Original Index", CVNumberPropertyType, largestVerticesList.data);
	
	if(theNetwork->vertexNames){
		CVString* names = calloc(groupVerticesCount, sizeof(CVString));
		CVString* propertyData = theNetwork->vertexNames;
		for (i=0; i<groupVerticesCount; i++) {
			names[i] = propertyData[verticesIndices.data[i]];
		}
		CVNetworkAppendProperty(groupNetwork, "name", CVStringPropertyType, names);
		free(names);
	}
	
	CVIndex propIndex = 0;
	for (propIndex=0; propIndex< theNetwork->propertiesCount; propIndex++) {
		CVPropertyType propertyType = theNetwork->propertiesTypes[propIndex];
		CVString propertyName = theNetwork->propertiesNames[propIndex];
		
		CVIndex i;
		switch (propertyType) {
			case CVStringPropertyType:{
				CVString* values = calloc(groupVerticesCount, sizeof(CVString));
				CVString* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount; i++) {
					values[i] = propertyData[verticesIndices.data[i]];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			case CVNumberPropertyType:{
				CVFloat* values = calloc(groupVerticesCount, sizeof(CVFloat));
				CVFloat* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount; i++) {
					values[i] = propertyData[verticesIndices.data[i]];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			case CVVector2DPropertyType:{
				CVFloat* values = calloc(groupVerticesCount*2, sizeof(CVFloat));
				CVFloat* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount*2; i+=2) {
					values[i] = propertyData[verticesIndices.data[i]*2];
					values[i+1] = propertyData[verticesIndices.data[i]*2+1];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			case CVVector3DPropertyType:{
				CVFloat* values = calloc(groupVerticesCount*3, sizeof(CVFloat));
				CVFloat* propertyData = theNetwork->propertiesData[propIndex];
				for (i=0; i<groupVerticesCount*3; i+=3) {
					values[i] = propertyData[verticesIndices.data[i]*3];
					values[i+1] = propertyData[verticesIndices.data[i]*3+1];
					values[i+2] = propertyData[verticesIndices.data[i]*3+2];
				}
				CVNetworkAppendProperty(groupNetwork, propertyName, propertyType, values);
				free(values);
				break;
			}
			default:
				break;
		}
	}
	
	CVFloatArrayDestroy(&largestVerticesList);
	CVUIntegerArrayDestroy(&largestEdgesFrom);
	CVUIntegerArrayDestroy(&largestEdgesTo);
	
	if(theNetwork->edgeWeighted){
		CVFloatArrayDestroy(&largestEdgesWeight);
	}
	
	CVUIntegerArrayDestroy(&newVerticesIndices);
	CVUIntegerArrayDestroy(&groupSizes);
	
	
	CVUIntegerArrayDestroy(&groups);
	CVUIntegerArrayDestroy(&edgesGroups);
	CVUIntegerArrayDestroy(&vertexList);
	CVUIntegerArrayDestroy(&verticesIndices);
	
	CVBitArrayDestroy(visited);
	CVBitArrayDestroy(inList);
	
	
	return groupNetwork;
}



CVSize CVNetworkLargestComponentSize(const CVNetwork* theNetwork, CVSize* connectedComponentsCount){
	CVSize verticesCount = theNetwork->verticesCount;
	CVSize edgesCount = theNetwork->edgesCount;
	
	CVUIntegerArray groups;
	CVUIntegerArrayInitWithCapacity(verticesCount, &groups);
	CVUIntegerArraySetCount(verticesCount, &groups);
	
	CVUIntegerArray edgesGroups;
	CVUIntegerArrayInitWithCapacity(edgesCount, &edgesGroups);
	CVUIntegerArraySetCount(edgesCount, &edgesGroups);
	
	CVUIntegerArray vertexList;
	CVUIntegerArrayInitWithCapacity(verticesCount, &vertexList);
	CVUIntegerArraySetCount(verticesCount, &vertexList);
	
	CVBitArray visited = CVNewBitArray(verticesCount);
	CVBitArray inList = CVNewBitArray(verticesCount);
	
	
	CVSize numGroups = 0;
	CVIndex vIndex;
	CVSize largestGroup=0;
	CVSize largestGroupSize=0;
	CVSize currentGroupSize=0;
	
	for(vIndex=0;vIndex<verticesCount;vIndex++){
		if(!CVBitArrayTest(visited, vIndex)){
			numGroups++;
			groups.data[vIndex]=numGroups-1;
			currentGroupSize++;
			CVSize listSize=1;
			CVIndex currentVertex;
			vertexList.data[0]=vIndex;
			CVBitArrayClearAll(inList, verticesCount);
			CVBitArraySet(inList, vIndex);
			while(listSize>0){
				currentVertex=vertexList.data[listSize-1];
				if(!CVBitArrayTest(visited, currentVertex)){
					CVBitArraySet(visited, currentVertex);
					
					CVSize edgesCount = theNetwork->vertexNumOfEdges[currentVertex];
					CVIndex* verticesList = theNetwork->vertexEdgesLists[currentVertex];
					CVIndex* edgesIndices = theNetwork->vertexEdgesIndices[currentVertex];
					
					CVIndex edgeIndex;
					
					CVBool addedToGroup = CVFalse;
					for(edgeIndex=0;edgeIndex<edgesCount;edgeIndex++){
						CVIndex linkedVertex = verticesList[edgeIndex];
						
						edgesGroups.data[edgesIndices[edgeIndex]] = numGroups-1;
						
						if(!CVBitArrayTest(inList,linkedVertex) && !CVBitArrayTest(visited,linkedVertex)){
							vertexList.data[listSize]=linkedVertex;
							listSize++;
							
							CVBitArraySet(inList, linkedVertex);
							
							groups.data[linkedVertex]=numGroups-1;
							currentGroupSize++;
							
							addedToGroup = CVTrue;
						}
					}
					if(theNetwork->directed){
						CVSize edgesCount = theNetwork->vertexNumOfInEdges[currentVertex];
						CVIndex* verticesList = theNetwork->vertexInEdgesLists[currentVertex];
						CVIndex* edgesIndices = theNetwork->vertexInEdgesIndices[currentVertex];
						
						CVIndex edgeIndex;
						//NSLog(@" Links from node %d listsize %d",currentVertex,listSize);
						for(edgeIndex=0;edgeIndex<edgesCount;edgeIndex++){
							CVIndex linkedVertex = verticesList[edgeIndex];
							
							edgesGroups.data[edgesIndices[edgeIndex]] = numGroups-1;
							
							if(!CVBitArrayTest(inList,linkedVertex) && !CVBitArrayTest(visited,linkedVertex)){
								vertexList.data[listSize]=linkedVertex;
								listSize++;
								
								CVBitArraySet(inList, linkedVertex);
								
								groups.data[linkedVertex]=numGroups-1;
								currentGroupSize++;
								
								addedToGroup = CVTrue;
							}
						}
					}
					if(!addedToGroup){
						CVBitArrayClear(inList, vertexList.data[listSize-1]);
						listSize--;
					}
				}else{
					CVBitArrayClear(inList, vertexList.data[listSize-1]);
					listSize--;
				}
			}
			if(currentGroupSize>largestGroupSize){
				largestGroupSize=currentGroupSize;
				largestGroup=numGroups-1;
			}
		}
	}
	
	CVFloatArray largestVerticesList;
	CVUIntegerArray largestEdgesFrom;
	CVUIntegerArray largestEdgesTo;
	CVFloatArray largestEdgesWeight;
	
	CVFloatArrayInitWithCapacity(10, &largestVerticesList);
	CVUIntegerArrayInitWithCapacity(10, &largestEdgesFrom);
	CVUIntegerArrayInitWithCapacity(10, &largestEdgesTo);
	
	if(theNetwork->edgeWeighted){
		CVFloatArrayInitWithCapacity(10, &largestEdgesWeight);
	}
	
	
	CVIndex i;
	CVUIntegerArray groupSizes;
	CVUIntegerArrayInitWithCapacity(numGroups, &groupSizes);
	CVUIntegerArraySetCount(numGroups, &groupSizes);
	
	for(i=0;i<verticesCount;i++){
		CVIndex group = groups.data[i];
		groupSizes.data[group]++;
	}
	
	CVIndex largestComponentIndex = 0;
	CVSize largestComponentSize = 0;
	
	for(i=0;i<numGroups;i++){
		if(groupSizes.data[i]>largestComponentSize){
			largestComponentSize = groupSizes.data[i];
			largestComponentIndex = i;
		}
	}
	
	CVUIntegerArrayDestroy(&groupSizes);
	
	
	CVUIntegerArrayDestroy(&groups);
	CVUIntegerArrayDestroy(&edgesGroups);
	CVUIntegerArrayDestroy(&vertexList);
	
	CVBitArrayDestroy(visited);
	CVBitArrayDestroy(inList);
	
	if(connectedComponentsCount){
		*connectedComponentsCount = numGroups;
	}
	
	return largestComponentSize;
}


CVNetwork* CVNewNetworkFromAdjacencyMatrix(const CVBitArray adjacencyMatrix, CVSize verticesCount, CVBool directed){
	CVSize edgesCapacity = 2;
	CVIndex* fromList = calloc(edgesCapacity, sizeof(CVIndex));
	CVIndex* toList = calloc(edgesCapacity, sizeof(CVIndex));
	CVSize edgesCount = 0;
	
	for (CVIndex fromIndex=0; fromIndex<verticesCount; fromIndex++) {
		CVSize startAt = directed?0:(fromIndex+1);
		for (CVIndex toIndex=startAt; toIndex<verticesCount; toIndex++) {
			if(CVBitArrayTest(adjacencyMatrix, fromIndex*verticesCount+toIndex)){
				if(edgesCapacity<edgesCount+1){
					edgesCapacity = CVCapacityGrow(edgesCount+1);
					fromList = realloc(fromList, sizeof(CVIndex)*edgesCapacity);
					toList = realloc(toList, sizeof(CVIndex)*edgesCapacity);
				}
				fromList[edgesCount] = fromIndex;
				toList[edgesCount] = toIndex;
				edgesCount++;
			}
		}
	}
	
	CVNetwork* theNetwork = CVNewNetwork(verticesCount, CVFalse, directed);
	CVNetworkAddNewEdges(theNetwork, fromList, toList, NULL, edgesCount);
	free(fromList);
	free(toList);
	return theNetwork;
}


CVBitArray CVNewAdjacencyMatrixFromNetwork(const CVNetwork* theNetwork){
	CVSize verticesCount = theNetwork->verticesCount;
	CVBitArray adjacencyMatrix = CVNewBitArray(verticesCount*verticesCount);
	CVIndex* fromList = theNetwork->edgeFromList;
	CVIndex* toList = theNetwork->edgeToList;
	
	for (CVIndex edgeIndex=0; edgeIndex<theNetwork->edgesCount; edgeIndex++) {
		CVBitArraySet(adjacencyMatrix, fromList[edgeIndex]*verticesCount+toList[edgeIndex]);
		if(!theNetwork->directed){
			CVBitArraySet(adjacencyMatrix, toList[edgeIndex]*verticesCount+fromList[edgeIndex]);
		}
	}
	return adjacencyMatrix;
}




