#!/bin/sh
#< build-me.sh for Sudoku - 20200730 -20150724

BN=`basename $0`

TMPLOG="bldlog-u.txt"
if [ -f "$TMPLOG" ]; then
    if [ -f "$TMPLOG.bak" ]; then
        rm -f $TMPLOG.bak
    fi
    mv $TMPLOG $TMPLOG.bak
fi

give_help()
{
    echo ""
    echo "$BN: Some suggested options to use for debug..."
    echo "  VERBOSE = Add -DCMAKE_VERBOSE_MAKEFILE=TRUE - set verbose Makefile to see flags. switches, libraries, etc..."
    echo "  DEBUG   = -DCMAKE_BUILD_TYPE=DEBUG - to add symbols for gdb use (add -g compiler switch). Def=Release"
    echo "  Then run gdb with '\$ gdb --args ask prompt'"
    echo "  -DCMAKE_INSTALL_PREFIX:PATH=$HOME - to add a spcific install location"
    echo ""
    exit 1
}

TMPOPTS=""
TMPDBG=0
TMPHLP=0

#for arg in $@; do
#    TMPOPTS="$TMPOPTS $arg"
#done

##############################################
### ***** NOTE THIS INSTALL LOCATION ***** ###
### Change to suit your taste, environment ###
##############################################
TMPOPTS="-DCMAKE_INSTALL_PREFIX=$HOME"
#############################################
# Use -DCMAKE_BUILD_TYPE=Debug to add gdb symbols
# Use -DCMAKE_VERBOSE_MAKEFILE=ON

# correctly quoted $@ is the default for shell for loops
for arg; do
    case "$arg" in
       VERBOSE) TMPOPTS="$TMPOPTS -DCMAKE_VERBOSE_MAKEFILE=ON";;
        DEBUG) TMPDBG=1;;
        *) echo "Unknown argument '$arg'!"; TMPHLP=1;;
    esac
done

if [ "$TMPHLP" = "1" ]; then
    give_help
fi

if [ "$TMPDBG" = "1" ]; then
    TMPOPTS="$TMPOPTS -DCMAKE_BUILD_TYPE=Debug"
else
    TMPOPTS="$TMPOPTS -DCMAKE_BUILD_TYPE=Release"
fi

echo "$BN: Doing 'cmake .. $TMPOPTS', output to $TMPLOG..."
cmake .. $TMPOPTS >$TMPLOG 2>&1
if [ ! "$?" = "0" ]; then
    echo "$BN: Cmake config, generation error!"
    exit 1
fi

echo "$BN: Doing 'make' output to $TMPLOG"
make >> $TMPLOG 2>&1
if [ ! "$?" = "0" ]; then
    echo "$BN: Appears a make error... see $TMPLOG"
    exit 1
fi
echo "$BN: Appears a successful build... see $TMPLOG"
exit 0

# eof

