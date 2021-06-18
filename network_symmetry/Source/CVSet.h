//
//  CVSet.h
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 11/5/16.
//  Copyright © 2016 Filipi Nascimento Silva. All rights reserved.
//

#ifndef CVSet_h
#define CVSet_h


#include "uthash.h"
#include "CVNetwork.h"


typedef struct {
	char* element;
	UT_hash_handle hh;
} CVStringSetEntry;

typedef CVStringSetEntry* CVStringSet;
typedef CVStringSet* CVStringSetRef;

#define CVStringSetFOR(setEntry,set) for(CVStringSetEntry* setEntry=*set; setEntry != NULL; setEntry=setEntry->hh.next)

CVStringSetRef CVNewStringSet();
CVBool CVStringSetHas(const CVStringSetRef set, const CVString key);
void CVStringSetAdd(CVStringSetRef set, const CVString element);
void CVStringSetRemove(CVStringSetRef set, const CVString element);
void CVStringSetClear(CVStringSetRef set);
void CVStringSetDestroy(CVStringSetRef set);
CVStringSetRef CVNewStringSetFromUnion(const CVStringSetRef firtSet, const CVStringSetRef secondSet);
void CVStringSetUnion(CVStringSetRef destinationSet, const CVStringSetRef unionSet);
CVStringSetRef CVNewStringSetFromIntersection(const CVStringSetRef firtSet, const CVStringSetRef secondSet);
CVStringSetRef CVNewStringSetFromDifference(const CVStringSetRef firtSet, const CVStringSetRef secondSet);
CVStringSetRef CVNewStringSetFromSymmetricDifference(const CVStringSetRef firtSet, const CVStringSetRef secondSet);
CVStringSetRef CVNewStringSetFromSet(const CVStringSetRef aSet);
CVBool CVStringSetIsSubsetOf(const CVStringSetRef subSet, const CVStringSetRef superSet);
CVBool CVStringSetIsSupersetOf(const CVStringSetRef superSet, const CVStringSetRef subSet);

CV_INLINE CVSize CVStringSetCount(CVStringSetRef aSet){
	return (CVSize)HASH_COUNT((*aSet));
}





typedef struct {
	CVInteger element;
	UT_hash_handle hh;
} CVIntegerSetEntry;

typedef CVIntegerSetEntry* CVIntegerSet;
typedef CVIntegerSet* CVIntegerSetRef;

#define CVIntegerSetFOR(setEntry,set) for(CVIntegerSetEntry* setEntry=*set; setEntry != NULL; setEntry=setEntry->hh.next)


CVIntegerSetRef CVNewIntegerSet();
CVBool CVIntegerSetHas(const CVIntegerSetRef set, const CVInteger key);
void CVIntegerSetAdd(CVIntegerSetRef set, const CVInteger element);
void CVIntegerSetRemove(CVIntegerSetRef set, const CVInteger element);
void CVIntegerSetClear(CVIntegerSetRef set);
void CVIntegerSetDestroy(CVIntegerSetRef set);
CVIntegerSetRef CVNewIntegerSetFromUnion(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet);
void CVIntegerSetUnion(CVIntegerSetRef destinationSet, const CVIntegerSetRef unionSet);
CVIntegerSetRef CVNewIntegerSetFromIntersection(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet);
CVIntegerSetRef CVNewIntegerSetFromDifference(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet);
CVIntegerSetRef CVNewIntegerSetFromSymmetricDifference(const CVIntegerSetRef firtSet, const CVIntegerSetRef secondSet);
CVIntegerSetRef CVNewIntegerSetFromSet(const CVIntegerSetRef aSet);
CVBool CVIntegerSetIsSubsetOf(const CVIntegerSetRef subSet, const CVIntegerSetRef superSet);
CVBool CVIntegerSetIsSupersetOf(const CVIntegerSetRef superSet, const CVIntegerSetRef subSet);

CV_INLINE CVSize CVIntegerSetCount(CVIntegerSetRef aSet){
	return (CVSize)HASH_COUNT((*aSet));
}



typedef struct {
	CVUInteger element;
	UT_hash_handle hh;
} CVUIntegerSetEntry;

