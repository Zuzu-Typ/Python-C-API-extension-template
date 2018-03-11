//#include "C:\Python36\include\Python.h"
#include <Python.h>
#include "structmember.h"
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include <stdbool.h>

#define PY3K (PY_VERSION_HEX >= 0x03000000)

double pi; // later we import this value from the math module

#if PY3K
#define Py_TPFLAGS_CHECKTYPES 0
#define PyNumeric_Check(op) (PyLong_Check(op) || PyFloat_Check(op) || PyBool_Check(op))
#endif

#if !PY3K
#define Py_RETURN_NOTIMPLEMENTED return Py_INCREF(Py_NotImplemented), Py_NotImplemented
#define PyLong_AS_LONG(op) PyLong_AsLong(op)
#define PyNumeric_Check(op) (PyLong_Check(op) || PyInt_Check(op) || PyFloat_Check(op) || PyBool_Check(op))
#endif

#define Py_IS_NOTIMPLEMENTED(op) (op == NULL || (PyObject*)op == Py_NotImplemented) // find out if op is NULL or NotImplemented

static double PyNumeric_as_double(PyObject * op) {
	double out;
	if (PyFloat_Check(op)) {
		out = PyFloat_AS_DOUBLE(op);
		return out;
	}
	else if (PyLong_Check(op)) {
		out = (double)PyLong_AS_LONG(op);
		return out;
	}
#if !PY3K
	else if (PyInt_Check(op)) {
		out = (double)PyInt_AS_LONG(op);
		return out;
	}
#endif
	else {
		out = (PyObject_IsTrue(op)) ? 1 : 0;
		return out;
	}
}
#if !PY3K
#define PyNumeric_AS_DOUBLE(op) ((PyFloat_Check(op)) ? PyFloat_AS_DOUBLE(op) : (PyLong_Check(op)) ? (double)PyLong_AS_LONG(op) : (PyInt_Check(op)) ? (double)PyInt_AS_LONG(op) : (PyObject_IsTrue(op)) ? 1 : 0)
#else
#define PyNumeric_AS_DOUBLE(op) ((PyFloat_Check(op)) ? PyFloat_AS_DOUBLE(op) : (PyLong_Check(op)) ? (double)PyLong_AS_LONG(op) : (PyObject_IsTrue(op)) ? 1 : 0)
#endif

#define PyType_AS_CSTRING(op) op->ob_type->tp_name

#define Py_RAISE_TYPEERROR_O(str, obj) PyErr_Format(PyExc_TypeError, "%s'%s'", str, PyType_AS_CSTRING(obj))
#define Py_RAISE_TYPEERROR_2O(str, obj1, obj2) PyErr_Format(PyExc_TypeError, "%s'%s' and '%s'", str, PyType_AS_CSTRING(obj1), PyType_AS_CSTRING(obj2))

static char * attr_name_to_cstr(PyObject * name) {
#if PY3K
	return PyBytes_AsString(PyUnicode_AsASCIIString(name));
#else
	return PyString_AsString(name);
#endif
}

// example_class
static PyTypeObject example_classType;
static PyTypeObject example_classIterType;

typedef struct {
	PyObject_HEAD
		double value;
} example_class;

typedef struct {
	PyObject_HEAD
		Py_ssize_t seq_index;
	example_class *sequence;
} example_classIter;

typedef struct {
	double value;
} internal_example_class;


static PyObject* pack_example_class(double value) {
	example_class* out = (example_class*)example_classType.tp_alloc(&example_classType, 0);

	if (out != NULL) {
		out->value = value;
	}

	return (PyObject*)out;
}

static bool unpack_example_class(PyObject * op, internal_example_class* out) {
	if (PyObject_TypeCheck(op, &example_classType)) {
		out->value = ((example_class*)op)->value;
		return true;
	}
	if (PyNumeric_Check(op)) {
		out->value = PyNumeric_as_double(op);
		return true;
	}
	return false;

}

static void
example_class_dealloc(example_class* self)
{
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
example_class_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	example_class *self;

	self = (example_class *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->value = 0.0;
	}

	return (PyObject *)self;
}

