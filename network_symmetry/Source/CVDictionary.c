//
//  CVDictionary.c
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 11/5/16.
//  Copyright © 2016 Filipi Nascimento Silva. All rights reserved.
//

#include "CVDictionary.h"


CVStringDictionaryRef CVNewStringDictionary(){
	return calloc(1, sizeof(CVStringDictionary));
}

void* CVStringDictionaryEntryForKey(const CVStringDictionaryRef dictionary, const CVString key){
	CVStringDictionaryEntry* entry = NULL;
	if(dictionary){
		HASH_FIND_STR((*dictionary), key, entry);
	}
	if(entry){
		return entry->data;
	}else{
		return NULL;
	}
}

void* CVStringDictionarySetEntry(CVStringDictionaryRef dictionary, const CVString key, void* data){
	CVStringDictionaryEntry* entry = calloc(1,sizeof(CVStringDictionaryEntry));
	entry->key = CVNewStringFromString(key);
	entry->data = data;
	CVStringDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND_STR((*dictionary), entry->key, entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->key);
		free(entryTemp);
	}
	HASH_ADD_KEYPTR( hh, (*dictionary), entry->key, strlen(entry->key), entry);
	return replacedData;
}

void* CVStringDictionaryDeleteEntry(CVStringDictionaryRef dictionary, const CVString key){
	CVStringDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND_STR((*dictionary), key, entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->key);
		free(entryTemp);
	}
	return replacedData;
}
void* CVStringDictionaryDeleteAndFreeEntry(CVStringDictionaryRef dictionary, const CVString key){
	CVStringDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND_STR((*dictionary), key, entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->key);
		free(entryTemp->data);
		free(entryTemp);
	}
	return replacedData;
}

void CVStringDictionaryClear(CVStringDictionaryRef dictionary){
	CVStringDictionaryEntry* entry = NULL;
	CVStringDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->key);
		free(entry);
	}
}

void CVStringDictionaryClearAndFree(CVStringDictionaryRef dictionary){
	CVStringDictionaryEntry* entry = NULL;
	CVStringDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->data);
		free(entry->key);
		free(entry);
	}
}

void CVStringDictionaryDestroy(CVStringDictionaryRef dictionary){
	CVStringDictionaryClear(dictionary);
	free(dictionary);
}




/////// CVUInteger

CVUIntegerDictionaryRef CVNewUIntegerDictionary(){
	return calloc(1, sizeof(CVUIntegerDictionary));
}

void* CVUIntegerDictionaryEntryForKey(const CVUIntegerDictionaryRef dictionary, const CVUInteger key){
	CVUIntegerDictionaryEntry* entry = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), &key, sizeof(CVUInteger), entry);
	}
	if(entry){
		return entry->data;
	}else{
		return NULL;
	}
}

void* CVUIntegerDictionarySetEntry(CVUIntegerDictionaryRef dictionary, const CVUInteger key, void* data){
	CVUIntegerDictionaryEntry* entry = calloc(1,sizeof(CVUIntegerDictionaryEntry));
	entry->key = key;
	entry->data = data;
	CVUIntegerDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), &(entry->key), sizeof(CVUInteger), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp);
	}
	HASH_ADD_KEYPTR( hh, (*dictionary), (&entry->key), sizeof(CVUInteger), entry);
	return replacedData;
}

void* CVUIntegerDictionaryDeleteEntry(CVUIntegerDictionaryRef dictionary, const CVUInteger key){
	CVUIntegerDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (&key), sizeof(CVUInteger), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp);
	}
	return replacedData;
}

void* CVUIntegerDictionaryDeleteAndFreeEntry(CVUIntegerDictionaryRef dictionary, const CVUInteger key){
	CVUIntegerDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (&key), sizeof(CVUInteger), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->data);
		free(entryTemp);
	}
	return replacedData;
}

void CVUIntegerDictionaryClear(CVUIntegerDictionaryRef dictionary){
	CVUIntegerDictionaryEntry* entry = NULL;
	CVUIntegerDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry);
	}
}

void CVUIntegerDictionaryClearAndFree(CVUIntegerDictionaryRef dictionary){
	CVUIntegerDictionaryEntry* entry = NULL;
	CVUIntegerDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->data);
		free(entry);
	}
}

void CVUIntegerDictionaryDestroy(CVUIntegerDictionaryRef dictionary){
	CVUIntegerDictionaryClear(dictionary);
	free(dictionary);
}



/////// CVInteger

CVIntegerDictionaryRef CVNewIntegerDictionary(){
	return calloc(1, sizeof(CVIntegerDictionary));
}

void* CVIntegerDictionaryEntryForKey(const CVIntegerDictionaryRef dictionary, const CVInteger key){
	CVIntegerDictionaryEntry* entry = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (&key), sizeof(CVInteger), entry);
	}
	if(entry){
		return entry->data;
	}else{
		return NULL;
	}
}

void* CVIntegerDictionarySetEntry(CVIntegerDictionaryRef dictionary, const CVInteger key, void* data){
	CVIntegerDictionaryEntry* entry = calloc(1,sizeof(CVIntegerDictionaryEntry));
	entry->key = key;
	entry->data = data;
	CVIntegerDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), &(entry->key), sizeof(CVInteger), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp);
	}
	HASH_ADD_KEYPTR( hh, (*dictionary), &(entry->key), sizeof(CVInteger), entry);
	return replacedData;
}

void* CVIntegerDictionaryDeleteEntry(CVIntegerDictionaryRef dictionary, const CVInteger key){
	CVIntegerDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), &(key), sizeof(CVInteger), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp);
	}
	return replacedData;
}

