//
//  CVSet.c
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 11/5/16.
//  Copyright © 2016 Filipi Nascimento Silva. All rights reserved.
//

#include "CVSet.h"



CVStringSetRef CVNewStringSet(){
	return calloc(1, sizeof(CVStringSet));
}

CVBool CVStringSetHas(const CVStringSetRef set, const CVString key){
	CVStringSetEntry* entry = NULL;
	if(set){
		HASH_FIND_STR((*set), key, entry);
	}
	if(entry){
		return CVTrue;
	}else{
		return CVFalse;
	}
}

void CVStringSetAdd(CVStringSetRef set, const CVString element){
	CVStringSetEntry* entry = calloc(1,sizeof(CVStringSetEntry));
	entry->element = CVNewStringFromString(element);

	CVStringSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND_STR((*set), entry->element, entryTemp);
	}
	if(!entryTemp){
		HASH_ADD_KEYPTR( hh, (*set), entry->element, strlen(entry->element), entry);
	}else{
		free(entry);
	}
}

void CVStringSetRemove(CVStringSetRef set, const CVString element){
	CVStringSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND_STR((*set), element, entryTemp);
	}
	if(entryTemp){
		HASH_DEL((*set), entryTemp);
		free(entryTemp->element);
		free(entryTemp);
	}
}

void CVStringSetClear(CVStringSetRef set){
	CVStringSetEntry* entry = NULL;
	CVStringSetEntry* entryTemp = NULL;
	HASH_ITER(hh, (*set), entry, entryTemp) {
		HASH_DEL((*set), entry);
		free(entry->element);
		free(entry);
	}
}

void CVStringSetDestroy(CVStringSetRef set){
	CVStringSetClear(set);
	free(set);
}