static int
example_class_init(example_class *self, PyObject *args, PyObject *kwargs)
{
	static char *kwlist[] = { "value", NULL };

	PyObject * arg1 = NULL;

	if (PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist,
		&arg1)) {
		if (arg1 == NULL) {
			return 0;
		}
		if (PyNumeric_Check(arg1)) {
			self->value = PyNumeric_as_double(arg1);
			return 0;
		}
	}
	PyErr_SetString(PyExc_TypeError, "invalid argument type(s) for example_class()");
	return -1;
}

// unaryfunc
static PyObject *
example_class_neg(example_class *obj)
{
	return pack_example_class(-obj->value);
}

static PyObject *
example_class_pos(example_class *obj)
{
	return pack_example_class(obj->value);
}

static PyObject *
example_class_abs(example_class *obj)
{
	return pack_example_class(fabs(obj->value));
}

// binaryfunc
static PyObject *
example_class_add(PyObject *obj1, PyObject *obj2)
{
	// combines __add__ and __radd__!!
	internal_example_class o1, o2;
	
	if (unpack_example_class(obj1, &o1) && unpack_example_class(obj2, &o2)) {
		return pack_example_class(
			o1.value + o2.value
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *
example_class_sub(PyObject *obj1, PyObject *obj2)
{
	internal_example_class o1, o2;

	if (unpack_example_class(obj1, &o1) && unpack_example_class(obj2, &o2)) {
		return pack_example_class(
			o1.value - o2.value
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *
example_class_mul(PyObject *obj1, PyObject *obj2)
{
	internal_example_class o1, o2;

	if (unpack_example_class(obj1, &o1) && unpack_example_class(obj2, &o2)) {
		return pack_example_class(
			o1.value * o2.value
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *
example_class_div(PyObject *obj1, PyObject *obj2)
{
	internal_example_class o1, o2;

	if (unpack_example_class(obj1, &o1) && unpack_example_class(obj2, &o2)) {
		return pack_example_class(
			o1.value / o2.value
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *
example_class_mod(PyObject *obj1, PyObject *obj2)
{
	internal_example_class o1, o2;

	if (unpack_example_class(obj1, &o1) && unpack_example_class(obj2, &o2)) {
		return pack_example_class(
			fmod(o1.value, o2.value)
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *
example_class_floordiv(PyObject *obj1, PyObject *obj2)
{
	internal_example_class o1, o2;

	if (unpack_example_class(obj1, &o1) && unpack_example_class(obj2, &o2)) {
		return pack_example_class(
			floor(o1.value / o2.value)
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *
example_class_divmod(PyObject * obj1, PyObject * obj2) {
	return Py_BuildValue("(OO)", example_class_floordiv(obj1, obj2), example_class_mod(obj1, obj2));
}

// ternaryfunc
static PyObject *
example_class_pow(PyObject * obj1, PyObject * obj2, PyObject * obj3) {
	internal_example_class o1, o2;

	if (!unpack_example_class(obj1, &o1) || !unpack_example_class(obj2, &o2)) {
		Py_RETURN_NOTIMPLEMENTED;
	}

	if (obj3 == Py_None) {
		return pack_example_class(
			pow(o1.value, o2.value)
		);
	}

	internal_example_class o3;

	if (unpack_example_class(obj1, &o3)) {
		return pack_example_class(
			fmod(pow(o1.value, o2.value), o3.value)
		);
	}

	Py_RETURN_NOTIMPLEMENTED;
}

// inplace
// binaryfunc
static PyObject *
example_class_iadd(example_class *self, PyObject *obj)
{
	example_class * temp = (example_class*)example_class_add((PyObject*)self, obj);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject *
example_class_isub(example_class *self, PyObject *obj)
{
	example_class * temp = (example_class*)example_class_sub((PyObject*)self, obj);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject *
example_class_imul(example_class *self, PyObject *obj)
{
	example_class * temp = (example_class*)example_class_mul((PyObject*)self, obj);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject *
example_class_idiv(example_class *self, PyObject *obj)
{
	example_class * temp = (example_class*)example_class_div((PyObject*)self, obj);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject *
example_class_imod(example_class *self, PyObject *obj)
{
	example_class * temp = (example_class*)example_class_mod((PyObject*)self, obj);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject *
example_class_ifloordiv(example_class *self, PyObject *obj)
{
	example_class * temp = (example_class*)example_class_floordiv((PyObject*)self, obj);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

// ternaryfunc
static PyObject *
example_class_ipow(example_class *self, PyObject *obj1, PyObject * obj2) // obj2 is unused. It points to an invalid address!
{
	example_class * temp = (example_class*)example_class_pow((PyObject*)self, obj1, Py_None);

	if (Py_IS_NOTIMPLEMENTED(temp)) return (PyObject*)temp;

	self->value = temp->value;

	Py_DECREF(temp);
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyMemberDef example_class_members[] = {
	{ "value", T_DOUBLE, offsetof(example_class, value), 0,
	"value of example_class" },
{ NULL }  /* Sentinel */
};

static PyObject *
example_class_str(example_class* self)
{
	char * out = (char*)malloc((30) * sizeof(char));
	snprintf(out, 30, "example_class( %12.6g )", self->value);
	PyObject* po = PyUnicode_FromString(out);
	free(out);
	return po;
}

static Py_ssize_t example_class_len(example_class * self) {
	return (Py_ssize_t)1;
}

static PyObject* example_class_sq_item(example_class * self, Py_ssize_t index) {
	switch (index) {
	case 0:
		return PyFloat_FromDouble(self->value);
	case -1:
		return PyFloat_FromDouble(self->value);
	default:
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}
}

static int example_class_sq_setitem(example_class * self, Py_ssize_t index, PyObject * value) {
	double value_as_double;
	if (PyNumeric_Check(value)) {
		value_as_double = PyNumeric_as_double(value);
	}
	else {
		Py_RAISE_TYPEERROR_O("must be a real number, not ", value);
		return -1;
	}
	switch (index) {
	case 0:
		self->value = value_as_double;
		return 0;
	case -1:
		self->value = value_as_double;
		return 0;
	default:
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return -1;
	}
}

static int example_class_contains(example_class * self, PyObject * value) {
	double d;
	if (PyNumeric_Check(value)) {
		d = PyNumeric_as_double(value);
		return (int)(d == self->value);
	}
	return 0;

}

static PyObject * example_class_richcompare(example_class * self, PyObject * other, int comp_type) {
	internal_example_class o2;

	if (!unpack_example_class(other, &o2)) {
		if (comp_type == Py_EQ || comp_type == Py_NE) {
			Py_RETURN_FALSE;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}

	switch (comp_type) {
	case Py_EQ :
		if (self->value == o2.value) Py_RETURN_TRUE;
		else Py_RETURN_FALSE;
		break;
	case Py_NE :
		if (self->value != o2.value) Py_RETURN_TRUE;
		else Py_RETURN_FALSE;
		break;
	case Py_LT :
		if (self->value < o2.value) Py_RETURN_TRUE;
		else Py_RETURN_FALSE;
		break;
	case Py_LE :
		if (self->value <= o2.value) Py_RETURN_TRUE;
		else Py_RETURN_FALSE;
		break;
	case Py_GT :
		if (self->value > o2.value) Py_RETURN_TRUE;
		else Py_RETURN_FALSE;
		break;
	case Py_GE :
		if (self->value >= o2.value) Py_RETURN_TRUE;
		else Py_RETURN_FALSE;
		break;
	default :
		Py_RETURN_NOTIMPLEMENTED;
	}
}

static PyObject * example_class_getattr(PyObject * obj, PyObject * name) {
	char * name_as_cstr = attr_name_to_cstr(name);

	if (strcmp(name_as_cstr, "secret") == 0) {
		return PyFloat_FromDouble(pi);
	}

	return PyObject_GenericGetAttr(obj, name);
}

// iterator

static PyObject *
example_classIter_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	example_class *sequence;

	if (!PyArg_UnpackTuple(args, "__iter__", 1, 1, &sequence))
		return NULL;

	/* Create a new example_classIter and initialize its state - pointing to the last
	* index in the sequence.
	*/
	example_classIter *rgstate = (example_classIter *)type->tp_alloc(type, 0);
	if (!rgstate)
		return NULL;

	rgstate->sequence = sequence;
	Py_INCREF(sequence);
	rgstate->seq_index = 0;

	return (PyObject *)rgstate;
}

static void
example_classIter_dealloc(example_classIter *rgstate)
{
	Py_XDECREF(rgstate->sequence);
	Py_TYPE(rgstate)->tp_free(rgstate);
}

static PyObject *
example_classIter_next(example_classIter *rgstate)
{
	if (rgstate->seq_index < 1) {
		if (rgstate->seq_index == 0) {
			rgstate->seq_index++;
			return PyFloat_FromDouble(rgstate->sequence->value);
		}
	}
	rgstate->seq_index = 1;
	Py_CLEAR(rgstate->sequence);
	return NULL;
}

static PyObject * example_class_geniter(example_class * self) {
	example_classIter *rgstate = (example_classIter *)((&example_classIterType)->tp_alloc(&example_classIterType, 0));
	if (!rgstate)
		return NULL;

	rgstate->sequence = self;
	Py_INCREF(self);
	rgstate->seq_index = 0;

	return (PyObject *)rgstate;
}

static PySequenceMethods example_classSeqMethods = {
	(lenfunc)example_class_len, // sq_length
	0, // sq_concat
	0, // sq_repeat
	(ssizeargfunc)example_class_sq_item, // sq_item
	0,
	(ssizeobjargproc)example_class_sq_setitem, // sq_ass_item
	0,
	(objobjproc)example_class_contains, // sq_contains
	0, // sq_inplace_concat
	0, // sq_inplace_repeat
};

#if PY3K
static PyNumberMethods example_classNumMethods = {
	(binaryfunc)example_class_add,
	(binaryfunc)example_class_sub,
	(binaryfunc)example_class_mul,
	(binaryfunc)example_class_mod, //nb_remainder
	(binaryfunc)example_class_divmod, //nb_divmod
	(ternaryfunc)example_class_pow, //nb_power
	(unaryfunc)example_class_neg, //nb_negative
	(unaryfunc)example_class_pos, //nb_positive
	(unaryfunc)example_class_abs, //nb_absolute
	0, //nb_bool
	0, //nb_invert
	0, //nb_lshift
	0, //nb_rshift
	0, //nb_and
	0, //nb_xor
	0, //nb_or
	0, //nb_int
	0, //nb_reserved
	0, //nb_float

	(binaryfunc)example_class_iadd, //nb_inplace_add
	(binaryfunc)example_class_isub, //nb_inplace_subtract
	(binaryfunc)example_class_imul, //nb_inplace_multiply
	(binaryfunc)example_class_imod, //nb_inplace_remainder
	(ternaryfunc)example_class_ipow, //nb_inplace_power
	0, //nb_inplace_lshift
	0, //nb_inplace_rshift
	0, //nb_inplace_and
	0, //nb_inplace_xor
	0, //nb_inplace_or

	(binaryfunc)example_class_floordiv, //nb_floor_divide
	(binaryfunc)example_class_div,
	(binaryfunc)example_class_ifloordiv, //nb_inplace_floor_divide
	(binaryfunc)example_class_idiv, //nb_inplace_true_divide

	0, //nb_index
};
#else
static PyNumberMethods example_classNumMethods = {
	(binaryfunc)example_class_add, //nb_add;
	(binaryfunc)example_class_sub, //nb_subtract;
	(binaryfunc)example_class_mul, //nb_multiply;
	(binaryfunc)example_class_div, //nb_divide;
	(binaryfunc)example_class_mod, //nb_remainder;
	(binaryfunc)example_class_divmod, //nb_divmod;
	(ternaryfunc)example_class_pow, //nb_power;
	(unaryfunc)example_class_neg, //nb_negative;
	(unaryfunc)example_class_pos, //nb_positive;
	(unaryfunc)example_class_abs, //nb_absolute;
	0, //nb_nonzero;       /* Used by PyObject_IsTrue */
	0, //nb_invert;
	0, //nb_lshift;
	0, //nb_rshift;
	0, //nb_and;
	0, //nb_xor;
	0, //nb_or;
	0, //nb_coerce;       /* Used by the coerce() function */
	0, //nb_int;
	0, //nb_long;
	0, //nb_float;
	0, //nb_oct;
	0, //nb_hex;

	   /* Added in release 2.0 */
	   (binaryfunc)example_class_iadd, //nb_inplace_add;
	   (binaryfunc)example_class_isub, //nb_inplace_subtract;
	   (binaryfunc)example_class_imul, //nb_inplace_multiply;
	   (binaryfunc)example_class_idiv, //nb_inplace_divide;
	   (binaryfunc)example_class_imod, //nb_inplace_remainder;
	   (ternaryfunc)example_class_ipow, //nb_inplace_power;
	   0, //nb_inplace_lshift;
	   0, //nb_inplace_rshift;
	   0, //nb_inplace_and;
	   0, //nb_inplace_xor;
	   0, //nb_inplace_or;

		  /* Added in release 2.2 */
		  (binaryfunc)example_class_floordiv, //nb_floor_divide;
		  (binaryfunc)example_class_div, //nb_true_divide;
		  (binaryfunc)example_class_ifloordiv, //nb_inplace_floor_divide;
		  (binaryfunc)example_class_idiv, //nb_inplace_true_divide;
};
#endif

static PyTypeObject example_classType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"template.example_class",             /* tp_name */
	sizeof(example_class),             /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)example_class_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	(reprfunc)example_class_str,                         /* tp_repr */
	&example_classNumMethods,             /* tp_as_number */
	&example_classSeqMethods,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	(reprfunc)example_class_str,                         /* tp_str */
	(getattrofunc)example_class_getattr,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |
	Py_TPFLAGS_BASETYPE |
	Py_TPFLAGS_CHECKTYPES,   /* tp_flags */
	"example_class( <example_class compatible type> )\nA simple example class holding a double value.",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	(richcmpfunc)example_class_richcompare,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	(getiterfunc)example_class_geniter,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	example_class_members,             /* tp_members */
	0,           			/* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)example_class_init,      /* tp_init */
	0,                         /* tp_alloc */
	(newfunc)example_class_new,                 /* tp_new */
};

static PyTypeObject example_classIterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"example_classIter",             /* tp_name */
	sizeof(example_classIter),             /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)example_classIter_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,             /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,   /* tp_flags */
	"example_class iterator",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	(iternextfunc)example_classIter_next,                         /* tp_iternext */
	0,             /* tp_methods */
	0,             /* tp_members */
	0,           			/* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,      /* tp_init */
	0,                         /* tp_alloc */
	(newfunc)example_classIter_new,                 /* tp_new */
};

static PyObject*
test(PyObject* o) {
	return o;
}

static PyMethodDef templatemethods[] = {
	{"test", (PyCFunction)test, METH_O, ""},
	{ NULL, NULL, 0, NULL }
};

#if PY3K
static PyModuleDef templatemodule = {
    PyModuleDef_HEAD_INIT,
    "template",
    "A simple template for Python's C-API",
    -1,
	templatemethods, NULL, NULL, NULL, NULL
};
#endif

PyMODINIT_FUNC
#if PY3K
PyInit_template(void)
#else
inittemplate(void)
#endif
{
	PyObject* mainmod = PyImport_AddModule("__main__");
	PyObject* maindict = PyModule_GetDict(mainmod);

	pi = PyFloat_AS_DOUBLE(PyObject_GetAttr(PyImport_ImportModuleEx("math", maindict, maindict, NULL), PyUnicode_FromString("pi")));
	
    PyObject* m;

	if (PyType_Ready(&example_classType) < 0 || PyType_Ready(&example_classIterType) < 0)
#if PY3K
		return NULL;
#else
		return;
#endif

#if PY3K
    m = PyModule_Create(&templatemodule);
#else
	m = Py_InitModule3("template", templatemethods, "Features that implement in Python the GLSL specification as closely as necessary.");
#endif
    if (m == NULL)
#if PY3K
		return NULL;
#else
		return;
#endif

	Py_INCREF(&example_classType);
	PyModule_AddObject(m, "example_class", (PyObject *)&example_classType);

#if PY3K
    return m;
#endif
}
