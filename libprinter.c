#include <Python.h>
#include <numpy/arrayobject.h>
#include <numpy/arrayscalars.h>
#include <numpy/ndarraytypes.h>


/*
Creates an array of NpyIter structs in the same order as the arrays supplied.

Caller is responsible for freeing memory. Returns NULL on error
*/
static NpyIter** initiateIters(PyObject *arrList ) {
  NpyIter **npyIters = PyObject_Malloc(sizeof(NpyIter *) * PyObject_Length(arrList));
  if (npyIters == NULL) {
    PyErr_NoMemory();
    return NULL;
  }

  for (Py_ssize_t i = 0; i < PyObject_Length(arrList); i++) {
    PyArrayObject *arr = (PyArrayObject *) PyList_GET_ITEM(arrList, i);

    
    // Check contents of each numpy array
    NpyIter* iter = NpyIter_New(arr, NPY_ITER_READONLY,
			      NPY_KEEPORDER, NPY_NO_CASTING,
			      NULL);

    if (iter == NULL) {
      if (i > 0) {
	while(--i) {
	  NpyIter_Deallocate(npyIters[i]);
	}
      }

      PyErr_NoMemory();
      return NULL;
    }
    
    npyIters[i] = iter;
  }

  return npyIters;
}

/*
Free an array of numpy array iterators. 

TODO: dynamically calculate how many to free rather than require length as arg
*/
static void freeIters(NpyIter **iters, Py_ssize_t length) {
  for (Py_ssize_t i = 0; i < length; i++ ) {
    NpyIter_Deallocate(iters[i]);
  }
}

static void printByColumn(PyObject *arrList) {
  Py_ssize_t colcount = PyObject_Length(arrList);
  printf("Here is the data printed by column\n");
  for (Py_ssize_t i = 0; i < colcount; i++) {
    PyArrayObject *arr = (PyArrayObject *) PyList_GET_ITEM(arrList, i);

    
    // Check contents of each numpy array
    NpyIter* iter = NpyIter_New(arr, NPY_ITER_READONLY,
			      NPY_KEEPORDER, NPY_NO_CASTING,
			      NULL);

    if (iter == NULL) {
      // TODO: set some error here
      return;
    }

    NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(iter, NULL);
    if (iternext == NULL) {
      NpyIter_Deallocate(iter);
      // TODO: set some error here
      return;
    }
    char** dataptr = NpyIter_GetDataPtrArray(iter);
    do {
      printf("%lld ", (long long)**dataptr);   // TODO - this cast isn't correct
    } while(iternext(iter));
    
    NpyIter_Deallocate(iter);
    printf("\n");
  }
}

static void printByRow(PyObject *arrList, Py_ssize_t rowcount) {
  Py_ssize_t colcount = PyObject_Length(arrList);
  printf("Here is the data printed by row\n");
  NpyIter **npyIters = initiateIters(arrList);

  // 2 hard coded from input example; should replace with df dims  
  for (Py_ssize_t rowindex = 0; rowindex < rowcount; rowindex++) { 
    for (Py_ssize_t i = 0; i < colcount; i++) {
      NpyIter *iter = npyIters[i];

      // TODO: possibly create a struct that holds iter / iternext together
      // so that we don't need to call NpyIter_GetIterNext within this loop
      NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(iter, NULL);
      if (iternext == NULL) {
	freeIters(npyIters, colcount);
	// TODO: error handling	
      }
      
      char** dataptr = NpyIter_GetDataPtrArray(iter);

      // TODO: here we get the descr of the array to figure out printing
      // we may want to bundle this in a struct with the iter / iternext
      // discussion above
      PyArrayObject *arr = (PyArrayObject *) PyList_GET_ITEM(arrList, i);
      if (arr->descr->kind == 'i') {
      }
      printf("%lld ", (long long)**dataptr);  // TODO - this cast isn't correct
      iternext(iter);
    }

    printf("\n");
  }

  freeIters(npyIters, colcount);
}

PyObject *print_frame(PyObject *self, PyObject *args, PyObject *kwargs) {
  static char *kwlist[] = {"obj", NULL};
  PyObject *dataframe;


  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &dataframe)) {
    return NULL;
  }

  PyObject *mgr = PyObject_GetAttrString(dataframe, "_mgr");
  if (mgr == NULL) {
    return NULL;
  }
  
  PyObject *arrList = PyObject_GetAttrString(mgr, "column_arrays");
  Py_DECREF(mgr);
  if (arrList == NULL) {
    return NULL;
  }

  Py_ssize_t df_len = PyObject_Length(dataframe);
  printByRow(arrList, df_len);
  printByColumn(arrList);

  return Py_None;
};
static PyMethodDef methods[] = {
  {"print_frame", (PyCFunction)print_frame, METH_VARARGS | METH_KEYWORDS, ""},
  {NULL, NULL, 0, NULL} // Sentinel
};

static PyModuleDef moduledef = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "libprinter",
  .m_methods = methods
};

PyMODINIT_FUNC PyInit_libprinter(void) {
  import_array();
  return PyModuleDef_Init(&moduledef);
}
