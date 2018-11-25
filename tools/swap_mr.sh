#!/bin/bash

dir=`dirname "${0}"`
BKDIR=backups

find_same_md () {
    if [ ! -e ../$BKDIR ]; then
        echo "Not found $dir./$BKDIR"
        exit 1
    else
        for md in $(ls $dir./$BKDIR); do
            find $dir./ -type d -name $BKDIR -prune -o -name $md -type f -exec echo {} \;
        done
    fi
}

swap_bk () {
    for md in $(find_same_md); do
        m=$(basename $md)
        m=${m%.*}
        m+=".bk.md"
        cp $md $dir./$BKDIR/$m
        
        spec=`find $dir./$BKDIR -type f -name $(basename $md)`
        if [ -n "$spec" ]; then
            cp $dir./$BKDIR/$(basename $md) $md
 			mv $dir./$BKDIR/$m $dir./$BKDIR/$(basename $md)
        else
            echo "Same name file not found"
        fi
    done
}

swap_bk
