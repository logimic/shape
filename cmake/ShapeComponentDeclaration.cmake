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

function(DeployShapeComponent componentName)
	INSTALL(TARGETS ${componentName}
		RUNTIME DESTINATION "\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_INSTALL_PREFIX}/bin"
		LIBRARY DESTINATION "\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_INSTALL_PREFIX}/bin"
		ARCHIVE DESTINATION "\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_INSTALL_PREFIX}/lib"
	)

	string(REPLACE ":" "_" NAMESPACE_COMPONENT ${componentName})
	set(schemafile "${CMAKE_CURRENT_SOURCE_DIR}/schema__${NAMESPACE_COMPONENT}.json")
	if(EXISTS "${schemafile}")
		install(
			FILE
			"${schemafile}"
			DESTINATION "\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_INSTALL_PREFIX}/bin"
		)
		endif()
endfunction()

function(DeployShapeConfiguration cfgName)
	set(cfgPath "\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_INSTALL_PREFIX}/runcfg/${cfgName}")
	
	set(ix 1)
	while(ix LESS ${ARGC})
		list(GET ARGV ${ix} src)
		MATH(EXPR ix "${ix}+1")	
		if(NOT ix LESS ${ARGC})
			break()
		endif()

		list(GET ARGV ${ix} dst)
		set(dst "${cfgPath}/${dst}")
		MATH(EXPR ix "${ix}+1")	

		install(
			DIRECTORY "${src}/"
			DESTINATION ${dst}
		)
	endwhile()

endfunction()

function(DeployShapeConfigurationStartUp executable cfgName)
	set(cfgPath "\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_INSTALL_PREFIX}/runcfg/${cfgName}")
	
	set(_EXE "${shape_DEPLOY}/Debug/${executable}")		
	set(_ARGS "./configuration/config.json")
	if(UNIX)
	  set(_SUFFIX sh)
	else()
	  set(_SUFFIX bat)
	endif()
	set(_TMP_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp/runcfg/${cfgName}/StartUp.tmp)
	
	configure_file(${shape_CMAKE_MODULE_PATH}/StartUp.in ${_TMP_FILE} @ONLY)
	install(
		PROGRAMS "${_TMP_FILE}"
		DESTINATION ${cfgPath}
		RENAME StartUp.${_SUFFIX}
	)
endfunction()

# This is temporary. MSVC cmake support shall be used in next MSVC ver.
# The file <Project>.vcxpro.user is configured only if it doesn't exist - it keeps possible manual changes
function(ConfigureMsvcProject executable cfgName)
    if(MSVC)
		set(_vcxProjUserName "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user")
        if (NOT EXISTS ${_vcxProjUserName})
			if(${MSVC_VERSION} EQUAL 1700)
				set(VS_VER 11.0)
			elseif(${MSVC_VERSION} EQUAL 1800)
				set(VS_VER 12.0)
			elseif(${MSVC_VERSION} EQUAL 1900)
				set(VS_VER 14.0)
			elseif(${MSVC_VERSION} EQUAL 1915)
				set(VS_VER 15.0)
			elseif(${MSVC_VERSION} EQUAL 1916)
				set(VS_VER 15.0)
			else()
				message(FATAL_ERROR "Unknown/unsupported VS version: ${MSVC_VERSION}")
			endif()

			set(DEBUG_EXE "${shape_DEPLOY}/Debug/${executable}")		
			set(RELEASE_EXE "${shape_DEPLOY}/Release/${executable}")		
			set(DEBUG_ARGS "./configuration/config.json")		
			set(RELEASE_ARGS "./configuration/config.json")		
			set(DEBUG_WD "${shape_DEPLOY}/Debug/${PROJECT_INSTALL_PREFIX}/runcfg/${cfgName}")		
			set(RELEASE_WD "${shape_DEPLOY}/Release/${PROJECT_INSTALL_PREFIX}/runcfg/${cfgName}")		

			configure_file(${shape_CMAKE_MODULE_PATH}/project.vcxproj.user.in ${_vcxProjUserName} @ONLY)
        endif()
    endif()

endfunction()
