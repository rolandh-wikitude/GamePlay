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
export LOG=$( (`"$EXEC_PATH/encoder" -gy -tex "$ROOT_MODEL_PATH/model/tex" -m "$ROOT_MODEL_PATH/model/model.material" -scene "$ROOT_MODEL_PATH/model/model.scene" "$MODEL_FILEPATH" "$ROOT_MODEL_PATH/model/model.gpb"`) 2>&1)

cp "$EXEC_PATH/version" "$ROOT_MODEL_PATH/model"

if grep -q Error <<< $LOG; then
	cd "$ROOT_MODEL_PATH"
	rm -Rf ./model
	cd "$ORIG_PATH"
	echo "$LOG"
	exit 1
fi


if [ ! -d "$OUT_PATH" ]; then
	mkdir -p "$OUT_PATH"
fi

# create .wt3
cd "$ROOT_MODEL_PATH"
zip -q -r "./$OUT_FILENAME" "./model"
rm -Rf ./model
mv "$OUT_FILENAME" "$OUT_FILEPATH"

cd "$ORIG_PATH"
exit 0