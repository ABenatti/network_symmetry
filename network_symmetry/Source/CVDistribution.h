//
//  CVDistribution.h
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 20/02/14.
//  Copyright (c) 2014 Filipi Nascimento Silva. All rights reserved.
//

#ifndef CVNetwork_CVDistribution_h
#define CVNetwork_CVDistribution_h
#include "CVCommons.h"



typedef struct{
	CVFloat* distribution;
	CVFloat* data;
	CVSize count;
	double* tree;
	double* cumulative;
} CVDistribution;


CVDistribution* CVCreateDistribution(const CVFloat* probabilities, const CVFloat* data, CVSize count);
void CVDestroyDistribution(CVDistribution* distribution);



CV_INLINE CVInteger CVDistributionIndexForChoice(CVDistribution* distribution,double choice){
	if(CVUnlikely(choice>=1.0)){
		return distribution->count-1;
	}else if(CVUnlikely(choice<0.0)){
		return 0;
	}
	CVSize n = (CVInteger)CVNextPowerOfTwo(distribution->count);
	double* tree = distribution->tree;
	CVIndex current = 0;
	while (current<n-1) {
		if(choice<tree[current]){
			current = (current+1)*2 - 1;
		}else{
			current = (current+1)*2;
		}
	}
	return current-(n-1);
}


CV_INLINE CVFloat CVDistributionValueForChoice(CVDistribution* distribution,double choice){
	CVInteger index = CVDistributionIndexForChoice(distribution,choice);
	if(distribution->data){
		return distribution->data[index];
	}else{
		return (CVFloat)index;
	}
}

CV_INLINE CVInteger CVDistributionRandomIndex(CVDistribution* distribution){
	return CVDistributionIndexForChoice(distribution, CVRandomFloat());
}

CV_INLINE CVFloat CVDistributionRandomValue(CVDistribution* distribution){
	return CVDistributionValueForChoice(distribution, CVRandomFloat());
}

void CVTestDistribution();

#endif