CVStringSetRef CVNewStringSetFromUnion(const CVStringSetRef firtSet, const CVStringSetRef secondSet){
	CVStringSetRef newSet = CVNewStringSet();
	CVStringSetFOR(setEntry, firtSet){
		CVStringSetAdd(newSet, setEntry->element);
	}
	CVStringSetFOR(setEntry, secondSet){
		CVStringSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

void CVStringSetUnion(CVStringSetRef destinationSet, const CVStringSetRef unionSet){
	CVStringSetFOR(setEntry, unionSet){
		CVStringSetAdd(destinationSet, setEntry->element);
	}
}

CVStringSetRef CVNewStringSetFromIntersection(const CVStringSetRef firtSet, const CVStringSetRef secondSet){
	CVStringSetRef newSet = CVNewStringSet();
	CVStringSetFOR(setEntry, firtSet){
		if(CVStringSetHas(secondSet, setEntry->element)){
			CVStringSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVStringSetRef CVNewStringSetFromDifference(const CVStringSetRef firtSet, const CVStringSetRef secondSet){
	CVStringSetRef newSet = CVNewStringSet();
	CVStringSetFOR(setEntry, firtSet){
		if(!CVStringSetHas(secondSet, setEntry->element)){
			CVStringSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVStringSetRef CVNewStringSetFromSymmetricDifference(const CVStringSetRef firtSet, const CVStringSetRef secondSet){
	CVStringSetRef newSet = CVNewStringSet();
	CVStringSetFOR(setEntry, firtSet){
		if(!CVStringSetHas(secondSet, setEntry->element)){
			CVStringSetAdd(newSet, setEntry->element);
		}
	}
	CVStringSetFOR(setEntry, secondSet){
		if(!CVStringSetHas(firtSet, setEntry->element)){
			CVStringSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVStringSetRef CVNewStringSetFromSet(const CVStringSetRef aSet){
	CVStringSetRef newSet = CVNewStringSet();
	CVStringSetFOR(setEntry, aSet){
		CVStringSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

CVBool CVStringSetIsSubsetOf(const CVStringSetRef subSet, const CVStringSetRef superSet){
	CVBool isSubset = CVTrue;
	CVStringSetFOR(setEntry, subSet){
		if(!CVStringSetHas(superSet, setEntry->element)){
			isSubset = CVFalse;
			break;
		}
	}
	return isSubset;
}

CVBool CVStringSetIsSupersetOf(const CVStringSetRef superSet, const CVStringSetRef subSet){
	CVBool isSuperset = CVTrue;
	CVStringSetFOR(setEntry, subSet){
		if(!CVStringSetHas(superSet, setEntry->element)){
			isSuperset = CVFalse;
			break;
		}
	}
	return isSuperset;
}


// CVGeneric



CVGenericSetRef CVNewGenericSet(){
	return calloc(1, sizeof(CVGenericSet));
}

CVBool CVGenericSetHas(const CVGenericSetRef set, const void* element, CVSize elementSize){
	CVGenericSetEntry* entry = NULL;
	if(set){
		HASH_FIND(hh, (*set), element, elementSize, entry);
	}
	if(entry){
		return CVTrue;
	}else{
		return CVFalse;
	}
}

void CVGenericSetAdd(CVGenericSetRef set, const void* element, CVSize elementSize){
	CVGenericSetEntry* entry = calloc(1,sizeof(CVGenericSetEntry));
	entry->element=calloc(1, elementSize);
	memcpy(entry->element, element, elementSize);
	entry->elementSize = elementSize;
	CVGenericSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), entry->element, entry->elementSize, entryTemp);
	}
	if(!entryTemp){
		HASH_ADD_KEYPTR( hh, (*set), entry->element, entry->elementSize, entry);
	}else{
		free(entry);
	}
}

void CVGenericSetRemove(CVGenericSetRef set, const void* element, CVSize elementSize){
	CVGenericSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), element, elementSize, entryTemp);
	}
	if(entryTemp){
		HASH_DEL((*set), entryTemp);
		free(entryTemp->element);
		free(entryTemp);
	}
}

void CVGenericSetClear(CVGenericSetRef set){
	CVGenericSetEntry* entry = NULL;
	CVGenericSetEntry* entryTemp = NULL;
	HASH_ITER(hh, (*set), entry, entryTemp) {
		HASH_DEL((*set), entry);
		free(entry->element);
		free(entry);
	}
}

void CVGenericSetDestroy(CVGenericSetRef set){
	CVGenericSetClear(set);
	free(set);
}

CVGenericSetRef CVNewGenericSetFromUnion(const CVGenericSetRef firtSet, const CVGenericSetRef secondSet){
	CVGenericSetRef newSet = CVNewGenericSet();
	CVGenericSetFOR(setEntry, firtSet){
		CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
	}
	CVGenericSetFOR(setEntry, secondSet){
		CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
	}
	return newSet;
}

void CVGenericSetUnion(CVGenericSetRef destinationSet, const CVGenericSetRef unionSet){
	CVGenericSetFOR(setEntry, unionSet){
		CVGenericSetAdd(destinationSet, setEntry->element, setEntry->elementSize);
	}
}

CVGenericSetRef CVNewGenericSetFromIntersection(const CVGenericSetRef firtSet, const CVGenericSetRef secondSet){
	CVGenericSetRef newSet = CVNewGenericSet();
	CVGenericSetFOR(setEntry, firtSet){
		if(CVGenericSetHas(secondSet, setEntry->element, setEntry->elementSize)){
			CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
		}
	}
	return newSet;
}

CVGenericSetRef CVNewGenericSetFromDifference(const CVGenericSetRef firtSet, const CVGenericSetRef secondSet){
	CVGenericSetRef newSet = CVNewGenericSet();
	CVGenericSetFOR(setEntry, firtSet){
		if(!CVGenericSetHas(secondSet, setEntry->element, setEntry->elementSize)){
			CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
		}
	}
	return newSet;
}

CVGenericSetRef CVNewGenericSetFromSymmetricDifference(const CVGenericSetRef firtSet, const CVGenericSetRef secondSet){
	CVGenericSetRef newSet = CVNewGenericSet();
	CVGenericSetFOR(setEntry, firtSet){
		if(!CVGenericSetHas(secondSet, setEntry->element, setEntry->elementSize)){
			CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
		}
	}
	CVGenericSetFOR(setEntry, secondSet){
		if(!CVGenericSetHas(firtSet, setEntry->element, setEntry->elementSize)){
			CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
		}
	}
	return newSet;
}

CVGenericSetRef CVNewGenericSetFromSet(const CVGenericSetRef aSet){
	CVGenericSetRef newSet = CVNewGenericSet();
	CVGenericSetFOR(setEntry, aSet){
		CVGenericSetAdd(newSet, setEntry->element, setEntry->elementSize);
	}
	return newSet;
}

CVBool CVGenericSetIsSubsetOf(const CVGenericSetRef subSet, const CVGenericSetRef superSet){
	CVBool isSubset = CVTrue;
	CVGenericSetFOR(setEntry, subSet){
		if(!CVGenericSetHas(superSet, setEntry->element, setEntry->elementSize)){
			isSubset = CVFalse;
			break;
		}
	}
	return isSubset;
}

CVBool CVGenericSetIsSupersetOf(const CVGenericSetRef superSet, const CVGenericSetRef subSet){
	CVBool isSuperset = CVTrue;
	CVGenericSetFOR(setEntry, subSet){
		if(!CVGenericSetHas(superSet, setEntry->element, setEntry->elementSize)){
			isSuperset = CVFalse;
			break;
		}
	}
	return isSuperset;
}





//CVUInteger



CVUIntegerSetRef CVNewUIntegerSet(){
	return calloc(1, sizeof(CVUIntegerSet));
}

CVBool CVUIntegerSetHas(const CVUIntegerSetRef set, CVUInteger element){
	CVUIntegerSetEntry* entry = NULL;
	if(set){
		HASH_FIND(hh, (*set), &element, sizeof(CVUInteger), entry);
	}
	if(entry){
		return CVTrue;
	}else{
		return CVFalse;
	}
}

void CVUIntegerSetAdd(CVUIntegerSetRef set, CVUInteger element){
	CVUIntegerSetEntry* entry = calloc(1,sizeof(CVUIntegerSetEntry));
	entry->element = element;
	CVUIntegerSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), &(entry->element), sizeof(CVUInteger), entryTemp);
	}
	if(!entryTemp){
		HASH_ADD_KEYPTR( hh, (*set), &(entry->element), sizeof(CVUInteger), entry);
	}else{
		free(entry);
	}
}

void CVUIntegerSetRemove(CVUIntegerSetRef set, CVUInteger element){
	CVUIntegerSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), &element, sizeof(CVUInteger), entryTemp);
	}
	if(entryTemp){
		HASH_DEL((*set), entryTemp);
		free(entryTemp);
	}
}

void CVUIntegerSetClear(CVUIntegerSetRef set){
	CVUIntegerSetEntry* entry = NULL;
	CVUIntegerSetEntry* entryTemp = NULL;
	HASH_ITER(hh, (*set), entry, entryTemp) {
		HASH_DEL((*set), entry);
		free(entry);
	}
}

void CVUIntegerSetDestroy(CVUIntegerSetRef set){
	CVUIntegerSetClear(set);
	free(set);
}

CVUIntegerSetRef CVNewUIntegerSetFromUnion(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet){
	CVUIntegerSetRef newSet = CVNewUIntegerSet();
	CVUIntegerSetFOR(setEntry, firtSet){
		CVUIntegerSetAdd(newSet, setEntry->element);
	}
	CVUIntegerSetFOR(setEntry, secondSet){
		CVUIntegerSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

void CVUIntegerSetUnion(CVUIntegerSetRef destinationSet, const CVUIntegerSetRef unionSet){
	CVUIntegerSetFOR(setEntry, unionSet){
		CVUIntegerSetAdd(destinationSet, setEntry->element);
	}
}

CVUIntegerSetRef CVNewUIntegerSetFromIntersection(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet){
	CVUIntegerSetRef newSet = CVNewUIntegerSet();
	CVUIntegerSetFOR(setEntry, firtSet){
		if(CVUIntegerSetHas(secondSet, setEntry->element)){
			CVUIntegerSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVUIntegerSetRef CVNewUIntegerSetFromDifference(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet){
	CVUIntegerSetRef newSet = CVNewUIntegerSet();
	CVUIntegerSetFOR(setEntry, firtSet){
		if(!CVUIntegerSetHas(secondSet, setEntry->element)){
			CVUIntegerSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVUIntegerSetRef CVNewUIntegerSetFromSymmetricDifference(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet){
	CVUIntegerSetRef newSet = CVNewUIntegerSet();
	CVUIntegerSetFOR(setEntry, firtSet){
		if(!CVUIntegerSetHas(secondSet, setEntry->element)){
			CVUIntegerSetAdd(newSet, setEntry->element);
		}
	}
	CVUIntegerSetFOR(setEntry, secondSet){
		if(!CVUIntegerSetHas(firtSet, setEntry->element)){
			CVUIntegerSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVUIntegerSetRef CVNewUIntegerSetFromSet(const CVUIntegerSetRef aSet){
	CVUIntegerSetRef newSet = CVNewUIntegerSet();
	CVUIntegerSetFOR(setEntry, aSet){
		CVUIntegerSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

CVBool CVUIntegerSetIsSubsetOf(const CVUIntegerSetRef subSet, const CVUIntegerSetRef superSet){
	CVBool isSubset = CVTrue;
	CVUIntegerSetFOR(setEntry, subSet){
		if(!CVUIntegerSetHas(superSet, setEntry->element)){
			isSubset = CVFalse;
			break;
		}
	}
	return isSubset;
}

CVBool CVUIntegerSetIsSupersetOf(const CVUIntegerSetRef superSet, const CVUIntegerSetRef subSet){
	CVBool isSuperset = CVTrue;
	CVUIntegerSetFOR(setEntry, subSet){
		if(!CVUIntegerSetHas(superSet, setEntry->element)){
			isSuperset = CVFalse;
			break;
		}
	}
	return isSuperset;
}


//CVInteger



CVIntegerSetRef CVNewIntegerSet(){
	return calloc(1, sizeof(CVIntegerSet));
}

CVBool CVIntegerSetHas(const CVIntegerSetRef set, CVInteger element){
	CVIntegerSetEntry* entry = NULL;
	if(set){
		HASH_FIND(hh, (*set), &element, sizeof(CVInteger), entry);
	}
	if(entry){
		return CVTrue;
	}else{
		return CVFalse;
	}
}

void CVIntegerSetAdd(CVIntegerSetRef set, CVInteger element){
	CVIntegerSetEntry* entry = calloc(1,sizeof(CVIntegerSetEntry));
	entry->element = element;
	CVIntegerSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), &(entry->element), sizeof(CVInteger), entryTemp);
	}
	if(!entryTemp){
		HASH_ADD_KEYPTR( hh, (*set), &(entry->element), sizeof(CVInteger), entry);
	}else{
		free(entry);
	}
}

void CVIntegerSetRemove(CVIntegerSetRef set, CVInteger element){
	CVIntegerSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), &element, sizeof(CVInteger), entryTemp);
	}
	if(entryTemp){
		HASH_DEL((*set), entryTemp);
		free(entryTemp);
	}
}

void CVIntegerSetClear(CVIntegerSetRef set){
	CVIntegerSetEntry* entry = NULL;
	CVIntegerSetEntry* entryTemp = NULL;
	HASH_ITER(hh, (*set), entry, entryTemp) {
		HASH_DEL((*set), entry);
		free(entry);
	}
}

void CVIntegerSetDestroy(CVIntegerSetRef set){
	CVIntegerSetClear(set);
	free(set);
}

CVIntegerSetRef CVNewIntegerSetFromUnion(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet){
	CVIntegerSetRef newSet = CVNewIntegerSet();
	CVIntegerSetFOR(setEntry, firtSet){
		CVIntegerSetAdd(newSet, setEntry->element);
	}
	CVIntegerSetFOR(setEntry, secondSet){
		CVIntegerSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

void CVIntegerSetUnion(CVIntegerSetRef destinationSet, const CVIntegerSetRef unionSet){
	CVIntegerSetFOR(setEntry, unionSet){
		CVIntegerSetAdd(destinationSet, setEntry->element);
	}
}

CVIntegerSetRef CVNewIntegerSetFromIntersection(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet){
	CVIntegerSetRef newSet = CVNewIntegerSet();
	CVIntegerSetFOR(setEntry, firtSet){
		if(CVIntegerSetHas(secondSet, setEntry->element)){
			CVIntegerSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVIntegerSetRef CVNewIntegerSetFromDifference(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet){
	CVIntegerSetRef newSet = CVNewIntegerSet();
	CVIntegerSetFOR(setEntry, firtSet){
		if(!CVIntegerSetHas(secondSet, setEntry->element)){
			CVIntegerSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVIntegerSetRef CVNewIntegerSetFromSymmetricDifference(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet){
	CVIntegerSetRef newSet = CVNewIntegerSet();
	CVIntegerSetFOR(setEntry, firtSet){
		if(!CVIntegerSetHas(secondSet, setEntry->element)){
			CVIntegerSetAdd(newSet, setEntry->element);
		}
	}
	CVIntegerSetFOR(setEntry, secondSet){
		if(!CVIntegerSetHas(firtSet, setEntry->element)){
			CVIntegerSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVIntegerSetRef CVNewIntegerSetFromSet(const CVIntegerSetRef aSet){
	CVIntegerSetRef newSet = CVNewIntegerSet();
	CVIntegerSetFOR(setEntry, aSet){
		CVIntegerSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

CVBool CVIntegerSetIsSubsetOf(const CVIntegerSetRef subSet, const CVIntegerSetRef superSet){
	CVBool isSubset = CVTrue;
	CVIntegerSetFOR(setEntry, subSet){
		if(!CVIntegerSetHas(superSet, setEntry->element)){
			isSubset = CVFalse;
			break;
		}
	}
	return isSubset;
}

CVBool CVIntegerSetIsSupersetOf(const CVIntegerSetRef superSet, const CVIntegerSetRef subSet){
	CVBool isSuperset = CVTrue;
	CVIntegerSetFOR(setEntry, subSet){
		if(!CVIntegerSetHas(superSet, setEntry->element)){
			isSuperset = CVFalse;
			break;
		}
	}
	return isSuperset;
}


//CVEdge



CVEdgeSetRef CVNewEdgeSet(){
	return calloc(1, sizeof(CVEdgeSet));
}

CVBool CVEdgeSetHas(const CVEdgeSetRef set, CVEdge element){
	CVEdgeSetEntry* entry = NULL;
	if(set){
		HASH_FIND(hh, (*set), &element, sizeof(CVEdge), entry);
	}
	if(entry){
		return CVTrue;
	}else{
		return CVFalse;
	}
}

void CVEdgeSetAdd(CVEdgeSetRef set, CVEdge element){
	CVEdgeSetEntry* entry = calloc(1,sizeof(CVEdgeSetEntry));
	entry->element = element;
	CVEdgeSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), &(entry->element), sizeof(CVEdge), entryTemp);
	}
	if(!entryTemp){
		HASH_ADD_KEYPTR( hh, (*set), &(entry->element), sizeof(CVEdge), entry);
	}else{
		free(entry);
	}
}

void CVEdgeSetRemove(CVEdgeSetRef set, CVEdge element){
	CVEdgeSetEntry* entryTemp = NULL;
	if(set){
		HASH_FIND(hh, (*set), &element, sizeof(CVEdge), entryTemp);
	}
	if(entryTemp){
		HASH_DEL((*set), entryTemp);
		free(entryTemp);
	}
}

void CVEdgeSetClear(CVEdgeSetRef set){
	CVEdgeSetEntry* entry = NULL;
	CVEdgeSetEntry* entryTemp = NULL;
	HASH_ITER(hh, (*set), entry, entryTemp) {
		HASH_DEL((*set), entry);
		free(entry);
	}
}

void CVEdgeSetDestroy(CVEdgeSetRef set){
	CVEdgeSetClear(set);
	free(set);
}

CVEdgeSetRef CVNewEdgeSetFromUnion(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet){
	CVEdgeSetRef newSet = CVNewEdgeSet();
	CVEdgeSetFOR(setEntry, firtSet){
		CVEdgeSetAdd(newSet, setEntry->element);
	}
	CVEdgeSetFOR(setEntry, secondSet){
		CVEdgeSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

void CVEdgeSetUnion(CVEdgeSetRef destinationSet, const CVEdgeSetRef unionSet){
	CVEdgeSetFOR(setEntry, unionSet){
		CVEdgeSetAdd(destinationSet, setEntry->element);
	}
}

CVEdgeSetRef CVNewEdgeSetFromIntersection(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet){
	CVEdgeSetRef newSet = CVNewEdgeSet();
	CVEdgeSetFOR(setEntry, firtSet){
		if(CVEdgeSetHas(secondSet, setEntry->element)){
			CVEdgeSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVEdgeSetRef CVNewEdgeSetFromDifference(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet){
	CVEdgeSetRef newSet = CVNewEdgeSet();
	CVEdgeSetFOR(setEntry, firtSet){
		if(!CVEdgeSetHas(secondSet, setEntry->element)){
			CVEdgeSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVEdgeSetRef CVNewEdgeSetFromSymmetricDifference(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet){
	CVEdgeSetRef newSet = CVNewEdgeSet();
	CVEdgeSetFOR(setEntry, firtSet){
		if(!CVEdgeSetHas(secondSet, setEntry->element)){
			CVEdgeSetAdd(newSet, setEntry->element);
		}
	}
	CVEdgeSetFOR(setEntry, secondSet){
		if(!CVEdgeSetHas(firtSet, setEntry->element)){
			CVEdgeSetAdd(newSet, setEntry->element);
		}
	}
	return newSet;
}

CVEdgeSetRef CVNewEdgeSetFromSet(const CVEdgeSetRef aSet){
	CVEdgeSetRef newSet = CVNewEdgeSet();
	CVEdgeSetFOR(setEntry, aSet){
		CVEdgeSetAdd(newSet, setEntry->element);
	}
	return newSet;
}

CVBool CVEdgeSetIsSubsetOf(const CVEdgeSetRef subSet, const CVEdgeSetRef superSet){
	CVBool isSubset = CVTrue;
	CVEdgeSetFOR(setEntry, subSet){
		if(!CVEdgeSetHas(superSet, setEntry->element)){
			isSubset = CVFalse;
			break;
		}
	}
	return isSubset;
}

CVBool CVEdgeSetIsSupersetOf(const CVEdgeSetRef superSet, const CVEdgeSetRef subSet){
	CVBool isSuperset = CVTrue;
	CVEdgeSetFOR(setEntry, subSet){
		if(!CVEdgeSetHas(superSet, setEntry->element)){
			isSuperset = CVFalse;
			break;
		}
	}
	return isSuperset;
}


