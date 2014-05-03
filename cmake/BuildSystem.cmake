
# Meta build system for unity build and builds with shared sources between different executables.
#
# Add executables using add_executable_shared(EXE TYPE SRC LIBS EXTRA).
#
# Build the executables using either separate_build(), shared_build() or unity_build().
#

# create a unity build file for the executable UB_SUFFIX with the sources stored in the variable named by SOURCE_VARIABLE_NAME
# the name of the resulting unity build file will be stored in the variable named by SOURCE_VARIABLE_NAME
function(enable_unity_build UB_SUFFIX SOURCE_VARIABLE_NAME)
	set(files ${${SOURCE_VARIABLE_NAME}})
	
	# Sort the file list to ensure we get the same files order on all machines
	list(SORT files)
	
	# Generate a unique filename for the unity build translation unit
	set(unit_build_file ${CMAKE_CURRENT_BINARY_DIR}/ub_${UB_SUFFIX}.cpp)
	
	# Open the ub file
	file(WRITE ${unit_build_file} "// Unity build generated by CMake\n\n")
	
	set(compile_flags "")
	
	# Add include statement for each translation unit
	list(LENGTH files numfiles)
	set(currentIdx 1)
	foreach(source_file IN LISTS files)
		
		if("${source_file}" MATCHES ".rc$")
			
			# .rc files need to be compiled separately
			
		else()
			
			# Exclude all translation units from compilation
			set_source_files_properties("${source_file}" PROPERTIES HEADER_FILE_ONLY true)
			
			get_source_file_property(source_compile_flags "${source_file}" COMPILE_FLAGS)
			if(source_compile_flags)
				set(compile_flags "${compile_flags} ${source_compile_flags}")
			endif()
			
			get_filename_component(source_file "${source_file}" ABSOLUTE)
			
			if(MSVC OR ${CMAKE_CXX_COMPILER} MATCHES "(^|/)icp?c$")
				string(REGEX REPLACE ".*\\/" "" short_file "${source_file}")
				file(APPEND ${unit_build_file} "#pragma message"
				     " (\"[${currentIdx}/${numfiles}] Compiling ${short_file}...\")\n")
				math(EXPR currentIdx "${currentIdx} + 1")
			else()
				# While it's nice to see what actual source files are bing included, gcc
				# and some gcc-compatible compilers such as clang show a warning and/or
				# call stack for #pragma message.
				# This makes actual warnings much harder to notice, so we only enable
				# the message for whitelisted compilers.
			endif()
			
			file(APPEND ${unit_build_file} "#include \"${source_file}\"\n\n")
			
		endif()
		
	endforeach(source_file)
	
	# Complement list of translation units with the name of ub
	set(${SOURCE_VARIABLE_NAME} ${${SOURCE_VARIABLE_NAME}} ${unit_build_file} PARENT_SCOPE)
	
	set_source_files_properties("${unit_build_file}" PROPERTIES COMPILE_FLAGS "${compile_flags}")
	
	# Put ub file at the root of the project
	source_group("" FILES ${unit_build_file})
endfunction(enable_unity_build)


unset(SHARED_BUILD_BINARIES CACHE)

function(_add_binary_shared BIN TYPE SRC LIBS EXTRA INSTALL)
	list(REMOVE_DUPLICATES SRC)
	list(REMOVE_DUPLICATES LIBS)
	set(SHARED_BUILD_${BIN}_TYPE "${TYPE}" CACHE INTERNAL "")
	set(SHARED_BUILD_${BIN}_SOURCES "${SRC}" CACHE INTERNAL "")
	set(SHARED_BUILD_${BIN}_LIBS "${LIBS}" CACHE INTERNAL "")
	set(SHARED_BUILD_${BIN}_EXTRA "${EXTRA}" CACHE INTERNAL "")
	set(SHARED_BUILD_${BIN}_INSTALL "${INSTALL}" CACHE INTERNAL "")
	set(SHARED_BUILD_BINARIES ${SHARED_BUILD_BINARIES} ${BIN} CACHE INTERNAL "")
endfunction(_add_binary_shared)

