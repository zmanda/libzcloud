#!/usr/bin/env bash
#  -*- Mode: makefile; tab-width: 4; indent-tabs-mode: nil -*- */
#  vi: set tabstop=4 shiftwidth=4 expandtab: */
# ***** BEGIN LICENSE BLOCK *****
# Copyright (C) 2009 Zmanda Incorporated. All Rights Reserved.
#
# This file is part of libzcloud.
#
# libzcloud is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License (the LGPL)
# as published by the Free Software Foundation, either version 2.1 of
# the LGPL, or (at your option) any later version.
#
# libzcloud is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#  ***** END LICENSE BLOCK *****



function die {
    echo $@
    exit 1
}

test -n "$GH_PAGES_REPO" || die "must set GH_PAGES_REPO"
if test ! -d build/git; then
    git clone -n "$GH_PAGES_REPO" build/git || die "clone failed"
    cd build/git || die "cd failed"
    git checkout -b gh-pages origin/gh-pages || die "checkout failed (does the gh-pages branch exist for this repo?)"
else
    cd build/git || die "cd failed"
fi
git rm -r . || die "git rm failed"
git clean -fdx || die "git clean failed"
cp -r ../html/* . || die "copy failed"
find * -type f -exec sed -i -e 's!_static/!static/!g' -e 's!_sources/!sources/!g' \{} \; || die "munging failed"
test -d sources && die "sources/ already exists"
mv _sources sources || die "rename failed (sources)"
test -d static && die "sources/ already exists"
mv _static static || die "rename failed (static)"
git add . || die "git add failed"
git commit -m "update docs" || die "commit failed"
git push origin gh-pages || die "push failed"
