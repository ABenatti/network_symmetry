//
//  CVMotifs.c
//  CVNetwork
//
//  Created by Alexandre Benatti, Henrique Ferraz de Arruda, and Filipi Nascimento Silva on 25/06/21.
//  Copyright (c) 2014 Filipi Nascimento Silva. All rights reserved.
//
#define PY_SSIZE_T_CLEAN
#include "PyCXVersion.h"
#include <CVDistribution.h>
#include <CVNetwork.h>
#include <CVSet.h>
#include <Python.h>
#include <pthread.h>
#include <CVConcentricStructure.h>
#include <CVNetworkSymmetry.h>
#include <getopt.h>
#include <CVNetworkCentrality.h>
#include "CVSymmetryApplication.h"

#include "structmember.h"

// #define NO_IMPORT_ARRAY
#include <numpy/arrayobject.h>

#if CV_USE_OPENMP
#include <omp.h>
#endif //_OPENMP

static PyArrayObject * pyvector(PyObject *objin){
	return (PyArrayObject *)PyArray_ContiguousFromObject(objin, NPY_FLOAT, 1, 1);
}

static PyArrayObject * convertToUIntegerArray(PyObject *object, int minDepth, int maxDepth) {
	int flags = NPY_ARRAY_C_CONTIGUOUS | NPY_ARRAY_ALIGNED;
	return PyArray_FromAny(
		object, PyArray_DescrFromType(NPY_UINT64), minDepth, maxDepth, flags, NULL);
}
static PyArrayObject * convertToIntegerArray(PyObject *object, int minDepth, int maxDepth){
	int flags = NPY_ARRAY_C_CONTIGUOUS | NPY_ARRAY_ALIGNED;
	return PyArray_FromAny(
		object, PyArray_DescrFromType(NPY_INT64), minDepth, maxDepth, flags, NULL);
}

static PyArrayObject * convertToDoubleArray(PyObject *object, int minDepth, int maxDepth){
	int flags = NPY_ARRAY_C_CONTIGUOUS | NPY_ARRAY_ALIGNED;
	return PyArray_FromAny(object,
							 PyArray_DescrFromType(NPY_FLOAT64),
							 minDepth,
							 maxDepth,
							 flags,
							 NULL);
}

static PyArrayObject * convertToFloatArray(PyObject *object, int minDepth, int maxDepth){
	int flags = NPY_ARRAY_C_CONTIGUOUS | NPY_ARRAY_ALIGNED;
	return PyArray_FromAny(object,
							 PyArray_DescrFromType(NPY_FLOAT32),
							 minDepth,
							 maxDepth,
							 flags,
							 NULL);
}

/* ==== Create 1D Carray from PyArray ======================
																																Assumes PyArray
	 is contiguous in memory.             */
static void * pyvector_to_Carrayptrs(PyArrayObject *arrayin){
	int i, n;

	n = arrayin->dimensions[0];
	return PyArray_DATA(arrayin); /* pointer to arrayin data as double */
}

/* ==== Check that PyArrayObject is a double (Float) type and a vector
				 ============== return 1 if an error and raise exception */
static int not_floatvector(PyArrayObject *vec){
	if (vec->descr->type_num != NPY_FLOAT) {
		PyErr_SetString(PyExc_ValueError,
						"In not_floatvector: array must be of "
						"type Float and 1 dimensional (n).");
		return 1;
	}
	return 0;
}

/* ==== Check that PyArrayObject is a double (Float) type and a vector
				 ============== return 1 if an error and raise exception */
// FIXME: make it work for 32bits
static int not_intvector(PyArrayObject *vec){
	if (vec->descr->type_num != NPY_UINT64) {
		PyErr_SetString(
			PyExc_ValueError,
			"In not_intvector: array must be of type Long and 1 dimensional (n).");
		return 1;
	}
	return 0;
}

typedef struct _PyNetwork{
	PyObject_HEAD CVNetwork *network;
	CVBool verbose;
} PyMeasurer;

int PyMeasurer_traverse(PyMeasurer *self, visitproc visit, void *arg){
	// Py_VISIT(self->...);
	return 0;
}

int PyMeasurer_clear(PyMeasurer *self){
	// Py_CLEAR(self->...);
	return 0;
}

void PyMeasurer_dealloc(PyMeasurer *self){ // USAR ISSO
	if (self->network) {
		CVNetworkDestroy(self->network);
	}
	Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *PyMeasurer_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
	PyMeasurer *self;
	self = (PyMeasurer *)type->tp_alloc(type, 0);
	self->network = NULL;
	return (PyObject *)self;
}

