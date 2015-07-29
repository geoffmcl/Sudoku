#!/bin/sh
#< build-me.sh for Sudoku - 20150724

BN=`basename $0`

TMPLOG="bldlog-u.txt"
if [ -f "$TMPLOG" ]; then
    if [ -f "$TMPLOG.bak" ]; then
        rm -f $TMPLOG.bak
    fi
    mv $TMPLOG $TMPLOG.bak
fi
echo ""
echo "$BN: Some suggested cmake options to use for debug..."
echo "  -DCMAKE_VERBOSE_MAKEFILE=TRUE - use a verbose Makefile good to see flags. switches, libraries, etc..."
echo "  -DCMAKE_BUILD_TYPE=DEBUG - to add symbols for gdb use (add -g compiler switch)"
echo "  Then run gdb with '\$ gdb --args ask prompt'"
echo "  -DCMAKE_INSTALL_PREFIX:PATH=$HOME - to add a spcific install location"
echo ""

TMPOPTS=""
for arg in $@; do
    TMPOPTS="$TMPOPTS $arg"
done


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

