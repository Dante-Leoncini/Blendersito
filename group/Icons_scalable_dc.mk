#
# ==============================================================================
#  Name        : Icons_scalable_dc.mk
#  Part of     : OpenGLEx / BlenderLite
#
#  Copyright (c) 2004-2006 Nokia Corporation.
#  This material, including documentation and any related
#  computer programs, is protected by copyright controlled by
#  Nokia Corporation.
# ==============================================================================
#


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\blenderlite_icon.mif

ICONDIR=..\gfx

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) \
		/c32 $(ICONDIR)\qgn_menu_blenderlite.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

