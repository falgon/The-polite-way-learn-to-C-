#!/bin/bash

openssl aes-256-cbc -K $encrypted_0de92ec5cff5_key -iv $encrypted_0de92ec5cff5_iv -in $TRAVIS_BUILD_DIR/.travis/travis_key.enc -out ~/.ssh/id_rsa -d
chmod 600 ~/.ssh/id_rsa
echo -e "Host github.com\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config
git config --global user.email "falgon53@yahoo.co.jp"
git config --global user.name "falgon"
git config --global core.autocrlf "input"
