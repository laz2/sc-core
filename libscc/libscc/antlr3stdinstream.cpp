/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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

#include "precompiled_p.h"

ANTLR3_UINT32 antlr3readAsciiStdin(pANTLR3_INPUT_STREAM input)
{
	ANTLR3_UINT32 bufSize = 8192;

	input->data = ANTLR3_MALLOC(static_cast<size_t>(bufSize));
	if (input->data == NULL)
		return ANTLR3_ERR_NOMEM;

	ANTLR3_UINT32 realSize = 0;
	pANTLR3_INT8  curData  = static_cast<pANTLR3_INT8>(input->data);

	while (!feof(stdin)) {
		size_t readed = fread(curData, 1, bufSize - realSize, stdin);

		curData  += readed;
		realSize += readed;

		if (realSize == bufSize) {
			bufSize *= 2;
			input->data = ANTLR3_REALLOC(input->data, static_cast<size_t>(bufSize));

			curData = static_cast<pANTLR3_INT8>(input->data) + realSize;
		}
	}

	input->sizeBuf     = realSize;
	input->isAllocated = ANTLR3_TRUE;

	antlr3Fclose(stdin);

	return ANTLR3_SUCCESS;
}

/// @brief Use the contents of an operating system stdin as the input
///        for an input stream.
/// @return Pointer to new input stream context upon success
///         One of the ANTLR3_ERR_ defines on error.
pANTLR3_INPUT_STREAM antlr3AsciiStdinStreamNew()
{
	// Pointer to the input stream we are going to create
	//
	pANTLR3_INPUT_STREAM input;
	ANTLR3_UINT32        status;

	// Allocate memory for the input stream structure
	//
	input = (pANTLR3_INPUT_STREAM) ANTLR3_CALLOC(1, sizeof(ANTLR3_INPUT_STREAM));

	if (input == NULL)
		return NULL;

	// Structure was allocated correctly, now we can read all stdin.
	//
	status = antlr3readAsciiStdin(input);

	// Call the common 8 bit ASCII input stream handler
	// Initializer type thingy doobry function.
	//
	antlr3AsciiSetupStream(input, ANTLR3_CHARSTREAM);

	// Now we can set up the file name
	//
	input->istream->streamName = input->strFactory->newStr(input->strFactory, (pANTLR3_UINT8)"(stdin)");
	input->fileName            = input->istream->streamName;

	if (status != ANTLR3_SUCCESS) {
		input->close(input);
		return NULL;
	}

	return input;
}