int PyMeasurer_init(PyMeasurer *self, PyObject *args, PyObject *kwds){
	CVRandomSeedDev();
	static char *kwlist[] = {
		"vertexCount",
		"edges",
		"directed",
		"weights",
		NULL
	};
	PyObject *edgesObject = NULL;
	PyObject *weightsObject = NULL;
	PyArrayObject *edgesArray = NULL;
	PyArrayObject *weightsArray = NULL;
	Py_ssize_t vertexCount = 0;
	int isDirected = 0;

	if (!PyArg_ParseTupleAndKeywords(args,
									 kwds,
									 "nO|pO",
									 kwlist,
									 &vertexCount,
									 &edgesObject,
									 &isDirected,
									 &weightsObject)) {
		return -1;
	}

	if (vertexCount <= 0) {
		PyErr_SetString(PyExc_TypeError,
						"The number of ndoes (vertexCount) must be a positive integer.");
		return -1;
	}

	if (!(edgesArray = convertToIntegerArray(edgesObject, 1, 2))) {
		// PyErr_SetString(PyExc_TypeError,"Error creating arrays.");
		return -1;
	}

	CVSize edgeCount = (CVSize)PyArray_SIZE(edgesArray) / 2;
	npy_int64 *edges = PyArray_DATA(edgesArray);

	if (weightsObject &&
		!(weightsArray = convertToDoubleArray(weightsObject, 1, 1))) {
		// PyErr_SetString(PyExc_TypeError,"The weights attribute must be a float32
		// numpy array.");
		Py_XDECREF(edgesArray);
		return -1;
	}

	CVSize weightsCount = 0;
	double *weights = NULL;

	if (weightsArray) {
		weightsCount = (CVSize)PyArray_SIZE(weightsArray);
		weights = PyArray_DATA(weightsArray);
	}

	if (weights && weightsCount != edgeCount) {
		PyErr_SetString(
			PyExc_TypeError,
			"Weights should have the same dimension as the number of edges.");
		Py_XDECREF(edgesArray);
		Py_XDECREF(weightsArray);
		return -1;
	}

	self->network = CVNewNetwork(
		vertexCount, weights ? CVTrue : CVFalse, isDirected ? CVTrue : CVFalse);
	for (CVIndex i = 0; i < edgeCount; i++) {
		CVIndex fromIndex = (CVIndex)edges[2 * i];
		CVIndex toIndex = (CVIndex)edges[2 * i + 1];
		CVDouble weight = 1.0;
		if (fromIndex >= vertexCount || toIndex >= vertexCount) {
			PyErr_SetString(
				PyExc_TypeError,
				"Edge indices should not be higher than the number of vertices.");
			Py_XDECREF(edgesArray);
			Py_XDECREF(weightsArray);
			return -1;
		}
		if (weights) {
			weight = weights[i];
		}
		CVNetworkAddNewEdge(self->network, fromIndex, toIndex, weight);
	}

	Py_XDECREF(edgesArray);
	Py_XDECREF(weightsArray);
	return 0;
}

PyMemberDef PyMeasurer_members[] = {
	{NULL}
};

static PyGetSetDef PyMeasurer_getsetters[] = {
	{NULL}
};


