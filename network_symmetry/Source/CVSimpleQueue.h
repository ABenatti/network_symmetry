//
//  CVSimpleCVQueue.h
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 11/23/12.
//  Copyright (c) 2012 Filipi Nascimento Silva. All rights reserved.
//

#ifndef CVNetwork_CVSimpleCVQueue_h
#define CVNetwork_CVSimpleCVQueue_h
#include "CVCommons.h"


typedef struct CVQueueNode {
	CVInteger item;
	struct CVQueueNode* next;
} CVQueueNode;

typedef struct CVQueue {
	CVQueueNode* head;
	CVQueueNode* tail;
	
	void (*push) (struct CVQueue*, CVInteger);
	
	CVInteger (*pop) (struct CVQueue*);
	
	CVInteger (*peek) (struct CVQueue*);
	
	CVInteger size;
	CVQueueNode** nodesPool;
	CVInteger nodesPoolCount;
	CVInteger nodesPoolCapacity;
	
} CVQueue;

void CVQueuePush (CVQueue* queue, CVInteger item);

CVInteger CVQueuePop (CVQueue* queue);

CVInteger CVQueuePeek (CVQueue* queue);

CVQueue CVQueueCreate ();
CVBool CVQueueDequeue (CVQueue* queue, CVInteger *value);
void CVQueueDestroy (CVQueue* queue);


#endif