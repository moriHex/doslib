# this makefile is included from all the dos*.mak files, do not use directly
# NTS: HPS is either \ (DOS) or / (Linux)

CFLAGS_THIS = -fr=nul -fo=$(SUBDIR)$(HPS).obj -i.. -i"../.." -i"../../.."
NOW_BUILDING = HW_VGA_LIB

OBJS =        $(SUBDIR)$(HPS)libbmp.obj

LIBBMP_LIB =  ..$(HPS)..$(HPS)$(SUBDIR)$(HPS)libbmp.lib

$(LIBBMP_LIB):
	@cd ..$(HPS)..$(HPS)
	@$(MAKECMD) build lib $(SUBDIR)
	@cd $(HERE)

!ifndef PC98
! ifndef TARGET_WINDOWS
160X200A_EXE =     $(SUBDIR)$(HPS)160x200a.$(EXEEXT)
320X200A_EXE =     $(SUBDIR)$(HPS)320x200a.$(EXEEXT)
640X200A_EXE =     $(SUBDIR)$(HPS)640x200a.$(EXEEXT)
! endif
!endif

# NTS we have to construct the command line into tmp.cmd because for MS-DOS
# systems all arguments would exceed the pitiful 128 char command line limit
.c.obj:
	%write tmp.cmd $(CFLAGS_THIS) $(CFLAGS_CON) $[@
	$(CC) @tmp.cmd
!ifdef TINYMODE
	$(OMFSEGDG) -i $@ -o $@
!endif

all: $(OMFSEGDG) lib exe
       
lib: $(LIBBMP_LIB) .symbolic

exe: $(160X200A_EXE) $(320X200A_EXE) $(640X200A_EXE) .symbolic

!ifdef 160X200A_EXE
$(160X200A_EXE): $(LIBBMP_LIB) $(SUBDIR)$(HPS)160x200a.obj
	%write tmp.cmd option quiet option map=$(160X200A_EXE).map system $(WLINK_CON_SYSTEM) library $(LIBBMP_LIB) file $(SUBDIR)$(HPS)160x200a.obj name $(160X200A_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)img$(HPS)4bpp$(HPS)w160$(HPS)200cga.bmp $(SUBDIR)$(HPS)16020016.bmp
	@$(COPY) ..$(HPS)..$(HPS)..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe
!endif

!ifdef 320X200A_EXE
$(320X200A_EXE): $(LIBBMP_LIB) $(SUBDIR)$(HPS)320x200a.obj
	%write tmp.cmd option quiet option map=$(320X200A_EXE).map system $(WLINK_CON_SYSTEM) library $(LIBBMP_LIB) file $(SUBDIR)$(HPS)320x200a.obj name $(320X200A_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)img$(HPS)4bpp$(HPS)w320$(HPS)200cga.bmp $(SUBDIR)$(HPS)32020016.bmp
	@$(COPY) ..$(HPS)..$(HPS)..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe
!endif

!ifdef 640X200A_EXE
$(640X200A_EXE): $(LIBBMP_LIB) $(SUBDIR)$(HPS)640x200a.obj
	%write tmp.cmd option quiet option map=$(640X200A_EXE).map system $(WLINK_CON_SYSTEM) library $(LIBBMP_LIB) file $(SUBDIR)$(HPS)640x200a.obj name $(640X200A_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)img$(HPS)2bpp$(HPS)w640$(HPS)200cga4.bmp $(SUBDIR)$(HPS)6402004.bmp
	@$(COPY) ..$(HPS)..$(HPS)..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe
!endif

clean: .SYMBOLIC
          del $(SUBDIR)$(HPS)*.obj
          del $(HW_VGA_LIB)
          del tmp.cmd
          @echo Cleaning done

