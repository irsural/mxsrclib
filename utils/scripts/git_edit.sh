#!/bin/bash
#./utils/scripts/git_edit.sh

# Переименовывание меток (tag)
tag_name=1352
git tag ${tag_name}_calibrator ${tag_name}b
git tag -d ${tag_name}b
git push origin :refs/tags/${tag_name}b

