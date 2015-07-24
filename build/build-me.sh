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
echo "$BN: Doing 'cmake ..', output to $TMPLOG..."
cmake .. >$TMPLOG 2>&1
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