# Add an executable to be build by either separate_build(), shared_build() or unity_build()
#  EXE   Name of the executable to add.
#  TYPE	 Type of exe (ex: WIN32)
#  SRC   The executable's source files.
#  LIBS  Libraries to link the executable against.
#  EXTRA Additional arguments to pass to add_executable() but not shared with
#        other binaries or included in unity builds.
#  INSTALL Install directory. Default: "${CMAKE_INSTALL_BINDIR}"
function(add_executable_shared EXE TYPE SRC LIBS)
	if(ARGC GREATER 4)
		set(extra "${ARGV4}")
	else()
		set(extra "")
	endif()
	if(ARGC GREATER 5)
		set(installdir ${ARGV5})
	elseif(DEFINED CMAKE_INSTALL_BINDIR)
		set(installdir "${CMAKE_INSTALL_BINDIR}")
	else()
		set(installdir bin)
	endif()
	set(install RUNTIME DESTINATION "${installdir}")
	_add_binary_shared("${EXE}" "${TYPE}" "${SRC}" "${LIBS}" "${extra}" "${install}")
endfunction()

# Add a library to be build by either separate_build(), shared_build() or unity_build()
#  LIB   Name of the library to add.
#  SRC   The librarie's source files.
#  LIBS  Libraries to link the executable against.
#  EXTRA Additional arguments to pass to add_library() but not shared with
#        orther binaries or included in unity builds.
#  INSTALL Install directory. Default: "${CMAKE_INSTALL_LIBDIR}"
function(add_library_shared LIB SRC LIBS)
	if(ARGC GREATER 4)
		set(extra "${ARGV4}")
	else()
		set(extra "")
	endif()
	if(ARGC GREATER 5)
		set(installdir ${ARGV5})
	elseif(DEFINED CMAKE_INSTALL_LIBDIR)
		set(installdir "${CMAKE_INSTALL_LIBDIR}")
	else()
		set(installdir lib)
	endif()
	set(install LIBRARY DESTINATION "${installdir}" ARCHIVE DESTINATION "${installdir}")
	_add_binary_shared("${LIB}" SHARED "${SRC}" "${LIBS}" "${extra}" "${install}")
endfunction()


# Calculate the intersection of the lists SRC1 and SRC2 and store the result in the variable named by DEST.
function(intersect DEST SRC1 SRC2)
	
	set(dest "")
	
	foreach(src IN LISTS SRC1)
		list(FIND SRC2 "${src}" found)
		if(NOT found EQUAL -1)
			list(APPEND dest "${src}")
		endif()
	endforeach()
	
	set(${DEST} "${dest}" PARENT_SCOPE)
	
endfunction(intersect)


