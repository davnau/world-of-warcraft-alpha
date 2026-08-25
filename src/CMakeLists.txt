cmake_minimum_required(VERSION 3.25)

project(WoWClient2 VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)

if(NOT WIN32)
    message(FATAL_ERROR "WoWClient2 currently targets the preserved Win32 client environment.")
endif()

set(WOW_INSTALL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(WOW_RUNTIME_WORKING_DIRECTORY "${WOW_INSTALL_ROOT}/out" CACHE PATH
    "Working directory used when debugging WoWClient2")

file(MAKE_DIRECTORY "${WOW_RUNTIME_WORKING_DIRECTORY}")

function(add_recovered_headers target root)
    file(GLOB_RECURSE recovered_headers CONFIGURE_DEPENDS
        "${root}/*.h" "${root}/*.hpp")
    target_sources(${target} PRIVATE ${recovered_headers})
endfunction()

add_library(Lua50 STATIC
    ThirdParty/Lua50/COPYRIGHT
    ThirdParty/Lua50/include/lua.h
    ThirdParty/Lua50/include/lauxlib.h
    ThirdParty/Lua50/include/lualib.h
    ThirdParty/Lua50/src/lapi.c
    ThirdParty/Lua50/src/lcode.c
    ThirdParty/Lua50/src/ldebug.c
    ThirdParty/Lua50/src/ldo.c
    ThirdParty/Lua50/src/ldump.c
    ThirdParty/Lua50/src/lfunc.c
    ThirdParty/Lua50/src/lgc.c
    ThirdParty/Lua50/src/llex.c
    ThirdParty/Lua50/src/lmem.c
    ThirdParty/Lua50/src/lobject.c
    ThirdParty/Lua50/src/lopcodes.c
    ThirdParty/Lua50/src/lparser.c
    ThirdParty/Lua50/src/lstate.c
    ThirdParty/Lua50/src/lstring.c
    ThirdParty/Lua50/src/ltable.c
    ThirdParty/Lua50/src/ltm.c
    ThirdParty/Lua50/src/lundump.c
    ThirdParty/Lua50/src/lvm.c
    ThirdParty/Lua50/src/lzio.c
    ThirdParty/Lua50/src/lib/lauxlib.c
    ThirdParty/Lua50/src/lib/lmathlib.c
    ThirdParty/Lua50/src/lib/lstrlib.c
    ThirdParty/Lua50/src/lib/ltablib.c
)
target_include_directories(Lua50 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Lua50/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Lua50/src"
)
if(MSVC)
    target_compile_definitions(Lua50 PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(Lua50 PRIVATE /W1)
endif()

add_executable(WoWClient2 WIN32
    WoW/Source/client.h
    WoW/Source/client.cpp
)

# Keep the preservation/self-test harness available as a separate executable.
# The shipped client target above intentionally enters through the recovered
# WinMain in WoW/Source/client.cpp rather than through the diagnostics harness.
add_executable(WoWClientDiagnostics
    app/main.cpp
)
target_include_directories(WoWClientDiagnostics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWClientDiagnostics PRIVATE cxx_std_20)
target_compile_definitions(WoWClientDiagnostics PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

add_library(Base STATIC
    ENGINE/Source/Base/activity.h ENGINE/Source/Base/activity.cpp
    ENGINE/Source/Base/base.h ENGINE/Source/Base/base.cpp
    ENGINE/Source/Base/cdataallocator.h ENGINE/Source/Base/cdataallocator.cpp
    ENGINE/Source/Base/cdatarecycler.h ENGINE/Source/Base/cdatarecycler.cpp
    ENGINE/Source/Base/cdatastore.h ENGINE/Source/Base/cdatastore.cpp
    ENGINE/Source/Base/cmdline.h ENGINE/Source/Base/cmdline.cpp
    ENGINE/Source/Base/convertutf.h ENGINE/Source/Base/convertutf.cpp
    ENGINE/Source/Base/coordinate.h ENGINE/Source/Base/coordinate.cpp
    ENGINE/Source/Base/cunreal.h ENGINE/Source/Base/cunreal.cpp
    ENGINE/Source/Base/filecache.h ENGINE/Source/Base/filecache.cpp
    ENGINE/Source/Base/handle.h ENGINE/Source/Base/handle.cpp
    ENGINE/Source/Base/msgbuffer.h ENGINE/Source/Base/msgbuffer.cpp
    ENGINE/Source/Base/prop.h ENGINE/Source/Base/prop.cpp
    ENGINE/Source/Base/rcstring.h ENGINE/Source/Base/rcstring.cpp
    ENGINE/Source/Base/status.h ENGINE/Source/Base/status.cpp
    ENGINE/Source/Base/unrealconstants.cpp
)
target_include_directories(Base PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Base PUBLIC cxx_std_20)
if(MSVC)
target_compile_options(Base PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(BlpFile STATIC
    ENGINE/Source/BlpFile/blp.h
    ENGINE/Source/BlpFile/blp.cpp
)
target_include_directories(BlpFile PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(BlpFile PUBLIC cxx_std_20)
target_link_libraries(BlpFile PUBLIC Base)
if(MSVC)
    target_compile_options(BlpFile PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Images STATIC
    ENGINE/Source/Images/images.h
    ENGINE/Source/Images/dxt.h
    ENGINE/Source/Images/blit.cpp
    ENGINE/Source/Images/dxt.cpp
    ENGINE/Source/Images/funcs.cpp
    ENGINE/Source/Images/tga.cpp
)
target_include_directories(Images PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Images PUBLIC cxx_std_20)
target_link_libraries(Images PUBLIC Base Tempest Storm)
if(MSVC)
    target_compile_options(Images PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Regex STATIC
    ENGINE/Source/Regex/regex.h
    ENGINE/Source/Regex/regex_internal.h
    ENGINE/Source/Regex/engine.cpp
    ENGINE/Source/Regex/regcomp.cpp
    ENGINE/Source/Regex/regexec.cpp
    ENGINE/Source/Regex/regfree.cpp
)
target_include_directories(Regex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Regex PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(Regex PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Frame STATIC
    ENGINE/Source/Frame/frame.h
    ENGINE/Source/Frame/framescript.h
    ENGINE/Source/Frame/framescript.cpp
    ENGINE/Source/Frame/framexml.h
    ENGINE/Source/Frame/framexml.cpp
    ENGINE/Source/Frame/cbackdropgenerator.h
    ENGINE/Source/Frame/cframepoint.h
    ENGINE/Source/Frame/cframepoint.cpp
    ENGINE/Source/Frame/clayoutframe.h
    ENGINE/Source/Frame/clayoutframe.cpp
    ENGINE/Source/Frame/cbackdropgenerator.cpp
    ENGINE/Source/Frame/csimplebutton.h
    ENGINE/Source/Frame/csimplebutton.cpp
    ENGINE/Source/Frame/csimplebuttonscript.cpp
    ENGINE/Source/Frame/csimplecheckbox.cpp
    ENGINE/Source/Frame/csimplecheckboxscript.cpp
    ENGINE/Source/Frame/csimpleeditbox.h
    ENGINE/Source/Frame/csimpleeditbox.cpp
    ENGINE/Source/Frame/csimpleeditboxscript.cpp
    ENGINE/Source/Frame/csimpleframe.h
    ENGINE/Source/Frame/csimpleframe.cpp
    ENGINE/Source/Frame/csimpleframescript.cpp
    ENGINE/Source/Frame/csimplehtml.cpp
    ENGINE/Source/Frame/csimplehtml.h
    ENGINE/Source/Frame/csimplehtmlscript.cpp
    ENGINE/Source/Frame/csimplehyperlinkedframe.h
    ENGINE/Source/Frame/csimplehyperlinkedframe.cpp
    ENGINE/Source/Frame/csimplemessageframe.h
    ENGINE/Source/Frame/csimplemessageframe.cpp
    ENGINE/Source/Frame/csimplemessageframescript.cpp
    ENGINE/Source/Frame/csimplemessagescrollframe.h
    ENGINE/Source/Frame/csimplemessagescrollframe.cpp
    ENGINE/Source/Frame/csimplemessagescrollframescript.cpp
    ENGINE/Source/Frame/csimplemodel.h
    ENGINE/Source/Frame/csimplemodel.cpp
    ENGINE/Source/Frame/csimplemodelscript.cpp
    ENGINE/Source/Frame/csimpleregion.cpp
    ENGINE/Source/Frame/csimpleregion.h
    ENGINE/Source/Frame/csimplerender.h
    ENGINE/Source/Frame/csimplerender.cpp
    ENGINE/Source/Frame/csimplerenderscript.cpp
    ENGINE/Source/Frame/csimplescrollframe.cpp
    ENGINE/Source/Frame/csimplescrollframe.h
    ENGINE/Source/Frame/csimplescrollframescript.cpp
    ENGINE/Source/Frame/csimpleslider.cpp
    ENGINE/Source/Frame/csimpleslider.h
    ENGINE/Source/Frame/csimplesliderscript.cpp
    ENGINE/Source/Frame/csimplestatusbar.h
    ENGINE/Source/Frame/csimplestatusbar.cpp
    ENGINE/Source/Frame/csimplestatusbarscript.cpp
    ENGINE/Source/Frame/csimpletop.cpp
    ENGINE/Source/Frame/csimpletop.h
    ENGINE/Source/Frame/simpleframeregistry.h
    ENGINE/Source/Frame/simpleframeregistry.cpp
)
target_include_directories(Frame PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Frame PUBLIC cxx_std_20)
target_compile_definitions(Frame PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(Frame PUBLIC Tempest Base Event Gx Lua50 Storm)
if(MSVC)
    target_compile_options(Frame PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE FRAME_RECOVERED CONFIGURE_DEPENDS ENGINE/Source/Frame/*.asm)
set_source_files_properties(${FRAME_RECOVERED} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(Frame PRIVATE ${FRAME_RECOVERED})

file(GLOB_RECURSE GX_RECOVERED CONFIGURE_DEPENDS
    ENGINE/Source/Gx/*.cpp ENGINE/Source/Gx/*.asm ENGINE/Source/Gx/*.h)
set_source_files_properties(${GX_RECOVERED} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties(
    ENGINE/Source/Gx/cgxdevice/cgxdevice.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dcaps.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dscene.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxdeviced3d.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dstate.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dprim.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dtexture.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dshader.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dxform.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11device.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11scene.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11state.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11prim.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11texture.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglcaps.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxdeviceopengl.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglscene.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglshader.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglstate.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglprim.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxogltexture.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglxform.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/w32/cgxogldevicew32.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/w32/glwndsupport.cpp
    ENGINE/Source/Gx/gxapi/gxapi.cpp
    ENGINE/Source/Gx/gxapi/gxformat.cpp
    PROPERTIES HEADER_FILE_ONLY FALSE)
add_library(Gx STATIC
    ENGINE/Source/Gx/cgxdevice/cgxdevice.h
    ENGINE/Source/Gx/gxapi/gxbase.h
    ENGINE/Source/Gx/cgxdevice/cgxdevice.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dcaps.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dscene.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxdeviced3d.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dstate.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dprim.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dtexture.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dshader.cpp
    ENGINE/Source/Gx/cgxdeviced3d/cgxd3dxform.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxdeviced3d11.h
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11device.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11scene.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11state.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11prim.cpp
    ENGINE/Source/Gx/cgxdeviced3d11/cgxd3d11texture.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglcaps.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxdeviceopengl.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglscene.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglshader.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglstate.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglprim.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxogltexture.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/cgxoglxform.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/w32/cgxogldevicew32.cpp
    ENGINE/Source/Gx/cgxdeviceopengl/w32/glwndsupport.cpp
    ENGINE/Source/Gx/gxapi/gxapi.cpp)
target_sources(Gx PRIVATE ${GX_RECOVERED})
set(GX_D3D11_SHADER "${CMAKE_CURRENT_SOURCE_DIR}/../shaders/gx_legacy.hlsl")
set_source_files_properties("${GX_D3D11_SHADER}" PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(Gx PRIVATE "${GX_D3D11_SHADER}")
source_group("Shaders" FILES "${GX_D3D11_SHADER}")
target_include_directories(Gx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Gx PUBLIC cxx_std_20)
target_compile_definitions(Gx PRIVATE NOMINMAX)
target_link_libraries(Gx PUBLIC d3d9 d3d11 dxgi d3dcompiler opengl32)
if(MSVC)
    target_compile_options(Gx PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE GXU_RECOVERED CONFIGURE_DEPENDS
    ENGINE/Source/Gxu/*.cpp ENGINE/Source/Gxu/*.asm ENGINE/Source/Gxu/*.h)
# These files are the raw Hex-Rays/decompiler exports. Keep them in the
# solution and in the source manifest, but do not present pseudocode as a
# compilable library until its missing declarations and class layouts have
# been reconstructed.
set_source_files_properties(${GXU_RECOVERED} PROPERTIES HEADER_FILE_ONLY TRUE)
add_custom_target(GxuRecoveredSources SOURCES ${GXU_RECOVERED})
add_library(Gxu INTERFACE)
target_sources(Gxu INTERFACE ${GXU_RECOVERED})
target_include_directories(Gxu INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(Event STATIC
    ENGINE/Source/Event/event.h ENGINE/Source/Event/event_internal.h
    ENGINE/Source/Event/observer.h ENGINE/Source/Event/cobserver.cpp
    ENGINE/Source/Event/cinputobserver.h ENGINE/Source/Event/cinputobserver.cpp
    ENGINE/Source/Event/evtapi.cpp ENGINE/Source/Event/evtinput.cpp
    ENGINE/Source/Event/evtqueue.cpp ENGINE/Source/Event/evtsched.cpp ENGINE/Source/Event/evttimer.cpp
)
target_include_directories(Event PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Event PUBLIC cxx_std_20)
target_link_libraries(Event PUBLIC Base OsW32 user32)
if(MSVC)
    target_compile_options(Event PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(SRP STATIC
    ENGINE/Source/SRP/sha.h
    ENGINE/Source/SRP/sha.cpp
    ENGINE/Source/SRP/srp6.h
    ENGINE/Source/SRP/srp6.cpp
)
target_include_directories(SRP PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(SRP PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(SRP PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(OsW32 STATIC
    ENGINE/Source/Os/W32/debugging.cpp
    ENGINE/Source/Os/W32/errors.cpp
    ENGINE/Source/Os/W32/tls.cpp
    ENGINE/Source/Os/W32/osmemory.cpp
    ENGINE/Source/Os/W32/ossystem.cpp
    ENGINE/Source/Os/W32/path.cpp
    ENGINE/Source/Os/W32/time.cpp
    ENGINE/Source/Os/W32/osclipboard.cpp
    ENGINE/Source/Os/W32/osfile.cpp
    ENGINE/Source/Os/W32/osjoystick.cpp
    ENGINE/Source/Os/W32/input.cpp
    ENGINE/Source/Os/W32/osime.cpp
    ENGINE/Source/Os/W32/oscall.cpp
    ENGINE/Source/Os/W32/osisoundcache.cpp
    ENGINE/Source/Os/W32/ossound.cpp
    ENGINE/Source/Os/W32/ossoundmidi.cpp
    ENGINE/Source/Os/W32/ostcp.cpp
    ENGINE/Source/Os/W32/osgui.cpp
)
target_include_directories(OsW32 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(OsW32 PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(OsW32 PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()
target_link_libraries(OsW32 PUBLIC shell32 winmm imm32 ws2_32 comctl32 Storm)

add_library(Tempest STATIC
    ENGINE/Source/Tempest/c22matrix.cpp
    ENGINE/Source/Tempest/c33matrix.cpp
    ENGINE/Source/Tempest/c34matrix.cpp
    ENGINE/Source/Tempest/c44matrix.cpp
    ENGINE/Source/Tempest/c4quaternion.cpp
    ENGINE/Source/Tempest/c2vector.cpp
    ENGINE/Source/Tempest/c3vector.cpp
    ENGINE/Source/Tempest/c3spline.cpp
    ENGINE/Source/Tempest/caabox.cpp
    ENGINE/Source/Tempest/cfacet.cpp
    ENGINE/Source/Tempest/cimvector.cpp
    ENGINE/Source/Tempest/cmath.cpp
    ENGINE/Source/Tempest/cmath_inline.cpp
    ENGINE/Source/Tempest/cmemblock.cpp
    ENGINE/Source/Tempest/crandom.cpp
    ENGINE/Source/Tempest/tempest_intersect.cpp
)

target_include_directories(Tempest PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(Tempest PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(Tempest PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Services STATIC
    ENGINE/Source/Services/asyncfileread.h
    ENGINE/Source/Services/asyncfileread.cpp
    ENGINE/Source/Services/camera.h
    ENGINE/Source/Services/camera.cpp
    ENGINE/Source/Services/datamgr.h
    ENGINE/Source/Services/datamgr.cpp
    ENGINE/Source/Services/profile.h
    ENGINE/Source/Services/profile.cpp
    ENGINE/Source/Services/sysmessage.h
    ENGINE/Source/Services/sysmessage.cpp
    ENGINE/Source/Services/textblock.h
    ENGINE/Source/Services/textblock.cpp
    ENGINE/Source/Services/lightning.h
    ENGINE/Source/Services/lightning.cpp
    ENGINE/Source/Services/particlemisc.h
    ENGINE/Source/Services/particlemisc.cpp
    ENGINE/Source/Services/particlesystem2.h
    ENGINE/Source/Services/particlesystem2.cpp
    ENGINE/Source/Services/particleemitters.h
    ENGINE/Source/Services/particleemitters.cpp
    ENGINE/Source/Services/particlesystem.h
    ENGINE/Source/Services/particlesystem.cpp
    ENGINE/Source/Services/ribbonemitter.h
    ENGINE/Source/Services/ribbonemitter.cpp
    ENGINE/Source/Services/gfxsingletonmanager.h
    ENGINE/Source/Services/gfxsingletonmanager.cpp
    ENGINE/Source/Services/texture.h
    ENGINE/Source/Services/texture.cpp
)
target_include_directories(Services PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Services PUBLIC cxx_std_20)
target_link_libraries(Services PUBLIC Tempest Storm OsW32 Gx gdi32)
if(MSVC)
    target_compile_options(Services PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Scrn STATIC
    ENGINE/Source/Scrn/scrn.h
    ENGINE/Source/Scrn/layer.h
    ENGINE/Source/Scrn/perf.h
    ENGINE/Source/Scrn/stock.h
    ENGINE/Source/Scrn/scrn.cpp
    ENGINE/Source/Scrn/layer.cpp
    ENGINE/Source/Scrn/perf.cpp
    ENGINE/Source/Scrn/stock.cpp
)
target_include_directories(Scrn PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Scrn PUBLIC cxx_std_20)
target_compile_definitions(Scrn PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(Scrn PUBLIC Tempest Base Event Gx Images Services OsW32 Storm)
target_link_libraries(Frame PUBLIC Scrn Services)
if(MSVC)
    target_compile_options(Scrn PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE SCRN_RECOVERED CONFIGURE_DEPENDS ENGINE/Source/Scrn/*.asm)
set_source_files_properties(${SCRN_RECOVERED} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(Scrn PRIVATE ${SCRN_RECOVERED})

add_library(Anim STATIC
    ENGINE/Source/Anim/anim.h
    ENGINE/Source/Anim/anim.cpp
    ENGINE/Source/Anim/init.h
    ENGINE/Source/Anim/init.cpp
    ENGINE/Source/Anim/interface.h
    ENGINE/Source/Anim/interface.cpp
    ENGINE/Source/Anim/interp.h
    ENGINE/Source/Anim/interp.cpp
    ENGINE/Source/Anim/mdlanim.h
    ENGINE/Source/Anim/mdlanim.cpp
    ENGINE/Source/Anim/transform.h
    ENGINE/Source/Anim/transform.cpp
    ENGINE/Source/Anim/worldmatrix.h
    ENGINE/Source/Anim/worldmatrix.cpp
)
target_include_directories(Anim PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(Anim PUBLIC cxx_std_20)
target_link_libraries(Anim PUBLIC Tempest Services)
if(MSVC)
    target_compile_options(Anim PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Model STATIC
    ENGINE/Source/Model/model.h
    ENGINE/Source/Model/modelparse.h
    ENGINE/Source/Model/collisiondata.h
    ENGINE/Source/Model/collisiondata.cpp
    ENGINE/Source/Model/collisiondisplay.h
    ENGINE/Source/Model/collisiondisplay.cpp
    ENGINE/Source/Model/mdlcamera.h
    ENGINE/Source/Model/mdlcamera.cpp
    ENGINE/Source/Model/mdlparticleemitter2.h
    ENGINE/Source/Model/mdlparticleemitter2.cpp
    ENGINE/Source/Model/mdlribbonemitter.h
    ENGINE/Source/Model/mdlribbonemitter.cpp
    ENGINE/Source/Model/mdlsglight.h
    ENGINE/Source/Model/mdlsglight.cpp
    ENGINE/Source/Model/mdlsgmodel.h
    ENGINE/Source/Model/mdlsgmodel.cpp
    ENGINE/Source/Model/mdlvalidate.h
    ENGINE/Source/Model/mdlvalidate.cpp
    ENGINE/Source/Model/modelanim.h
    ENGINE/Source/Model/modelanim.cpp
    ENGINE/Source/Model/modelcreate.h
    ENGINE/Source/Model/modelcreate.cpp
    ENGINE/Source/Model/modelmisc.h
    ENGINE/Source/Model/modelmisc.cpp
    ENGINE/Source/Model/modelrender.h
    ENGINE/Source/Model/modelrender.cpp
)

target_include_directories(Model PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(Model PUBLIC cxx_std_20)
target_link_libraries(Model PUBLIC Tempest Services Storm Anim)

if(MSVC)
    target_compile_options(Model PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE WOW_WORLDCLIENT_SOURCES CONFIGURE_DEPENDS
    WoW/Source/WorldClient/*.h
    WoW/Source/WorldClient/*.cpp
)
add_library(WorldClient STATIC ${WOW_WORLDCLIENT_SOURCES})

target_include_directories(WorldClient PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WorldClient PUBLIC cxx_std_20)
target_link_libraries(WorldClient PUBLIC Model)

if(MSVC)
    target_compile_options(WorldClient PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWCommon STATIC
    WoW/common/wowtime.h
    WoW/common/wowtime.cpp
    WoW/common/compdecorationcore.h
    WoW/common/compdecorationcore.cpp
    WoW/common/aabsp.h
    WoW/common/aabsp.cpp
    WoW/common/gameobjectdefs.h
    WoW/common/gameobjectdefs.cpp
    WoW/common/lightsandfog.h
    WoW/common/lightsandfog.cpp
    WoW/common/daynight.h
    WoW/common/daynight.cpp
    WoW/common/componentcore/charactercustomizationcore.h
    WoW/common/componentcore/charactercustomizationcore.cpp
)

target_include_directories(WoWCommon PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WoWCommon PUBLIC cxx_std_20)
target_link_libraries(WoWCommon PUBLIC Storm Services)
if(MSVC)
    target_compile_options(WoWCommon PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWLoadingScreen STATIC
    WoW/Source/loadingscreen.h
    WoW/Source/loadingscreen.cpp
)

target_include_directories(WoWLoadingScreen PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WoWLoadingScreen PUBLIC cxx_std_20)
target_link_libraries(WoWLoadingScreen PUBLIC Base)
if(MSVC)
    target_compile_options(WoWLoadingScreen PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWLogClient STATIC
    WoW/Source/LogClient/log.h
    WoW/Source/LogClient/log.cpp
)
target_include_directories(WoWLogClient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWLogClient PUBLIC cxx_std_20)
target_link_libraries(WoWLogClient PUBLIC Base)
if(MSVC)
    target_compile_options(WoWLogClient PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWWowServices STATIC
    WoW/Source/WowServices/misc.h
    WoW/Source/WowServices/misc.cpp
    WoW/Source/WowServices/wdatastore.h
    WoW/Source/WowServices/wdatastore.cpp
    WoW/Source/WowServices/wowconnection.h
    WoW/Source/WowServices/wowconnection.cpp
    WoW/Source/WowServices/wowconnectionnet.cpp
    WoW/Source/WowServices/wowconnectionnetselect.cpp
    WoW/Source/WowServices/linux/wowconnectionnetselect.cpp
)
set_source_files_properties(
    WoW/Source/WowServices/linux/wowconnectionnetselect.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE)
target_include_directories(WoWWowServices PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWWowServices PUBLIC cxx_std_20)
target_link_libraries(WoWWowServices PUBLIC Base)
if(MSVC)
    target_compile_options(WoWWowServices PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWSource STATIC
    WoW/Source/clientruntime.h
    WoW/Source/clientruntime.cpp
    WoW/Source/clientcommands.h
    WoW/Source/clientcommands.cpp
    WoW/Source/clientgmcommands.h
    WoW/Source/clientgmcommands.cpp
)

target_include_directories(WoWSource PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WoWSource PUBLIC cxx_std_20)
target_link_libraries(WoWSource PUBLIC Base Storm)
if(MSVC)
    target_compile_options(WoWSource PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWClientBootstrap STATIC
    WoW/Source/client.h
    WoW/Source/client.cpp
)

target_include_directories(WoWClientBootstrap PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WoWClientBootstrap PUBLIC cxx_std_20)
target_link_libraries(WoWClientBootstrap PUBLIC WoWSource WoWLoadingScreen Base Event Services Model Scrn)
if(MSVC)
    target_compile_options(WoWClientBootstrap PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWComponent STATIC
    WoW/Source/component/component.h
    WoW/Source/component/component.cpp
    WoW/Source/component/charactercustomization.h
    WoW/Source/component/charactercustomization.cpp
)

target_include_directories(WoWComponent PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WoWComponent PUBLIC cxx_std_20)
target_link_libraries(WoWComponent PUBLIC WoWCommon WoWLoadingScreen Storm Services)
if(MSVC)
    target_compile_options(WoWComponent PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWObjectAlloc STATIC
    WoW/Source/ObjectAlloc/objectalloc.h
    WoW/Source/ObjectAlloc/objectalloc.cpp
)
target_include_directories(WoWObjectAlloc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWObjectAlloc PUBLIC cxx_std_20)
target_link_libraries(WoWObjectAlloc PUBLIC WoWConsole Base)
if(MSVC)
    target_compile_options(WoWObjectAlloc PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE WOW_UIUTIL_SOURCES CONFIGURE_DEPENDS
    WoW/Source/UIUtil/*.h
    WoW/Source/UIUtil/*.cpp
)
add_library(WoWUIUtil STATIC ${WOW_UIUTIL_SOURCES})
target_include_directories(WoWUIUtil PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWUIUtil PUBLIC cxx_std_20)
target_link_libraries(WoWUIUtil PUBLIC Frame Base WoWObjectAlloc Gx Tempest)
if(MSVC)
    target_compile_options(WoWUIUtil PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE WOW_UI_SOURCES CONFIGURE_DEPENDS
    WoW/Source/UI/*.h
    WoW/Source/UI/*.cpp
)
add_library(WoWUI STATIC ${WOW_UI_SOURCES})
target_include_directories(WoWUI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWUI PUBLIC cxx_std_20)
target_link_libraries(WoWUI PUBLIC Frame WoWUIUtil WoWGameClient)
if(MSVC)
    target_compile_options(WoWUI PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE WOW_OBJECTMGRCLIENT_SOURCES CONFIGURE_DEPENDS
    WoW/Source/ObjectMgrClient/*.h
    WoW/Source/ObjectMgrClient/*.cpp
)
add_library(WoWObjectMgrClient STATIC ${WOW_OBJECTMGRCLIENT_SOURCES})
target_include_directories(WoWObjectMgrClient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWObjectMgrClient PUBLIC cxx_std_20)
target_link_libraries(WoWObjectMgrClient PUBLIC Object WoWNet WoWDBAutoCode Base)
if(MSVC)
    target_compile_options(WoWObjectMgrClient PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE WOW_WOWSVCS_SOURCES CONFIGURE_DEPENDS
    WoW/Source/WowSvcs/*.h
    WoW/Source/WowSvcs/*.cpp
)
add_library(WoWWowSvcs STATIC ${WOW_WOWSVCS_SOURCES})
target_include_directories(WoWWowSvcs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWWowSvcs PUBLIC cxx_std_20)
target_link_libraries(WoWWowSvcs PUBLIC WoWWowServices WoWNet Frame SRP Base Storm)
if(MSVC)
    target_compile_options(WoWWowSvcs PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWNet STATIC
    WoW/Source/Net/netinternal.h
    WoW/Source/Net/netinternal.cpp
    WoW/Source/Net/netclient.h
    WoW/Source/Net/netclient.cpp
    WoW/Source/Net/netclient/netclient.h
    WoW/Source/Net/netclient/netclient.cpp
)
set_source_files_properties(
    WoW/Source/Net/netclient/netclient.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE)
target_include_directories(WoWNet PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWNet PUBLIC cxx_std_20)
target_link_libraries(WoWNet PUBLIC Event OsW32 Base)
if(MSVC)
    target_compile_options(WoWNet PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWMagicNet STATIC
    WoW/Source/Magic/spellnet.h
    WoW/Source/Magic/spellnet.cpp
)
target_include_directories(WoWMagicNet PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWMagicNet PUBLIC cxx_std_20)
target_link_libraries(WoWMagicNet PUBLIC Base Tempest)
if(MSVC)
    target_compile_options(WoWMagicNet PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWMagic STATIC
    WoW/Source/Magic/spell_c.h
    WoW/Source/Magic/spell_c.cpp
    WoW/Source/Magic/spelltable.h
    WoW/Source/Magic/spelltable.cpp
    WoW/Source/Magic/spellvisuals.h
    WoW/Source/Magic/spellvisuals.cpp
    WoW/Source/Magic/magicclient/spell_c.h
    WoW/Source/Magic/magicclient/spell_c.cpp
    WoW/Source/Magic/magicclient/spelltable.h
    WoW/Source/Magic/magicclient/spelltable.cpp
    WoW/Source/Magic/magicclient/spellvisuals.h
    WoW/Source/Magic/magicclient/spellvisuals.cpp
)
set_source_files_properties(
    WoW/Source/Magic/magicclient/spell_c.cpp
    WoW/Source/Magic/magicclient/spelltable.cpp
    WoW/Source/Magic/magicclient/spellvisuals.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE)
target_include_directories(WoWMagic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWMagic PUBLIC cxx_std_20)
target_link_libraries(WoWMagic PUBLIC WoWMagicNet WoWDBAutoCode Base Tempest Model WorldClient)
if(MSVC)
    target_compile_options(WoWMagic PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWSoundGlueMusic STATIC
    WoW/Source/SoundInterface/soundinterfacegluemusic.h
    WoW/Source/SoundInterface/soundinterfacegluemusic.cpp
)
target_include_directories(WoWSoundGlueMusic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWSoundGlueMusic PUBLIC cxx_std_20)
target_link_libraries(WoWSoundGlueMusic PUBLIC OsW32)
if(MSVC)
    target_compile_options(WoWSoundGlueMusic PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWSoundWorld STATIC
    WoW/Source/SoundInterface/soundinterfacemidi.h
    WoW/Source/SoundInterface/soundinterfacemidi.cpp
    WoW/Source/SoundInterface/soundinterfacezonemusic.h
    WoW/Source/SoundInterface/soundinterfacezonemusic.cpp
    WoW/Source/SoundInterface/soundinterfacezoneintro.h
    WoW/Source/SoundInterface/soundinterfacezoneintro.cpp
    WoW/Source/SoundInterface/soundinterfaceproviderprefs.h
    WoW/Source/SoundInterface/soundinterfaceproviderprefs.cpp
    WoW/Source/SoundInterface/soundinterfacewater.h
    WoW/Source/SoundInterface/soundinterfacewater.cpp
    WoW/Source/SoundInterface/soundinterfacevocal.h
    WoW/Source/SoundInterface/soundinterfacevocal.cpp
    WoW/Source/SoundInterface/soundinterfacedoodad.h
    WoW/Source/SoundInterface/soundinterfacedoodad.cpp
    WoW/Source/SoundInterface/soundinterface.h
    WoW/Source/SoundInterface/soundinterface.cpp
    WoW/Source/SoundInterface/isoundinterface.h
    WoW/Source/SoundInterface/isoundinterface.cpp
    WoW/Source/SoundInterface/snddebug.h
    WoW/Source/SoundInterface/snddebug.cpp
    WoW/Source/SoundInterface/snddebugindoors.h
    WoW/Source/SoundInterface/snddebugindoors.cpp
    WoW/Source/SoundInterface/snddebugoutdoors.h
    WoW/Source/SoundInterface/snddebugoutdoors.cpp
)
target_include_directories(WoWSoundWorld PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWSoundWorld PUBLIC cxx_std_20)
target_link_libraries(WoWSoundWorld PUBLIC OsW32 Event WoWDBAutoCode WoWConsole Frame)
if(MSVC)
    target_compile_options(WoWSoundWorld PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWGlueScriptEvents STATIC
    WoW/Source/Glue/gluescriptevents.h
    WoW/Source/Glue/gluescriptevents.cpp
)
target_include_directories(WoWGlueScriptEvents PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWGlueScriptEvents PUBLIC cxx_std_20)
target_link_libraries(WoWGlueScriptEvents PUBLIC Frame OsW32 WoWNet WoWSoundGlueMusic)
if(MSVC)
    target_compile_options(WoWGlueScriptEvents PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWGlueMgr STATIC
    WoW/Source/Glue/cgluemgr.h
    WoW/Source/Glue/cgluemgr.cpp
    WoW/Source/Glue/charselectinfo.h
    WoW/Source/Glue/charselectinfo.cpp
    WoW/Source/Glue/charcreateinfo.h
    WoW/Source/Glue/charcreateinfo.cpp
)
target_include_directories(WoWGlueMgr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWGlueMgr PUBLIC cxx_std_20)
target_link_libraries(WoWGlueMgr PUBLIC WoWGlueScriptEvents Services Event OsW32)
if(MSVC)
    target_compile_options(WoWGlueMgr PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWCharSelect STATIC
    WoW/Source/Glue/charselectinfo.h
    WoW/Source/Glue/charselectinfo.cpp
)
target_include_directories(WoWCharSelect PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWCharSelect PUBLIC cxx_std_20)
target_link_libraries(WoWCharSelect PUBLIC WoWGlueMgr Frame Tempest)
if(MSVC)
    target_compile_options(WoWCharSelect PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWCharCreate STATIC
    WoW/Source/Glue/charcreateinfo.h
    WoW/Source/Glue/charcreateinfo.cpp
)
target_include_directories(WoWCharCreate PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWCharCreate PUBLIC cxx_std_20)
target_link_libraries(WoWCharCreate PUBLIC WoWGlueMgr Frame Tempest)
if(MSVC)
    target_compile_options(WoWCharCreate PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWConsole STATIC
    WoW/Source/Console/consoleclient.h
    WoW/Source/Console/consoleclient.cpp
    WoW/Source/Console/consolecommand.h
    WoW/Source/Console/consolecommand.cpp
    WoW/Source/Console/consoledetect.h
    WoW/Source/Console/consoledetect.cpp
    WoW/Source/Console/consolevar.h
    WoW/Source/Console/consolevar.cpp
    WoW/Source/Console/iconsole.h
    WoW/Source/Console/iconsole.cpp
)
target_include_directories(WoWConsole PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWConsole PUBLIC cxx_std_20)
target_link_libraries(WoWConsole PUBLIC WoWSource Base Gx)
if(MSVC)
    target_compile_options(WoWConsole PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB WOW_DB_AUTOCODE CONFIGURE_DEPENDS
    WoW/Source/DB/DBClient/AutoCode/*.cpp
    WoW/Source/DB/DBClient/AutoCode/*.h)
add_library(WoWDBAutoCode STATIC
    WoW/Source/DB/DBClient/dbrecord.h
    WoW/Source/DB/DBClient/dbcache.h
    WoW/Source/DB/DBClient/dbcache.cpp
    WoW/Source/DB/DBClient/dbcacheinstances.h
    WoW/Source/DB/DBClient/dbcacheinstances.cpp
    WoW/Source/DB/DBClient/dbclient.h
    WoW/Source/DB/DBClient/dbclient.cpp
    WoW/Source/DB/DBClient/sdbitemsubclass.h
    WoW/Source/DB/DBClient/sdbitemsubclass.cpp
    WoW/Source/DB/DBClient/sdbwmoareatable.h
    WoW/Source/DB/DBClient/sdbwmoareatable.cpp
    ${WOW_DB_AUTOCODE}
)
target_include_directories(WoWDBAutoCode PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWDBAutoCode PUBLIC cxx_std_20)
target_link_libraries(WoWDBAutoCode PUBLIC Base)
if(MSVC)
    target_compile_options(WoWDBAutoCode PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWGame STATIC
    WoW/Source/Game/gametime.h
    WoW/Source/Game/gametime.cpp
    WoW/Source/Game/loot.h
    WoW/Source/Game/loot.cpp
    WoW/Source/Game/validatename.h
    WoW/Source/Game/validatename.cpp
)

target_include_directories(WoWGame PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWGame PUBLIC cxx_std_20)
target_link_libraries(WoWGame PUBLIC WoWCommon WoWDBAutoCode Base)
if(MSVC)
    target_compile_options(WoWGame PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WoWGameClient STATIC
    WoW/Source/Game/GameClient/gametime_c.h
    WoW/Source/Game/GameClient/gametime_c.cpp
    WoW/Source/Game/GameClient/guildclient.h
    WoW/Source/Game/GameClient/guildclient.cpp
    WoW/Source/Game/GameClient/namecache.h
    WoW/Source/Game/GameClient/namecache.cpp
    WoW/Source/Game/GameClient/petnamecache.h
    WoW/Source/Game/GameClient/petnamecache.cpp
    WoW/Source/Game/GameClient/questcache.h
    WoW/Source/Game/GameClient/questcache.cpp
    WoW/Source/Game/GameClient/taximap.h
    WoW/Source/Game/GameClient/taximap.cpp
    WoW/Source/Game/GameClient/violencelevels.h
    WoW/Source/Game/GameClient/violencelevels.cpp
    WoW/Source/Game/GameClient/worldtext.h
    WoW/Source/Game/GameClient/worldtext.cpp
    WoW/Source/Game/GameClient/minimap.h
    WoW/Source/Game/GameClient/minimap.cpp
    WoW/Source/Game/GameClient/playername.h
    WoW/Source/Game/GameClient/playername.cpp
    WoW/Source/Game/GameClient/shadow.h
    WoW/Source/Game/GameClient/shadow.cpp
    WoW/Source/Game/GameClient/weapontrails.h
    WoW/Source/Game/GameClient/weapontrails.cpp
)
target_include_directories(WoWGameClient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(WoWGameClient PUBLIC cxx_std_20)
target_link_libraries(WoWGameClient PUBLIC WoWGame WoWConsole WoWDBAutoCode Base)
if(MSVC)
    target_compile_options(WoWGameClient PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

file(GLOB_RECURSE WOW_OBJECT_SOURCES CONFIGURE_DEPENDS
    WoW/Source/Object/*.h
    WoW/Source/Object/*.cpp
)
add_library(Object STATIC ${WOW_OBJECT_SOURCES})

target_include_directories(Object PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(Object PUBLIC cxx_std_20)
target_link_libraries(Object PUBLIC WorldClient)

if(MSVC)
    target_compile_options(Object PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(Storm STATIC
    Storm/h/sapibase.h
    Storm/h/stpl.h
    Storm/h/storm.h
    Storm/h/sapi.h
    Storm/h/sclass.h
    Storm/h/SAPIEXTEND.H
    Storm/h/SFile2.h
    Storm/h/SFile.h
    Storm/h/SEvt.h
    Storm/h/SMem.h
    Storm/h/SSignature.h
    Storm/h/SLog.h
    Storm/h/LogMachineState.h
    Storm/h/SCmd.h
    Storm/h/SMsg.h
    Storm/h/SComp.h
    Storm/h/SThread.h
    Storm/h/SZip.h
    Storm/h/storm.cpp
    Storm/Source/CRC.cpp
    Storm/Source/CryptRSA.cpp
    Storm/Source/LogMachineState.cpp
    Storm/Source/SBig.cpp
    Storm/Source/SCmd.cpp
    Storm/Source/SComp.cpp
    Storm/Source/SErr.cpp
    Storm/Source/SEvt.cpp
    Storm/Source/SFile.cpp
    Storm/Source/SFile2.cpp
    Storm/Source/SLog.cpp
    Storm/Source/SMem.cpp
    Storm/Source/SMsg.cpp
    Storm/Source/SOption.cpp
    Storm/Source/SReg.cpp
    Storm/Source/SSignature.cpp
    Storm/Source/SRgn.cpp
    Storm/Source/SRtl.cpp
    Storm/Source/Sha1.cpp
    Storm/Source/SStr.cpp
    Storm/Source/soprintf.cpp
    Storm/Source/SUnicode.cpp
    Storm/Source/SZip.cpp
    Storm/Source/Storm.cpp
    Storm/Source/StormService.cpp
    Storm/Source/W32/SSystem.cpp
    Storm/Source/W32/SLock.cpp
    Storm/Source/W32/SThread.cpp
    Storm/Source/W32/SInterlocked.cpp
    Storm/Source/W32/SDir.cpp
    Storm/Source/W32/SServer.cpp
)

target_include_directories(Storm PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(Storm PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(Storm PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

add_library(WorldCommon STATIC
    WoW/Source/WorldCommon/WorldMath.cpp
)

target_include_directories(WorldCommon PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(WorldCommon PUBLIC cxx_std_20)
target_link_libraries(WorldCommon PUBLIC Object Storm)

if(MSVC)
    target_compile_options(WorldCommon PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

# Register the complete recovered header trees with their owning targets.
# Several original projects relied on umbrella headers or precompiled-header
# discovery; explicit recursive registration keeps every recovered declaration
# visible in the generated Visual Studio projects without changing paths.
add_recovered_headers(Base ENGINE/Source/Base)
add_recovered_headers(BlpFile ENGINE/Source/BlpFile)
add_recovered_headers(Images ENGINE/Source/Images)
add_recovered_headers(Regex ENGINE/Source/Regex)
add_recovered_headers(Frame ENGINE/Source/Frame)
add_recovered_headers(Gx ENGINE/Source/Gx)
add_recovered_headers(Event ENGINE/Source/Event)
add_recovered_headers(SRP ENGINE/Source/SRP)
add_recovered_headers(OsW32 ENGINE/Source/Os/W32)
add_recovered_headers(Tempest ENGINE/Source/Tempest)
add_recovered_headers(Services ENGINE/Source/Services)
add_recovered_headers(Scrn ENGINE/Source/Scrn)
add_recovered_headers(Anim ENGINE/Source/Anim)
add_recovered_headers(Model ENGINE/Source/Model)
add_recovered_headers(Storm Storm)
add_recovered_headers(WoWCommon WoW/common)
add_recovered_headers(WoWLogClient WoW/Source/LogClient)
add_recovered_headers(WoWWowServices WoW/Source/WowServices)
add_recovered_headers(WoWComponent WoW/Source/component)
add_recovered_headers(WoWObjectAlloc WoW/Source/ObjectAlloc)
add_recovered_headers(WoWUIUtil WoW/Source/UIUtil)
add_recovered_headers(WoWUI WoW/Source/UI)
add_recovered_headers(WoWObjectMgrClient WoW/Source/ObjectMgrClient)
add_recovered_headers(WoWWowSvcs WoW/Source/WowSvcs)
add_recovered_headers(WoWNet WoW/Source/Net)
add_recovered_headers(WoWMagicNet WoW/Source/Magic)
add_recovered_headers(WoWMagic WoW/Source/Magic)
add_recovered_headers(WoWSoundGlueMusic WoW/Source/SoundInterface)
add_recovered_headers(WoWSoundWorld WoW/Source/SoundInterface)
add_recovered_headers(WoWGlueScriptEvents WoW/Source/Glue)
add_recovered_headers(WoWGlueMgr WoW/Source/Glue)
add_recovered_headers(WoWConsole WoW/Source/Console)
add_recovered_headers(WoWDBAutoCode WoW/Source/DB)
add_recovered_headers(WoWGame WoW/Source/Game)
add_recovered_headers(WoWGameClient WoW/Source/Game/GameClient)
add_recovered_headers(Object WoW/Source/Object)
add_recovered_headers(WorldClient WoW/Source/WorldClient)
add_recovered_headers(WorldCommon WoW/Source/WorldCommon)

target_compile_features(WoWClient2 PRIVATE cxx_std_20)
target_compile_definitions(WoWClient2 PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(MSVC)
    target_compile_options(WoWClient2 PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

target_link_libraries(WoWClient2 PRIVATE
    WorldCommon
    WoWCommon
    WoWComponent
    WoWObjectAlloc
    WoWUI
    WoWUIUtil
    WoWGameClient
    WoWGame
    WoWObjectMgrClient
    Object
    WoWMagic
    WoWMagicNet
    WoWSoundWorld
    WoWSoundGlueMusic
    WoWGlueMgr
    WoWGlueScriptEvents
    WoWCharSelect
    WoWCharCreate
    WoWWowSvcs
    WoWWowServices
    WoWNet
    WoWDBAutoCode
    WoWConsole
    WoWSource
    WoWLoadingScreen
    WoWLogClient
    WorldClient
    Model
    Anim
    Scrn
    Frame
    Gx
    Images
    Regex
    Event
    Services
    OsW32
    SRP
    BlpFile
    Tempest
    Base
    Storm
)

target_link_libraries(WoWClientDiagnostics PRIVATE
    WorldCommon WoWCommon WoWComponent WoWObjectAlloc
    WoWUI WoWUIUtil WoWGameClient WoWGame WoWObjectMgrClient Object WoWMagic
    WoWMagicNet WoWSoundWorld WoWSoundGlueMusic WoWGlueMgr WoWGlueScriptEvents
    WoWCharSelect WoWCharCreate WoWWowSvcs WoWWowServices WoWNet WoWDBAutoCode
    WoWConsole WoWSource WoWLoadingScreen WoWLogClient WorldClient Model Anim
    Scrn Frame Gx Images Regex Event Services OsW32 SRP BlpFile Tempest Base Storm
)

set_target_properties(WoWClient2 PROPERTIES
    OUTPUT_NAME "WoWClient2"
    RUNTIME_OUTPUT_DIRECTORY "${WOW_INSTALL_ROOT}"
    VS_DEBUGGER_WORKING_DIRECTORY "${WOW_RUNTIME_WORKING_DIRECTORY}"
)

set(WOW_GLUE_OVERRIDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/WoW/Interface/GlueXML")
set(WOW_GLUE_RUNTIME_DIR "${WOW_INSTALL_ROOT}/Interface/GlueXML")
add_custom_command(TARGET WoWClient2 POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${WOW_GLUE_RUNTIME_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${WOW_GLUE_OVERRIDE_DIR}/GlueXML.toc"
            "${WOW_GLUE_OVERRIDE_DIR}/AccountCredentials.lua"
            "${WOW_GLUE_OVERRIDE_DIR}/AccountCredentials.xml"
            "${WOW_GLUE_RUNTIME_DIR}"
    VERBATIM
)

if(BUILD_TESTING)
    add_executable(BlpEventTests tests/blp_event_tests.cpp)
    target_include_directories(BlpEventTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(BlpEventTests PRIVATE cxx_std_20)
    target_compile_definitions(BlpEventTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(BlpEventTests PRIVATE BlpFile Event)
    if(MSVC)
        target_compile_options(BlpEventTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME BlpEventTests COMMAND BlpEventTests)

    add_executable(BaseTests tests/base_tests.cpp)
    target_include_directories(BaseTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(BaseTests PRIVATE cxx_std_20)
    target_compile_definitions(BaseTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(BaseTests PRIVATE Base)
    if(MSVC)
        target_compile_options(BaseTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME BaseTests COMMAND BaseTests)

    add_executable(ModelServicesTests tests/model_services_tests.cpp)
    target_include_directories(ModelServicesTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(ModelServicesTests PRIVATE cxx_std_20)
    target_compile_definitions(ModelServicesTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(ModelServicesTests PRIVATE Model Services OsW32 Base)
    if(MSVC)
        target_compile_options(ModelServicesTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME ModelServicesTests COMMAND ModelServicesTests)

    add_executable(FrameTests tests/frame_tests.cpp)
    target_include_directories(FrameTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(FrameTests PRIVATE cxx_std_20)
    target_compile_definitions(FrameTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(FrameTests PRIVATE Frame Model)
    if(MSVC)
        target_compile_options(FrameTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME FrameTests COMMAND FrameTests)

    add_executable(ScrnTests tests/scrn_tests.cpp)
    target_include_directories(ScrnTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(ScrnTests PRIVATE cxx_std_20)
    target_compile_definitions(ScrnTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(ScrnTests PRIVATE Scrn)
    if(MSVC)
        target_compile_options(ScrnTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME ScrnTests COMMAND ScrnTests)

    add_executable(StormArchiveTests tests/storm_archive_tests.cpp)
    target_include_directories(StormArchiveTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(StormArchiveTests PRIVATE cxx_std_20)
    target_compile_definitions(StormArchiveTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(StormArchiveTests PRIVATE Storm)
    if(MSVC)
        target_compile_options(StormArchiveTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME StormArchiveTests COMMAND StormArchiveTests "${WOW_INSTALL_ROOT}/Data/interface.MPQ")

    add_executable(StormExtract tests/storm_extract.cpp)
    target_include_directories(StormExtract PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(StormExtract PRIVATE cxx_std_20)
    target_compile_definitions(StormExtract PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(StormExtract PRIVATE Storm)
    if(MSVC)
        target_compile_options(StormExtract PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()

    add_executable(FrameScriptTests tests/framescript_tests.cpp)
    target_include_directories(FrameScriptTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(FrameScriptTests PRIVATE cxx_std_20)
    target_compile_definitions(FrameScriptTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(FrameScriptTests PRIVATE Frame Model)
    if(MSVC)
        target_compile_options(FrameScriptTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME FrameScriptTests COMMAND FrameScriptTests "${WOW_INSTALL_ROOT}/Data/interface.MPQ")

    add_executable(FrameXMLTests tests/framexml_tests.cpp)
    target_include_directories(FrameXMLTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(FrameXMLTests PRIVATE cxx_std_20)
    target_compile_definitions(FrameXMLTests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(FrameXMLTests PRIVATE Frame Model)
    if(MSVC)
        target_compile_options(FrameXMLTests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME FrameXMLTests COMMAND FrameXMLTests "${WOW_INSTALL_ROOT}/Data/interface.MPQ")
endif()

foreach(configuration Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${configuration}" configuration_upper)
    set_target_properties(WoWClient2 PROPERTIES
        "RUNTIME_OUTPUT_DIRECTORY_${configuration_upper}" "${WOW_INSTALL_ROOT}"
    )
endforeach()

message(STATUS "WoWClient2 executable output: ${WOW_INSTALL_ROOT}/WoWClient2.exe")
message(STATUS "WoWClient2 debugger working directory: ${WOW_RUNTIME_WORKING_DIRECTORY}")
