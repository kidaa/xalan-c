// Base header file.  Must be first.
#include <Include/PlatformDefinitions.hpp>



#include <cassert>
#include <fstream>
#include <iostream>
#include <strstream>



#include <util/PlatformUtils.hpp>



#include <XalanTransformer/XalanTransformer.hpp>



//This is here for the Windows threads.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#define THREADFUNCTIONRETURN DWORD WINAPI



#if !defined(XALAN_NO_NAMESPACES)
	using std::cerr;
	using std::cout;
	using std::endl;
	using std::ifstream;
	using std::ios_base;
	using std::ostrstream;
	using std::string;
#endif



// Used to hold compiled stylesheet and XML source document.
XalanCompiledStylesheet*	glbCompiledStylesheet = 0;
XalanParsedSource*			glbParsedSource = 0;
int							glbError = 0;	 

// Print messages tracking the progress of each thread, and the
// beginning and end of the entire operation.
void outputMessage(DWORD id, char msg[])
{
		ostrstream threadMsg;
	
		threadMsg << "\n" << msg << " Thread: " << id << '\0';
		cout << threadMsg.str();
		threadMsg.freeze(false);
}


THREADFUNCTIONRETURN theThread(LPVOID	param)
{
// This routine uses a compiled stylesheet (glbCompiledStylesheet), 
// and a binary source tree (glbParsedSource) to perform the 
// transformation.

	int	theResult = 0;

	const int	number = reinterpret_cast<int>(param);

	const DWORD		theThreadID = GetCurrentThreadId();

	outputMessage(theThreadID,"Starting ");

	// Create a XalanTransformer.
	XalanTransformer theXalanTransformer;

	// Generate the output file name for this thread.
    ostrstream theFormatterOut;
    theFormatterOut << "birds" << number << ".out" << '\0';

	//Generate the XML output object.
	XSLTResultTarget	theResultTarget(XalanDOMString(theFormatterOut.str()));

	// Unfreeze the ostrstream, so memory is returned...
	theFormatterOut.freeze(false);

	outputMessage(theThreadID,"Transforming");

 	// Do the transform.
	theResult = theXalanTransformer.transform(*glbParsedSource, glbCompiledStylesheet, theResultTarget);

	if(theResult != 0)
	{
		cerr << "ThreadSafe Error: \n" << theXalanTransformer.getLastError()
			 << endl
			 << endl;

		glbError = theResult;
	}

	outputMessage(theThreadID,"Finishing");

	return (theResult);
}

// Create and run the threads...
// Print messages tracking the progress of each thread and of the 
// overall operation...
void doThreads(int x)
{
	DWORD dwStackSize = 4096;              	// initial thread stack size
	LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)theThread;
	DWORD dwCreationFlags = 0;             	// creation flags
 	int nThreads = x;

	std::vector<HANDLE> hThreads;
	hThreads.reserve(nThreads);
 	int i = 0;	

	cout << endl << "Clock before starting threads: " << clock() << endl;

	for (i=0; i< nThreads; i++)
	{
		HANDLE hThread;
		DWORD  threadID;

		hThread = CreateThread(
				0, 
				dwStackSize,
				lpStartAddress,					// pointer to thread function
				reinterpret_cast<LPVOID>(i),	// argument for new thread
				dwCreationFlags,				// creation flags
				&threadID);

		assert(hThread != 0);

		hThreads.push_back(hThread);
	}

	WaitForMultipleObjects(hThreads.size(), &hThreads[0], TRUE, INFINITE);

	cout << endl << "Clock after threads: " << clock() << endl;

	for (i = 0; i < nThreads; ++i)
	{
		CloseHandle(hThreads[i]);
	}
}


int main(int argc, const char*	/* argv */[])
{
	if (argc != 1)
	{
		cerr << "Usage: ThreadTest"
			 << endl
			 << endl;
	}
	else
	{
		// Call the static initializer for Xerces.
		XMLPlatformUtils::Initialize();

		// Initialize Xalan.
		XalanTransformer::initialize();

		// Create a XalanTransformer.
		XalanTransformer theXalanTransformer;

		// Our input files...The assumption is that the executable will be run
		// from same directory as the input files.
		const char*		theXSLFileName = "birds.xsl";
		const char*		theXMLFileName = "birds.xml";
		
		glbCompiledStylesheet =	theXalanTransformer.compileStylesheet(theXSLFileName);

		assert(glbCompiledStylesheet != 0);

		// Compile the XML source document as well. All threads will use
		// this binary representation of the source tree.
		glbParsedSource =theXalanTransformer.parseSource(theXMLFileName);
		
		assert(glbParsedSource != 0);

		// Create and run the threads...
		// Each thread uses the same XalanNode and 
		// StylesheetRoot to perform a transformation.
		doThreads(10);

		// Terminate Xalan.
		XalanTransformer::terminate();

		// Call the static terminator for Xerces.
		XMLPlatformUtils::Terminate();
	}

	return glbError;
}