function(_shared_build_helper LIB LIST BINARIES FIRST)
	
	set(list ${LIST})
	set(first ${FIRST})
	
	# Find common sources and extract static libraries.
	foreach(exe IN LISTS LIST)
		
		list(REMOVE_ITEM list ${exe})
		
		set(exeset ${BINARIES} ${exe})
		
		_shared_build_helper(${LIB}_${exe} "${list}" "${exeset}" ${first})
		
		set(first 0)
		
	endforeach(exe)
	
	# Find sources common to all executables in the current set.
	set(first 1)
	foreach(exe IN LISTS BINARIES)
		if(first)
			set(first 0)
			set(common_src "${SHARED_BUILD_${exe}_SOURCES}")
		else()
			intersect(common_src "${common_src}" "${SHARED_BUILD_${exe}_SOURCES}")
		endif()
	endforeach(exe)
	
	# We found common sources!
	if(NOT "${common_src}" STREQUAL "")
		
		list(LENGTH LIST all_execs)
		if(FIRST AND all_execs EQUAL 0)
			set(lib common)
		else()
			set(lib _${LIB}_common)
		endif()
		
		# Add a new library for the common sources
		add_library(${lib} STATIC ${common_src})
		
		set(is_shared_lib 0)
		
		# Remove sources from executables and link the library instead.
		foreach(exe IN LISTS BINARIES)
			
			if("${SHARED_BUILD_${exe}_TYPE}" STREQUAL "SHARED")
				set(is_shared_lib 1)
			endif()
			
			set(uncommon_src "${SHARED_BUILD_${exe}_SOURCES}")
			foreach(src IN LISTS common_src)
				list(REMOVE_ITEM uncommon_src ${src})
			endforeach(src)
			set(SHARED_BUILD_${exe}_SOURCES "${uncommon_src}" CACHE INTERNAL "")
			
			set(SHARED_BUILD_${exe}_LIBS ${lib} "${SHARED_BUILD_${exe}_LIBS}" CACHE INTERNAL "")
			
		endforeach(exe)
		
		if(is_shared_lib)
			if(CMAKE_VERSION LESS 2.8.9)
				set(pic_flags "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
				set_target_properties(${lib} PROPERTIES COMPILE_FLAGS "${pic_flags}")
			else()
				set_target_properties(${lib} PROPERTIES POSITION_INDEPENDENT_CODE ON)
			endif()
		endif()
		
	endif()
	
endfunction(_shared_build_helper)


function(_shared_build_cleanup)
	
	foreach(exe IN LISTS SHARED_BUILD_BINARIES)
		unset(SHARED_BUILD_${exe}_TYPE CACHE)
		unset(SHARED_BUILD_${exe}_SOURCES CACHE)
		unset(SHARED_BUILD_${exe}_LIBS CACHE)
		unset(SHARED_BUILD_${exe}_EXTRA CACHE)
		unset(SHARED_BUILD_${exe}_INSTALL CACHE)
	endforeach(exe)
	
	unset(SHARED_BUILD_BINARIES CACHE)
	
endfunction(_shared_build_cleanup)

function(_shared_build_add_executable exe)
	if("${SHARED_BUILD_${exe}_TYPE}" STREQUAL "SHARED")
		add_library(
			${exe} ${SHARED_BUILD_${exe}_TYPE}
			${SHARED_BUILD_${exe}_SOURCES}
			${SHARED_BUILD_${exe}_EXTRA}
		)
	else()
		add_executable(
			${exe} ${SHARED_BUILD_${exe}_TYPE}
			${SHARED_BUILD_${exe}_SOURCES}
			${SHARED_BUILD_${exe}_EXTRA}
		)
	endif()
	target_link_libraries(${exe} ${SHARED_BUILD_${exe}_LIBS})
	install(TARGETS ${exe} ${SHARED_BUILD_${exe}_INSTALL})
endfunction(_shared_build_add_executable)

# Build each executable separately.
function(separate_build)
	
	foreach(exe IN LISTS SHARED_BUILD_BINARIES)
		_shared_build_add_executable(${exe})
	endforeach(exe)
	
	_shared_build_cleanup()
	
endfunction(separate_build)


# Build each source file separately and extract common source files into static libraries.
function(shared_build)
	
	set(list1 ${SHARED_BUILD_BINARIES})
	
	set(first 1)
	
	# Find common sources and extract static libraries.
	foreach(exe1 IN LISTS SHARED_BUILD_BINARIES)
		list(REMOVE_ITEM list1 ${exe1})
		set(list2 ${list1})
		# Require two source sets before calling _shared_build_helper so we don't create static libraries for individual executables!
		foreach(exe2 IN LISTS list1)
			list(REMOVE_ITEM list2 ${exe2})
			set(exeset ${exe1} ${exe2})
			_shared_build_helper(${exe1}_${exe2} "${list2}" "${exeset}" ${first})
			set(first 0)
		endforeach(exe2)
	endforeach(exe1)
	
	separate_build()
	
endfunction(shared_build)


# Build each executable by including all the source files into one big master file.
function(unity_build)
	
	add_custom_target(ub_notice COMMENT "Note: The unity build executables may take a long time to compile, without any indication of progress. Be patient.")
	
	foreach(exe IN LISTS SHARED_BUILD_BINARIES)
		enable_unity_build(${exe} SHARED_BUILD_${exe}_SOURCES)
		_shared_build_add_executable(${exe})
		add_dependencies(${exe} ub_notice)
	endforeach(exe)
	
	_shared_build_cleanup()
	
endfunction(unity_build)