PyObject *PyMeasurer_compute(PyMeasurer *self, PyObject *commandList){
	CVNetwork *network = self->network;
	CVSize verticesCount = network->verticesCount;

	CVIndex commandListLength = PySequence_Fast_GET_SIZE(commandList);

	//Counting valid inputs
	CVIndex size = 1;
	for (CVIndex i =0; i < commandListLength; i++){
		if (strcmp(CVNewStringFromString((CVString)PyUnicode_AsUTF8(PySequence_Fast_GET_ITEM(commandList, i))), "") != 0){
			size++;
		}	
	}
	//Cleaning inputs
	char **commandLine = calloc((size+1), sizeof(char*));
	commandLine[0] = CVNewStringFromString("");

	CVIndex pos = 0;
	for (CVIndex i =0; i < commandListLength; i++){
		CVString currentCommandString = CVNewStringFromString((CVString)PyUnicode_AsUTF8(PySequence_Fast_GET_ITEM(commandList, i)));
		if (strcmp(currentCommandString, "") != 0){
			commandLine[pos+1] = currentCommandString;
			pos ++;
		}else{
			CVStringDestroy(currentCommandString);
		}
	}

	CVSymmetryOutputParameters **results;
	CVInteger level = -1;
	CVInteger* level_pointer = &level;
	
	results = calloc(network->verticesCount, sizeof(CVSymmetryOutputParameters*));

	CVSymmetryApplication(network, (int) size, commandLine, results, level_pointer);

	PyListObject* pySymmetries = NULL;
	CVIndex numberOfLists = ((CVIndex) level-1) * 3;  

	pySymmetries = PyList_New(numberOfLists);
	
	PyListObject* measurement = NULL;
	CVIndex positionInList = 0;
	CVSymmetryOutputParameters* vertexOutput = NULL;
	
	
	CVFloat aux = 0.;
	PyObject* value = NULL;
	//Accessibility
	for (CVIndex i=2; i<=(CVIndex)level; i++){
		measurement = PyList_New(0);
		for (CVIndex j=0; j < verticesCount; j++){
			aux = results[j]->backboneAccessibility[i];
			value = Py_BuildValue("d", aux);
			PyList_Append(measurement, value);
			Py_DECREF(value);
		}
		PyList_SET_ITEM(pySymmetries,positionInList,measurement);
	 	positionInList++;
	}

	//Symmetry Backbone
	for (CVIndex i=2; i<=(CVIndex)level; i++){
		measurement = PyList_New(0);
		for (CVIndex j=0; j < verticesCount; j++){
			aux = results[j]->normalizedBackboneAccessibility[i];
			value = Py_BuildValue("d", aux);
			PyList_Append(measurement, value);
			Py_DECREF(value);
		}
		PyList_SET_ITEM(pySymmetries,positionInList,measurement);
	 	positionInList++;
	}

	//Symmetry Merged
	for (CVIndex i=2; i<=(CVIndex)level; i++){
		measurement = PyList_New(0);
		for (CVIndex j=0; j < verticesCount; j++){
			aux = results[j]->normalizedMergedAccessibility[i];
			value = Py_BuildValue("d", aux);
			PyList_Append(measurement, value);
			Py_DECREF(value);
		}
		PyList_SET_ITEM(pySymmetries,positionInList,measurement);
	 	positionInList++;
	}

	//Cleaning memory
	for (CVIndex i=0; i<network->verticesCount; i++) {
		CVSymmetryOutputDestroy(results[i]);
	}
	free(results);

	//Arruamr aqui
	for (CVIndex i=0; i < size; i++){
		CVStringDestroy(commandLine[i]);
	}
	free(commandLine);
	
	return (PyListObject*) pySymmetries;
}

static PyMethodDef PyMeasurer_methods[] = {
	//Colocar aqui os métodos que serão lidos no python
	{"_compute",
	 (PyCFunction)PyMeasurer_compute,
	 METH_VARARGS | METH_KEYWORDS,
	 "Computing measurements."},
	{NULL} /* Sentinel */
};

static PyTypeObject PyMeasurerType = {
	// neste caso from networkSymetry import Measurer
	PyVarObject_HEAD_INIT(NULL, 0).tp_name = "network_symmetry_core.Measurer",
	.tp_doc = "PyMeasurer objects",
	.tp_basicsize = sizeof(PyMeasurer),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // | Py_TPFLAGS_HAVE_GC,
	.tp_new = PyMeasurer_new,
	.tp_init = (initproc)PyMeasurer_init,
	.tp_dealloc = (destructor)PyMeasurer_dealloc,
	.tp_traverse = NULL, //(traverseproc) PyMeasurer_traverse,
	.tp_clear = NULL,	 //(inquiry) PyMeasurer_clear,
	.tp_members = PyMeasurer_members,
	.tp_methods = PyMeasurer_methods,
	.tp_getset = PyMeasurer_getsetters,
};

char cxsymmetrymod_docs[] = "This is the network-Symmetry module.";

static PyModuleDef networksymmetry_mode = {PyModuleDef_HEAD_INIT,
										 .m_name = "network_symmetry_core",
										 .m_doc = cxsymmetrymod_docs,
										 .m_size = -1,
										 .m_methods = NULL,
										 .m_slots = NULL,
										 .m_traverse = NULL,
										 .m_clear = NULL,
										 .m_free = NULL};

PyMODINIT_FUNC PyInit_network_symmetry_core(void){
	import_array();

	PyObject *m;
	if (PyType_Ready(&PyMeasurerType) < 0) {
		return NULL;
	}
	m = PyModule_Create(&networksymmetry_mode);
	if (m == NULL) {
		return NULL;
	}
	Py_INCREF(&PyMeasurerType);
	if (PyModule_AddObject(m, "Measurer", (PyObject *)&PyMeasurerType) < 0) {
		Py_DECREF(&PyMeasurerType);
		Py_DECREF(m);
		return NULL;
	}

	if (PyModule_AddStringConstant(m,"__version__",CVTOKENTOSTRING(k_PYCXVersion))<0) {
			Py_DECREF(m);
			return NULL;
	}

	return m;
}
