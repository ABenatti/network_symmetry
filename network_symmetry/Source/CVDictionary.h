//
//  CVDictionary.h
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 11/5/16.
//  Copyright © 2016 Filipi Nascimento Silva. All rights reserved.
//

#ifndef CVDictionary_h
#define CVDictionary_h

#include "uthash.h"
#include "CVNetwork.h"


typedef struct {
	char* key;
	void* data;
	UT_hash_handle hh;
} CVStringDictionaryEntry;

typedef CVStringDictionaryEntry* CVStringDictionary;
typedef CVStringDictionary* CVStringDictionaryRef;

#define CVStringDictionaryFOR(dictionaryEntry,dictionary) for(CVStringDictionaryEntry* dictionaryEntry=*dictionary; dictionaryEntry != NULL; dictionaryEntry=dictionaryEntry->hh.next)

CVStringDictionaryRef CVNewStringDictionary();
void* CVStringDictionaryEntryForKey(const CVStringDictionaryRef dictionary, const CVString key);
void* CVStringDictionarySetEntry(CVStringDictionaryRef dictionary, const CVString key, void* data);
void* CVStringDictionaryDeleteEntry(CVStringDictionaryRef dictionary, const CVString key);
void* CVStringDictionaryDeleteAndFreeEntry(CVStringDictionaryRef dictionary, const CVString key);
void CVStringDictionaryClear(CVStringDictionaryRef dictionary);
void CVStringDictionaryClearAndFree(CVStringDictionaryRef dictionary);
void CVStringDictionaryDestroy(CVStringDictionaryRef dictionary);

CV_INLINE CVSize CVStringDictionaryCount(CVStringDictionaryRef dictionary){
	return (CVSize)HASH_COUNT((*dictionary));
}



typedef struct {
	CVUInteger key;
	void* data;
	UT_hash_handle hh;
} CVUIntegerDictionaryEntry;

typedef CVUIntegerDictionaryEntry* CVUIntegerDictionary;
typedef CVUIntegerDictionary* CVUIntegerDictionaryRef;

#define CVUIntegerDictionaryFOR(dictionaryEntry,dictionary) for(CVUIntegerDictionaryEntry* dictionaryEntry=*dictionary; dictionaryEntry != NULL; dictionaryEntry=dictionaryEntry->hh.next)

CVUIntegerDictionaryRef CVNewUIntegerDictionary();
void* CVUIntegerDictionaryEntryForKey(const CVUIntegerDictionaryRef dictionary, const CVUInteger key);
void* CVUIntegerDictionarySetEntry(CVUIntegerDictionaryRef dictionary, const CVUInteger key, void* data);
void* CVUIntegerDictionaryDeleteEntry(CVUIntegerDictionaryRef dictionary, const CVUInteger key);
void* CVUIntegerDictionaryDeleteAndFreeEntry(CVUIntegerDictionaryRef dictionary, const CVUInteger key);
void CVUIntegerDictionaryClear(CVUIntegerDictionaryRef dictionary);
void CVUIntegerDictionaryClearAndFree(CVUIntegerDictionaryRef dictionary);
void CVUIntegerDictionaryDestroy(CVUIntegerDictionaryRef dictionary);

CV_INLINE CVSize CVUIntegerDictionaryCount(CVUIntegerDictionaryRef dictionary){
	return (CVSize)HASH_COUNT((*dictionary));
}



typedef struct {
	CVInteger key;
	void* data;
	UT_hash_handle hh;
} CVIntegerDictionaryEntry;

typedef CVIntegerDictionaryEntry* CVIntegerDictionary;
typedef CVIntegerDictionary* CVIntegerDictionaryRef;

#define CVIntegerDictionaryFOR(dictionaryEntry,dictionary) for(CVIntegerDictionaryEntry* dictionaryEntry=*dictionary; dictionaryEntry != NULL; dictionaryEntry=dictionaryEntry->hh.next)

