INCLUDE(TribitsSubPackageMacros)
INCLUDE(TribitsLibraryMacros)
INCLUDE(FortranSettings)

TRIBITS_SUBPACKAGE(Mapvarlib)

SET(SUPLIB_VERSION_MAJOR "1")
SET(SUPLIB_VERSION_MINOR "00")
SET(SUPLIB_VERSION_PATCH "0")
SET(MAPVARLIB_VERSION "${MAPVARLIB_VERSION_MAJOR}.${MAPVARLIB_VERSION_MINOR}")
SET(MAPVARLIB_VERSION_FULL "${MAPVARLIB_VERSION}.${MAPVARLIB_VERSION_PATCH}")

INCLUDE_DIRECTORIES(
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

SET(HEADERS  aexds1.blk amesh.blk  contrl.blk ex2tp.blk  ntpdat.blk steps.blk  toldat.blk varnpt.blk
	     aexds2.blk bmesh.blk  ebbyeb.blk header.blk rundat.blk tapes.blk  varept.blk)

SET(SOURCES
	avg.f    bldsrf.f cntr.f   elton1.f extq.f   frge.f   intrpe.f   jacobn.f mkei.f   qadsrc.f rdb1.f
	seton1.f sintpn.f tranab.f wrtc.f   bldpte.f bs.f     elgrad.f   error.f  exts.f   hexsrc.f intrpn.f
	node.f   rda1.f   rdb2.f   shapef.f stran.f  trutbl.f bldptn.f   clsfil.f elton0.f exth.f
	flgrad.f intrp3.f invcon.f mke.f    mvopnfil.f rda2.f   seton0.f   shlsrc.f tetsrc.f vol.f    ininod.f
	initlz.f debug.f  inielt.f sintpe.f rdinpt.f )

TRIBITS_ADD_LIBRARY(mapvarlib HEADERS ${HEADERS} SOURCES ${SOURCES} DEPLIBS exodus_for suplib)

TRIBITS_SUBPACKAGE_POSTPROCESS()
