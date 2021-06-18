//
//  CVDistribution.c
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 20/02/14.
//  Copyright (c) 2014 Filipi Nascimento Silva. All rights reserved.
//

#include "CVDistribution.h"


CVDistribution* CVCreateDistribution(const CVFloat* probabilities, const CVFloat* data, CVSize count){
	CVIndex i;
	CVDistribution* distribution = calloc(1, sizeof(CVDistribution));
	
	distribution->count=count;
	distribution->distribution = calloc(count, sizeof(CVFloat));
	distribution->cumulative = calloc(count, sizeof(double));
	
	
	memcpy(distribution->distribution, probabilities, count*sizeof(CVFloat));
	
	if(data){
		distribution->data = calloc(count, sizeof(CVFloat));
		memcpy(distribution->data, data, count*sizeof(CVFloat));
	}
	
	double cumValue = 0.0;
	for(i=0;i<count;i++){
		cumValue += distribution->distribution[i];
		distribution->cumulative[i] =cumValue;
	}
	
	for(i=0;i<count;i++){
		distribution->distribution[i]/=cumValue;
		distribution->cumulative[i]/=cumValue;
	}
	
	CVIndex m = 0;
	CVSize n = (CVInteger)CVNextPowerOfTwo(count);
	CVIndex p2n = ilog2(n);
	
	distribution->tree = calloc(n-1, sizeof(double));
	
	for(m=0;m<n-1;m++){
		CVInteger lm = ilog2(m+1);
		CVInteger jm = p2n - lm-1;
		CVInteger im = m - (ipow2(lm)-1);
		CVInteger Kij = im*ipow2(jm+1) + ipow2(jm) - 1;
		if(Kij<count){
			distribution->tree[m] = distribution->cumulative[Kij];
		}else{
			distribution->tree[m] = 1.0;
		}
	}
	/*printf("-----\nGenerated Tree:\n----\n");
	for(m=0;m<n-1;m++){
		CVInteger lm = ilog2(m+1);
		CVInteger jm = p2n - lm-1;
		CVInteger im = m - (ipow2(lm)-1);
		CVInteger Kij = im*ipow2(jm+1) + ipow2(jm) - 1;
		printf("%"CVIntegerScan" ",Kij);
	}
	printf("\n-----\n");*/
	return distribution;
}


void CVDestroyDistribution(CVDistribution* distribution){
	distribution->count=0;
	free(distribution->cumulative);
	free(distribution->distribution);
	free(distribution->tree);
	free(distribution->data);
	free(distribution);
}


void CVTestDistribution(){
	const CVInteger nnn = 1000;
	
	CVFloat probs[nnn];
	CVInteger freqs[nnn];
	
	CVInteger m;
	for(m=0;m<nnn;m++){
		probs[m] = expf(-(m-500.0)*(m-500.0)/200.0/200.0);
		freqs[m] = 0;
	}
	
	CVDistribution* distrib = CVCreateDistribution(probs, NULL, nnn);
	
	CVRandomSeedDev();
	
	CVSize reps = 50000000;
	for(m=0;m<reps;m++){
		CVInteger index = CVDistributionRandomIndex(distrib);
		freqs[index]++;
	}
	
	for(m=0;m<distrib->count;m++){
		printf("%g\t%10g\n", m/(double)(distrib->count-1), freqs[m]/(double)reps);
	}
}

