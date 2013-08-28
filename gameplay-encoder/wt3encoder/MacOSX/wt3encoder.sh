#!/bin/sh

#save current path
export ORIG_PATH=$PWD
export EXEC_PATH=`sh readlink.sh $(dirname $0)`

# sh Wikitude3dEncoder.sh <ABSOLUTE_MODELPATH> <OUTPUT_NAME> <OUTPUT_PATH>
# example: 
# sh Wikitude3dEncoder.sh ./Documents/butterfly/butterfly.fbx ./Documents/out/butterfly.wt3
export ROOT_MODEL_PATH=`sh readlink.sh $(dirname $1)`
export MODEL_FILENAME=`basename $1`
export MODEL_FILEPATH="$ROOT_MODEL_PATH/$MODEL_FILENAME"
export OUT_FILENAME=`basename $2`
export OUT_PATH=`sh readlink.sh $(dirname $2)`
export OUT_FILEPATH="$OUT_PATH/$OUT_FILENAME"

# create missing directories
if [ ! -d "$ROOT_MODEL_PATH/model" ]; then
	mkdir -p "$ROOT_MODEL_PATH/model"
fi

if [ ! -d "$ROOT_MODEL_PATH/model/tex" ]; then
	mkdir -p "$ROOT_MODEL_PATH/model/tex"
fi

# encode 3d-model
"$EXEC_PATH/encoder" -gy -tex "$ROOT_MODEL_PATH/model/tex" -m "$ROOT_MODEL_PATH/model/model.material" -scene "$ROOT_MODEL_PATH/model/model.scene" "$MODEL_FILEPATH" "$ROOT_MODEL_PATH/model/model.gpb"
cp "$EXEC_PATH/version" "$ROOT_MODEL_PATH/model"

if [ ! -d "$OUT_PATH" ]; then
	mkdir -p "$OUT_PATH"
fi

# create .wt3
cd "$ROOT_MODEL_PATH"
zip -r "./$OUT_FILENAME" "./model"
rm -Rf ./model
mv "$OUT_FILENAME" "$OUT_FILEPATH"

cd "$ORIG_PATH"
echo "**** encoding finished"