typedef CVUIntegerSetEntry* CVUIntegerSet;
typedef CVUIntegerSet* CVUIntegerSetRef;

#define CVUIntegerSetFOR(setEntry,set) for(CVUIntegerSetEntry* setEntry=*set; setEntry != NULL; setEntry=setEntry->hh.next)


CVUIntegerSetRef CVNewUIntegerSet();
CVBool CVUIntegerSetHas(const CVUIntegerSetRef set, const CVUInteger key);
void CVUIntegerSetAdd(CVUIntegerSetRef set, const CVUInteger element);
void CVUIntegerSetRemove(CVUIntegerSetRef set, const CVUInteger element);
void CVUIntegerSetClear(CVUIntegerSetRef set);
void CVUIntegerSetDestroy(CVUIntegerSetRef set);
CVUIntegerSetRef CVNewUIntegerSetFromUnion(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet);
void CVUIntegerSetUnion(CVUIntegerSetRef destinationSet, const CVUIntegerSetRef unionSet);
CVUIntegerSetRef CVNewUIntegerSetFromIntersection(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet);
CVUIntegerSetRef CVNewUIntegerSetFromDifference(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet);
CVUIntegerSetRef CVNewUIntegerSetFromSymmetricDifference(const CVUIntegerSetRef firtSet, const CVUIntegerSetRef secondSet);
CVUIntegerSetRef CVNewUIntegerSetFromSet(const CVUIntegerSetRef aSet);
CVBool CVUIntegerSetIsSubsetOf(const CVUIntegerSetRef subSet, const CVUIntegerSetRef superSet);
CVBool CVUIntegerSetIsSupersetOf(const CVUIntegerSetRef superSet, const CVUIntegerSetRef subSet);

CV_INLINE CVSize CVUIntegerSetCount(CVUIntegerSetRef aSet){
	return (CVSize)HASH_COUNT((*aSet));
}





typedef struct {
	CVEdge element;
	UT_hash_handle hh;
} CVEdgeSetEntry;

typedef CVEdgeSetEntry* CVEdgeSet;
typedef CVEdgeSet* CVEdgeSetRef;

#define CVEdgeSetFOR(setEntry,set) for(CVEdgeSetEntry* setEntry=*set; setEntry != NULL; setEntry=setEntry->hh.next)

CVEdgeSetRef CVNewEdgeSet();
CVBool CVEdgeSetHas(const CVEdgeSetRef set, const CVEdge key);
void CVEdgeSetAdd(CVEdgeSetRef set, const CVEdge element);
void CVEdgeSetRemove(CVEdgeSetRef set, const CVEdge element);
void CVEdgeSetClear(CVEdgeSetRef set);
void CVEdgeSetDestroy(CVEdgeSetRef set);
CVEdgeSetRef CVNewEdgeSetFromUnion(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet);
void CVEdgeSetUnion(CVEdgeSetRef destinationSet, const CVEdgeSetRef unionSet);
CVEdgeSetRef CVNewEdgeSetFromIntersection(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet);
CVEdgeSetRef CVNewEdgeSetFromDifference(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet);
CVEdgeSetRef CVNewEdgeSetFromSymmetricDifference(const CVEdgeSetRef firtSet, const CVEdgeSetRef secondSet);
CVEdgeSetRef CVNewEdgeSetFromSet(const CVEdgeSetRef aSet);
CVBool CVEdgeSetIsSubsetOf(const CVEdgeSetRef subSet, const CVEdgeSetRef superSet);
CVBool CVEdgeSetIsSupersetOf(const CVEdgeSetRef superSet, const CVEdgeSetRef subSet);

CV_INLINE CVSize CVEdgeSetCount(CVEdgeSetRef aSet){
	return (CVSize)HASH_COUNT((*aSet));
}



typedef struct {
	void* element;
	CVSize elementSize;
	UT_hash_handle hh;
} CVGenericSetEntry;

typedef CVGenericSetEntry* CVGenericSet;
typedef CVGenericSet* CVGenericSetRef;

#define CVGenericSetFOR(setEntry,set) for(CVGenericSetEntry* setEntry=*set; setEntry != NULL; setEntry=setEntry->hh.next)



#endif /* CVSet_h */
