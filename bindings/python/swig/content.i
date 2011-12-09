
/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

%module Content
%include stl.i 
%include cmalloc.i
%include "carrays.i"
%include "typemaps.i" 
%{
#include <libsc/sc_content.h>
#include <stdio.h>
%}

%allocators(char);
%allocators(void);
%array_class(char, charArray);
%ignore Content::operator=;
%rename(convertToCont) Content::operator const Cont &() const;
%rename(convertToChar) Content::operator char*() const;
%rename(bindata) cdata;
//%apply (const char *STRING) { (char *ptr) };

%include libsc/sc_content.h

%include cdata.i
%cdata(char);



%inline{
	unsigned long castChar_p(char *address)
	{
		return (unsigned long)address;
	}
	
	void* castCharPtoVoidP(char *address)
	{
		return (void*)address;
	}

	bool saveContentToFile(const char* file_name, Content *content)
	{
		FILE *file = fopen(file_name, "wb");
		
		if (file)
		{
			Cont cont = Cont(*content);
			fwrite((void*)cont.d.ptr, cont.d.size, 1, file);
			fclose(file);
			return true;
		}
		
		return false;
	}
	
	
}

%include cstring.i
%cstring_chunk_output(char* data, int);


%extend Content {
 
	int get_size()
	{
		Cont cont = Cont(*$self);
		return cont.d.size;
	}

	%cstring_output_withsize(char *data, int *maxdata);
	void get_data(char *data, int *maxdata)
	{
		Cont cont = Cont(*$self);
		if (*maxdata >= cont.d.size)
			memcpy((void*)data, (void*)cont.d.ptr, cont.d.size);
		else
			memcpy((void*)data, (void*)cont.d.ptr, *maxdata);
	}
   
// typemap for an incoming buffer
%typemap(in) (char *INDATA, int SZ) {
   if (!PyString_Check($input)) {
       PyErr_SetString(PyExc_ValueError, "Expecting a string");
       return NULL;
   }
   $1 = (char *) PyString_AsString($input);
   $2 = PyString_Size($input);
} 

int StrToData(char* INDATA,int SZ) {
		
		//printf("data = %s , size = %d\n",INDATA,SZ);
		//Cont cont  = (Cont)(*$self);
		
		char* d = (char*)malloc(SZ);
		memcpy(d,INDATA,SZ);
		(*$self) = Content::data(SZ,d);
		free(d);
		return SZ;
  }




// typemap for an outgoing buffer
%typemap(in) (char *OUTDATA, int SZ) {
   if (!PyInt_Check($input)) {
       PyErr_SetString(PyExc_ValueError, "Expecting an integer");
       return NULL;
   }
   $2 = PyInt_AsLong($input);
   if ($2 < 0) {
       PyErr_SetString(PyExc_ValueError, "Positive integer expected");
       return NULL;
   }
   $1 = (char*) malloc($2);
}


// Return the buffer.  Discarding any previous return result
%typemap(argout) (char *OUTDATA, int SZ) {
   Py_XDECREF($result);   /* Blow away any previous result */
   if (result < 0) {      /* Check for I/O error */
       free($1);
       PyErr_SetFromErrno(PyExc_IOError);
       return NULL;
   }
   
   
   $result = PyString_FromStringAndSize($1,result);
   /*PyObject* res = PyTuple_New(2);
   PyTuple_SetItem(res, 0, $result);
   PyTuple_SetItem(res, 1, PyInt_FromLong(result));
   $result=res;*/
   
   free($1);
}  

int DataToStr(char *OUTDATA, int SZ) {
		Cont cont  = (Cont)(*$self);
		
		//OUTDATA = cont.d.ptr;
		OUTDATA = (char*)malloc(cont.d.size+1);
		memcpy(OUTDATA,(void*)cont.d.ptr,cont.d.size);
		SZ = cont.d.size;
		OUTDATA[cont.d.size]=0;
		//printf("data = %s , size = %d\n",OUTDATA,SZ);
		return SZ;
}  

}
