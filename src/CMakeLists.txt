set(GEN_RPSL ${CMAKE_CURRENT_BINARY_DIR}/gen/rpsl)
set(GEN_RTCONFIG ${CMAKE_CURRENT_BINARY_DIR}/gen/rtconfig)
file(MAKE_DIRECTORY ${GEN_RPSL} ${GEN_RTCONFIG})

# The grammars name their own outputs, so run the tools in the output directory.
function(irr_bison dir input)
  get_filename_component(name ${input} NAME)
  add_custom_command(
    OUTPUT ${dir}/${name}.cc ${dir}/${name}.hh
    COMMAND ${BISON_EXECUTABLE} -d ${CMAKE_CURRENT_SOURCE_DIR}/${input}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${input}
    WORKING_DIRECTORY ${dir}
    VERBATIM)
endfunction()

function(irr_flex dir input)
  get_filename_component(name ${input} NAME)
  add_custom_command(
    OUTPUT ${dir}/${name}.cc
    COMMAND ${FLEX_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${input}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${input} ${ARGN}
    WORKING_DIRECTORY ${dir}
    VERBATIM)
endfunction()

irr_bison(${GEN_RPSL} rpsl/rpsl.y)
irr_flex(${GEN_RPSL} rpsl/rpsl.l ${GEN_RPSL}/rpsl.y.hh)
irr_flex(${GEN_RPSL} rpsl/object_log.l)
irr_bison(${GEN_RTCONFIG} rtconfig/command.y)
irr_flex(${GEN_RTCONFIG} rtconfig/command.l ${GEN_RTCONFIG}/command.y.hh)

add_library(irrtoolset STATIC
  dataset/CacheKey.CacheValue.AVLMap.cc
  dataset/CacheKey.CacheValue.Map.cc
  dataset/NormalTermPtr.DLList.cc
  dataset/PrefixRange.Plex.cc
  dataset/PrefixRange.XPlex.cc
  dataset/SymID.Set.cc
  dataset/SymID.VHSet.cc
  dataset/SymbolConjunctPtr.Bag.cc
  dataset/SymbolConjunctPtr.SplayBag.cc
  dataset/SymbolConjunctPtr.SplayNode.cc
  dataset/unsigned.OXPSet.cc
  dataset/unsigned.Plex.cc
  dataset/unsigned.Set.cc
  dataset/unsigned.XPlex.cc

  irr/autnum.cc
  irr/birdwhoisc.cc
  irr/cache_value.cc
  irr/irr.cc
  irr/rawhoisc.cc
  irr/route.cc

  irrutil/Argv.cc
  irrutil/debug.cc
  irrutil/errhandler.cc
  irrutil/net.cc
  irrutil/rusage.cc
  irrutil/strcase.c
  irrutil/trace.cc
  irrutil/version.cc

  normalform/Filter.cc
  normalform/FilterOfASPath.cc
  normalform/FilterOfCommunity.cc
  normalform/FilterOfPrefix.cc
  normalform/FilterOfSymbol.cc
  normalform/IPv6RadixSet.cc
  normalform/NE.cc
  normalform/NT.cc
  normalform/RadixSet.cc
  normalform/SetOfIPv6Prefix.cc
  normalform/SetOfPrefix.cc
  normalform/SetOfSymbol.cc
  normalform/SymbolConjunct.cc
  normalform/community.cc

  re2dfa/re2dfa.c
  re2dfa/regexp_nf.cc

  rpsl/Buffer.cc
  rpsl/afi.cc
  rpsl/object.cc
  rpsl/prefix.cc
  rpsl/rangelist.cc
  rpsl/regexp.cc
  rpsl/rpsl_attr.cc
  rpsl/rpsl_filter.cc
  rpsl/rpsl_item.cc
  rpsl/rpsl_policy.cc
  rpsl/rptype.cc
  rpsl/schema.cc
  rpsl/schema_rpsl.cc
  rpsl/symbols.cc
  ${GEN_RPSL}/object_log.l.cc
  ${GEN_RPSL}/rpsl.l.cc
  ${GEN_RPSL}/rpsl.y.cc)

# Headers are included relative to src/; the parsers also need their own directories.
target_include_directories(irrtoolset PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${GEN_RPSL}
  ${CMAKE_CURRENT_SOURCE_DIR}/rpsl)

if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  target_link_libraries(irrtoolset PUBLIC socket nsl)
endif()

add_executable(peval peval/peval.cc)
add_executable(rpslcheck rpslcheck/rpslcheck.cc)
add_executable(rtconfig
  rtconfig/rtconfig.cc
  rtconfig/f_cisco.cc
  rtconfig/f_ciscoxr.cc
  rtconfig/f_junos.cc
  ${GEN_RTCONFIG}/command.l.cc
  ${GEN_RTCONFIG}/command.y.cc)
target_include_directories(rtconfig PRIVATE ${GEN_RTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/rtconfig)

foreach(tool peval rpslcheck rtconfig)
  target_link_libraries(${tool} PRIVATE irrtoolset)
endforeach()

if(HAVE_LIBREADLINE)
  target_include_directories(irrtoolset PUBLIC ${READLINE_INCLUDE_DIR})
  target_link_libraries(peval PRIVATE ${READLINE_LIBRARIES})
  target_link_libraries(rtconfig PRIVATE ${READLINE_LIBRARIES})
endif()

install(TARGETS peval rpslcheck rtconfig RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES peval/peval.1 rpslcheck/rpslcheck.1 rtconfig/rtconfig.1
              rpsl/irrtoolset-errors.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