CVIntegerDictionaryRef CVNewIntegerDictionary();
void* CVIntegerDictionaryEntryForKey(const CVIntegerDictionaryRef dictionary, const CVInteger key);
void* CVIntegerDictionarySetEntry(CVIntegerDictionaryRef dictionary, const CVInteger key, void* data);
void* CVIntegerDictionaryDeleteEntry(CVIntegerDictionaryRef dictionary, const CVInteger key);
void* CVIntegerDictionaryDeleteAndFreeEntry(CVIntegerDictionaryRef dictionary, const CVInteger key);
void CVIntegerDictionaryClear(CVIntegerDictionaryRef dictionary);
void CVIntegerDictionaryClearAndFree(CVIntegerDictionaryRef dictionary);
void CVIntegerDictionaryDestroy(CVIntegerDictionaryRef dictionary);

CV_INLINE CVSize CVIntegerDictionaryCount(CVIntegerDictionaryRef dictionary){
	return (CVSize)HASH_COUNT((*dictionary));
}



typedef struct {
	CVEdge key;
	void* data;
	UT_hash_handle hh;
} CVEdgeDictionaryEntry;

typedef CVEdgeDictionaryEntry* CVEdgeDictionary;
typedef CVEdgeDictionary* CVEdgeDictionaryRef;

#define CVEdgeDictionaryFOR(dictionaryEntry,dictionary) for(CVEdgeDictionaryEntry* dictionaryEntry=*dictionary; dictionaryEntry != NULL; dictionaryEntry=dictionaryEntry->hh.next)


CVEdgeDictionaryRef CVNewEdgeDictionary();
void* CVEdgeDictionaryEntryForKey(const CVEdgeDictionaryRef dictionary, const CVEdge key);
void* CVEdgeDictionarySetEntry(CVEdgeDictionaryRef dictionary, const CVEdge key, void* data);
void* CVEdgeDictionaryDeleteEntry(CVEdgeDictionaryRef dictionary, const CVEdge key);
void* CVEdgeDictionaryDeleteAndFreeEntry(CVEdgeDictionaryRef dictionary, const CVEdge key);
void CVEdgeDictionaryClear(CVEdgeDictionaryRef dictionary);
void CVEdgeDictionaryClearAndFree(CVEdgeDictionaryRef dictionary);
void CVEdgeDictionaryDestroy(CVEdgeDictionaryRef dictionary);

CV_INLINE CVSize CVEdgeDictionaryCount(CVEdgeDictionaryRef dictionary){
	return (CVSize)HASH_COUNT((*dictionary));
}



typedef struct {
	void* key;
	void* data;
	UT_hash_handle hh;
} CVGenericDictionaryEntry;

typedef CVGenericDictionaryEntry* CVGenericDictionary;
typedef CVGenericDictionary* CVGenericDictionaryRef;

#define CVGenericDictionaryFOR(dictionaryEntry,dictionary) for(CVGenericDictionaryEntry* dictionaryEntry=*dictionary; dictionaryEntry != NULL; dictionaryEntry=dictionaryEntry->hh.next)

CVGenericDictionaryRef CVNewGenericDictionary();
void* CVGenericDictionarySetEntry(CVGenericDictionaryRef dictionary, const void* key, CVSize keysize, void* data);
void* CVGenericDictionaryDeleteEntry(CVGenericDictionaryRef dictionary, const void* key, CVSize keysize);
void* CVGenericDictionaryDeleteAndFreeEntry(CVGenericDictionaryRef dictionary, const void* key, CVSize keysize);
void CVGenericDictionaryClear(CVGenericDictionaryRef dictionary);
void CVGenericDictionaryClearAndFree(CVGenericDictionaryRef dictionary);
void CVGenericDictionaryDestroy(CVGenericDictionaryRef dictionary);

CV_INLINE CVSize CVGenericDictionaryCount(CVGenericDictionaryRef dictionary){
	return (CVSize)HASH_COUNT((*dictionary));
}

#endif /* CVDictionary_h */
