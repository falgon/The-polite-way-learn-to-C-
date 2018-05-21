#!/bin/bash -xu

PLANTUML=plantuml
UMLPATH=./src

if [ $# -eq 1 ]; then
    if [ "clear" = $1 ]; then
        rm -rf dist
    else
        echo "Invalid argument"
    fi
else
    mkdir -p dist
    ${PLANTUML} `ls ${UMLPATH}/*.uml`
    mv ${UMLPATH}/*.png ./dist
fi
