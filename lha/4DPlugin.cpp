/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : lha
 #	author : miyako
 #	2018/03/27
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
// --- lha

		case 1 :
			LHA(pResult, pParams);
			break;

	}
}

#pragma mark -

// -------------------------------------- lha -------------------------------------

void copyPath(char *filename, CUTF16String *path)
{
	/* convert to HFS */
	C_TEXT t;
#if VERSIONMAC
	t.setUTF8String((const uint8_t *)filename, (uint32_t)strlen(filename));
	NSString *s = t.copyUTF16String();
	NSURL *u = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)s, kCFURLPOSIXPathStyle, false);
	if(u)
	{
		NSString *str = (NSString *)CFURLCopyFileSystemPath((CFURLRef)u, kCFURLHFSPathStyle);
		t.setUTF16String(str);
		t.copyUTF16String(path);
		[str release];
		[u release];
	}
	[s release];
#else
	int error = 0;
	int size = strlen(filename);
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)filename, size, NULL, 0);
	if (len)
	{
		std::vector<char> buf((len + 1) * sizeof(wchar_t));
		if (MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)filename, size, (LPWSTR)&buf[0], len)) {
			t.setUTF16String((const PA_Unichar *)&buf[0], len);
			t.copyUTF16String(path);
		}
	}
#endif
}

void PA_YieldAbsolute2()
{
	PA_Variable	params;
	PA_ExecuteCommandByID(311, &params, 0);/* IDLE */
}

void _cb2(char *filename, void *array)
{
	JSONNODE *arr = (JSONNODE *)array;
	
	CUTF16String path;
	copyPath(filename, &path);
	
	json_push_back_s(arr, &path);
}

typedef enum
{
	lha_error_none = 0,
	lha_error_extract_archive = 1,
	lha_error_fopen = 2,
	lha_error_input_stream_from_file = 3,
	lha_error_reader_new = 4
}lha_error_t;

void LHA(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT Param3i;
	C_TEXT Param3o;
	C_LONGINT returnValue;

	JSONNODE *arr = json_new(JSON_ARRAY);
	
	lha_error_t lha_error = lha_error_none;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3i.fromParamAtIndex(pParams, 3);
	
	CUTF8String src;
	Param1.copyPath(&src);
	
	CUTF8String dst;
	Param2.copyPath(&dst);
	
	LHAOptions options;
	options.overwrite_policy = LHA_OVERWRITE_ALL;
	options.quiet = 2;
	options.verbose = 0;
	options.dry_run = 0;
	options.extract_path = (char *)dst.c_str();
	options.use_path = 1;
	
#if VERSIONWIN
	FILE *fstream = NULL;
	wchar_t	buf[_MAX_PATH];
	if (MultiByteToWideChar(CP_UTF8, 0, (const char *)src.c_str(), -1, (LPWSTR)buf, _MAX_PATH))
		fstream = _wfopen((const wchar_t *)buf, L"rb");
#else
	FILE *fstream = fopen((char *)src.c_str(), "rb");
#endif

	if(fstream)
	{
		LHAInputStream *stream = lha_input_stream_from_FILE(fstream);
		if(stream)
		{
			LHAReader *reader = lha_reader_new(stream);
			if(reader)
			{
				LHAFilter filter;
				filter.num_filters = 0;
				filter.reader = reader;
				filter.filters = NULL;
				
				std::vector<CUTF8String>_filters;
				std::vector<char *>filters;
				unsigned int num_filters = 0;
				
				JSONNODE *params = json_parse(Param3i);
				
				if(params)
				{
					if (json_type(params) == JSON_ARRAY)
					{
						JSONNODE_ITERATOR i = json_begin(params);
						while (i != json_end(params))
						{
							json_char *str = json_as_string(*i);
							if(str)
							{
								CUTF8String u8;
								json_wconv(str, &u8);
								_filters.push_back(u8);
								filters.push_back((char *)_filters.back().c_str());
								json_free(str);
							}
							
							++i;num_filters++;
						}
						if(num_filters)
						{
							lha_filter_init(&filter, reader, &filters[0], num_filters);
						}
	
					}
					json_delete(params);
				}
				
				void (*_PA_YieldAbsolute)(void) = PA_YieldAbsolute2;
				
				if(!extract_archive(&filter, &options, _PA_YieldAbsolute, _cb2, arr))
				{
					lha_error = lha_error_extract_archive;
				}
				
				lha_reader_free(reader);
			}else{lha_error = lha_error_reader_new;}
			lha_input_stream_free(stream);
		}else{lha_error = lha_error_input_stream_from_file;}
		fclose(fstream);
	}else{lha_error = lha_error_fopen;};
	
	json_stringify(arr, Param3o);
	json_delete(arr);
	
	Param3o.toParamAtIndex(pParams, 3);
	
	returnValue.setIntValue(lha_error);
	returnValue.setReturn(pResult);
}

