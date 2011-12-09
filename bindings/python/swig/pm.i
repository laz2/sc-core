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

%module(directors="1") pm_

%include windows.i
%include stl.i

%{
#include "libsc/sc_atoms.h"
#include "libpm/pm.h"
#include "libpm/pm_sched.h"
#include "libpm/sc_operation.h"
#include "libpm/scp/scp_core.h"
#include "libpm/scp/scp_package.h"
#include "libpm/scp/scp_debugger.h"

#define SWIG_PYTHON_THREADS	1
%}

%include libsc/sc_atoms.h

%import libpm/config.h
%include libpm/pm.h
%include libpm/pm_sched.h

%feature("director") ScOperation;
%feature("director") ScOperationActSetMember;
%feature("director") ScOperationActInSet;
%feature("director") ScOperationElChange;
%feature("director") ScOperationCont;
%feature("director") ScOperationIdtf;
%feature("director") ScOperationDie;
%feature("director") ScOperationChangeType;
%include libpm/sc_operation.h

%include libsc/advanced_activity.h
%include libpm/scp/scp_core.h
%include libpm/scp/scp_package.h
