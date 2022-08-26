#!/usr/bin/env bash

cd $(dirname $0)

DOCS_DIR=$(pwd)/..
PROJECT_DIR=${DOCS_DIR}/..

# run doxygen
cd ${DOCS_DIR}/doxygen
DOXYGEN_DIR=$(pwd)
rm -rf generated
mkdir -p generated
doxygen Doxyfile.in

# generate rst file
cd "${DOCS_DIR}/sphinx"
# chmod +x generateRstFile.sh
# bash generateRstFile.sh || exit 1

# copy README.md
# cp -rf ${PROJECT_DIR}/README.md ${DOCS_DIR}/sphinx/README.md
# cp -rf ${PROJECT_DIR}/samples ${DOCS_DIR}/sphinx/samples

# run sphinx
SPHINX_SOURCE_DIR=$(pwd)
SPHINX_BUILD_DIR=${SPHINX_SOURCE_DIR}/generated

rm -rf ${SPHINX_BUILD_DIR}
mkdir -p ${SPHINX_BUILD_DIR}

if [[ "$(uname -s)" == 'Darwin' ]] && ! [ -x "$(command -v sphinx-build)" ]; then
	echo "sphinx-build is not installed. run "brew install sphinx-build" manully"
	if ! brew install sphinx-build; then
		echo "installing sphinx-build failed! please try it your self!"
		exit 1
	fi
fi

pip install sphinx-rtd-theme breathe myst_parser sphinx_rtd_size sphinx_markdown_tables
sphinx-build -b html -D breathe_projects.graphicRenderEngine=${DOXYGEN_DIR}/generated/xml ${SPHINX_SOURCE_DIR} ${SPHINX_BUILD_DIR}
# rm -rf ${DOCS_DIR}/sphinx/README.md
# rm -rf ${DOCS_DIR}/sphinx/samples
echo "Generating documentation with Sphinx!"