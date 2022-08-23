#!/usr/bin/env bash

cd $(dirname $0)

THIS_DIR=$(pwd)
DOCS_DIR=$(pwd)/..
PROJECT_DIR=${DOCS_DIR}/..
SOURCES_DIR=${PROJECT_DIR}/ceres
echo ${SOURCES_DIR}

# generate source
function generatedRst()
{
#1st param, the dir name
for file in `ls $1`;
do
    if [ -d "$1/$file" ]; then
        cd "$1/$file"
        echo -e ".. ${file}-api:\n\n==================\n${file}\n==================\n\n.. toctree::\n   :maxdepth: 5\n" >index.rst
		for dir in $(ls -F | grep "/"); do
			echo -e "   ${dir%?}/index" >>index.rst
		done
        for header in $(ls | grep -E "\.h"); do
			name=${header%.*}
			fileName=${name}.rst
			mv "$header" ${fileName}
			echo  -e ".. ${name}-h:\n\n============\n${name}.h\n============\n\n.. doxygenfile:: ${name}.h" >${fileName}
			echo -e "   ${name}" >>index.rst
		done
        generatedRst "$1/$file"
    fi
done
}

# rm source
cd ${THIS_DIR}
rm -rf ${THIS_DIR}/source
cp -rf ${SOURCES_DIR} ${THIS_DIR}

cd ceres 
rm -rf 3rdparty
rm -rf shaders
rm -rf tools
# 删除所有的cpp文件
rm -rf $(find . -name "*.cpp" -o -name "*.mm" -o -name "*.txt" -o -name "*.inl")
generatedRst $(pwd)