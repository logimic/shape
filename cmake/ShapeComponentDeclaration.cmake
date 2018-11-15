function(DeclareShapeComponent componentName)
	message(STATUS "DeclareShapeComponent(): ${componentName}")
	string(REPLACE ":" "_" NAMESPACE_COMPONENT ${componentName})
    set(SHAPE_COMPONENT_${NAMESPACE_COMPONENT} ${componentName} PARENT_SCOPE)
endfunction()

function(AddShapeProvidedInterface componentName interface)
	string(REPLACE ":" "_" NAMESPACE_COMPONENT ${componentName})
	if(${SHAPE_COMPONENT_${NAMESPACE_COMPONENT}} STREQUAL ${componentName})
		set(IFACE
			"    component.provideInterface<${interface}>(\"${interface}\");"
		)
		set(SHAPE_COMPONENT_PROVIDED_IFACES_${NAMESPACE_COMPONENT} ${SHAPE_COMPONENT_PROVIDED_IFACES_${NAMESPACE_COMPONENT}} "${IFACE}" PARENT_SCOPE)
	else()
		message( SEND_ERROR "AddShapeProvidedInterface(): Component ${componentName} must be declared in advance by DeclareShapeComponent()")
	endif()
endfunction()

function(AddShapeRequiredInterface componentName interface optionality cardinality)
	string(REPLACE ":" "_" NAMESPACE_COMPONENT ${componentName})
	if(${SHAPE_COMPONENT_${NAMESPACE_COMPONENT}} STREQUAL ${componentName})
		if(NOT ((${optionality} MATCHES MANDATORY) OR (${optionality} MATCHES UNREQUIRED)))
			message( SEND_ERROR "optionality must be UNREQUIRED|MANDATORY" )
		endif()
		if(NOT ((${cardinality} MATCHES SINGLE) OR (${cardinality} MATCHES MULTIPLE)))
			message( SEND_ERROR "optionality must be UNREQUIRED|MANDATORY" )
		endif()
		set(IFACE
			"    component.requireInterface<${interface}>(\"${interface}\", shape::Optionality::${optionality}, shape::Cardinality::${cardinality});"
		)
		set(SHAPE_COMPONENT_REQUIRED_IFACES_${NAMESPACE_COMPONENT} ${SHAPE_COMPONENT_REQUIRED_IFACES_${NAMESPACE_COMPONENT}} "${IFACE}" PARENT_SCOPE)
	else()
		message( SEND_ERROR "AddShapeRequiredInterface(): Component ${componentName} must be declared in advance by DeclareShapeComponent()")
	endif()
endfunction()

function(ConfigureShapeComponent componentName componentHeader)
	string(REPLACE ":" "_" NAMESPACE_COMPONENT ${componentName})
	if(${SHAPE_COMPONENT_${NAMESPACE_COMPONENT}} STREQUAL ${componentName})
		string(REPLACE ";" ";\n" PROVIDED_INTERFACES "${SHAPE_COMPONENT_PROVIDED_IFACES_${NAMESPACE_COMPONENT}}")
		string(REPLACE ";" ";\n" REQUIRED_INTERFACES "${SHAPE_COMPONENT_REQUIRED_IFACES_${NAMESPACE_COMPONENT}}")
		set(TARGET_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAMESPACE_COMPONENT}.hxx")
		configure_file(${shape_CMAKE_MODULE_PATH}/Component.hxx.in ${TARGET_FILE} @ONLY)
		set(${componentHeader} ${TARGET_FILE} PARENT_SCOPE)
		message(STATUS "Component ${componentName} configured to ${TARGET_FILE}")
	else()
		message( SEND_ERROR "ConfigureShapeComponent(): Component ${componentName} must be declared in advance by DeclareShapeComponent()")
	endif()
endfunction()

function(DeployShapeTarget subdir projectName)
	set(targetdir "${shape_DEPLOY}/$<CONFIGURATION>/${subdir}")
	set(subdirpath ${targetdir} PARENT_SCOPE)
	set(targetfile "$<TARGET_FILE:${PROJECT_NAME}>")
	string(CONCAT comment "Copy: " "${targetfile}" " to " "${targetdir}")
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${targetdir}" 
		COMMAND "${CMAKE_COMMAND}" -E copy "${targetfile}" "${targetdir}"
		COMMAND "${CMAKE_COMMAND}" -E echo "${comment}"
	)
endfunction()

function(DeployShapeComponent subdir projectName componentName)
	DeployShapeTarget(${subdir} ${projectName})
	string(REPLACE ":" "_" NAMESPACE_COMPONENT ${componentName})
	set(schemafile "${CMAKE_CURRENT_SOURCE_DIR}/schema__${NAMESPACE_COMPONENT}.json")
    if(EXISTS "${schemafile}")
		string(CONCAT comment "Copy: " "${schemafile}" " to " "${subdirpath}")
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy "${schemafile}" "${subdirpath}"
			COMMAND "${CMAKE_COMMAND}" -E echo "${comment}"
		)
    endif()
endfunction()

function(DeployShapeConfiguration subdir cfgName)
	set(cfgPath "${shape_DEPLOY}/$<CONFIGURATION>/${subdir}/${cfgName}")
	
	set(ix 2)
	while(ix LESS ${ARGC})
		list(GET ARGV ${ix} src)
		MATH(EXPR ix "${ix}+1")	
		if(NOT ix LESS ${ARGC})
			break()
		endif()

		list(GET ARGV ${ix} dst)
		set(dst "${cfgPath}/${dst}")
		MATH(EXPR ix "${ix}+1")	

		string(CONCAT comment "Copy: " "${src}" " to " "${dst}")
		
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy_directory "${src}" "${dst}"
			COMMAND "${CMAKE_COMMAND}" -E echo "${comment}"
		)

	endwhile()
	
endfunction()
