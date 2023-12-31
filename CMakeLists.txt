cmake_minimum_required (VERSION 3.8)

set (PROJECT_NAME "OccDemo")
project (${PROJECT_NAME})

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

function(add_source var_list global_list)
	get_property(tmp GLOBAL PROPERTY ${global_list})
	foreach(arg ${var_list})
		list(APPEND tmp ${arg})
		#message("arg ${arg}")
	endforeach() 
	set_property(GLOBAL PROPERTY ${global_list} "${tmp}")
endfunction(add_source)

function(dirlist alllist curdir group)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	source_group(${group})
	set(curFiles "")
	foreach(child ${children})
		set (childPath ${curdir}/${child})
		if(IS_DIRECTORY ${childPath})
			set(childGroup ${group}\\\\${child})
			dirlist(${alllist} ${childPath} ${childGroup})
		else()
			add_source(${childPath} ${alllist})
			LIST(APPEND curFiles ${childPath})
		endif()
	endforeach()
	source_group(${group} FILES ${curFiles})
endfunction()

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(OCC_LIB FoundationClasses ModelingData ModelingAlgorithms)
add_subdirectory(${PROJECT_SOURCE_DIR}/WinBind)
add_subdirectory(${PROJECT_SOURCE_DIR}/OsgViewDemo)