void* CVIntegerDictionaryDeleteAndFreeEntry(CVIntegerDictionaryRef dictionary, const CVInteger key){
	CVIntegerDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), &(key), sizeof(CVInteger), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->data);
		free(entryTemp);
	}
	return replacedData;
}

void CVIntegerDictionaryClear(CVIntegerDictionaryRef dictionary){
	CVIntegerDictionaryEntry* entry = NULL;
	CVIntegerDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry);
	}
}

void CVIntegerDictionaryClearAndFree(CVIntegerDictionaryRef dictionary){
	CVIntegerDictionaryEntry* entry = NULL;
	CVIntegerDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->data);
		free(entry);
	}
}

void CVIntegerDictionaryDestroy(CVIntegerDictionaryRef dictionary){
	CVIntegerDictionaryClear(dictionary);
	free(dictionary);
}




/////// CVEdge

CVEdgeDictionaryRef CVNewEdgeDictionary(){
	return calloc(1, sizeof(CVEdgeDictionary));
}

void* CVEdgeDictionaryEntryForKey(const CVEdgeDictionaryRef dictionary, const CVEdge key){
	CVEdgeDictionaryEntry* entry = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (&key), sizeof(CVEdge), entry);
	}
	if(entry){
		return entry->data;
	}else{
		return NULL;
	}
}

void* CVEdgeDictionarySetEntry(CVEdgeDictionaryRef dictionary, const CVEdge key, void* data){
	CVEdgeDictionaryEntry* entry = calloc(1,sizeof(CVEdgeDictionaryEntry));
	entry->key = key;
	entry->data = data;
	CVEdgeDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), &(entry->key), sizeof(CVEdge), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp);
	}
	HASH_ADD_KEYPTR( hh, (*dictionary), &(entry->key), sizeof(CVEdge), entry);
	return replacedData;
}

void* CVEdgeDictionaryDeleteEntry(CVEdgeDictionaryRef dictionary, const CVEdge key){
	CVEdgeDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (&key), sizeof(CVEdge), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp);
	}
	return replacedData;
}

void* CVEdgeDictionaryDeleteAndFreeEntry(CVEdgeDictionaryRef dictionary, const CVEdge key){
	CVEdgeDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (&key), sizeof(CVEdge), entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->data);
		free(entryTemp);
	}
	return replacedData;
}

void CVEdgeDictionaryClear(CVEdgeDictionaryRef dictionary){
	CVEdgeDictionaryEntry* entry = NULL;
	CVEdgeDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry);
	}
}

void CVEdgeDictionaryClearAndFree(CVEdgeDictionaryRef dictionary){
	CVEdgeDictionaryEntry* entry = NULL;
	CVEdgeDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->data);
		free(entry);
	}
}

void CVEdgeDictionaryDestroy(CVEdgeDictionaryRef dictionary){
	CVEdgeDictionaryClear(dictionary);
	free(dictionary);
}






/////// CVGeneric

CVGenericDictionaryRef CVNewGenericDictionary(){
	return calloc(1, sizeof(CVGenericDictionary));
}

void* CVGenericDictionaryEntryForKey(const CVGenericDictionaryRef dictionary, const void* key, CVSize keysize){
	CVGenericDictionaryEntry* entry = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), key, keysize, entry);
	}
	if(entry){
		return entry->data;
	}else{
		return NULL;
	}
}

void* CVGenericDictionarySetEntry(CVGenericDictionaryRef dictionary, const void* key, CVSize keysize, void* data){
	CVGenericDictionaryEntry* entry = calloc(1,sizeof(CVGenericDictionaryEntry));
	entry->key = calloc(1, keysize);
	entry->data = data;
	memcpy(entry->key, key, keysize);
	CVGenericDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), (entry->key), keysize, entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->key);
		free(entryTemp);
	}
	HASH_ADD_KEYPTR( hh, (*dictionary), (entry->key), keysize, entry);
	return replacedData;
}

void* CVGenericDictionaryDeleteEntry(CVGenericDictionaryRef dictionary, const void* key, CVSize keysize){
	CVGenericDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), key, keysize, entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->key);
		free(entryTemp);
	}
	return replacedData;
}

void* CVGenericDictionaryDeleteAndFreeEntry(CVGenericDictionaryRef dictionary, const void* key, CVSize keysize){
	CVGenericDictionaryEntry* entryTemp = NULL;
	void* replacedData = NULL;
	if(dictionary){
		HASH_FIND(hh, (*dictionary), key, keysize, entryTemp);
	}
	if(entryTemp){
		replacedData = entryTemp->data;
		HASH_DEL((*dictionary), entryTemp);
		free(entryTemp->data);
		free(entryTemp->key);
		free(entryTemp);
	}
	return replacedData;
}

void CVGenericDictionaryClear(CVGenericDictionaryRef dictionary){
	CVGenericDictionaryEntry* entry = NULL;
	CVGenericDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->key);
		free(entry);
	}
}

void CVGenericDictionaryClearAndFree(CVGenericDictionaryRef dictionary){
	CVGenericDictionaryEntry* entry = NULL;
	CVGenericDictionaryEntry* entryTemp = NULL;
	HASH_ITER(hh, (*dictionary), entry, entryTemp) {
		HASH_DEL((*dictionary), entry);
		free(entry->data);
		free(entry->key);
		free(entry);
	}
}

void CVGenericDictionaryDestroy(CVGenericDictionaryRef dictionary){
	CVGenericDictionaryClear(dictionary);
	free(dictionary);
}


