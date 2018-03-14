#!/bin/bash -xev
type module >& /dev/null \
|| . /mnt/software/Modules/current/init/bash
module load gcc
module load boost
module load swig
module load ccache

if [[ $bamboo_repository_branch_name =~ -DEP-438- ]]; then
  export BOOST_ROOT=/mnt/software/b/boost/1.60
fi
if [[ $USER == "bamboo" ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.mobs.ccachedir
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'
export CCACHE_BASEDIR=$PWD
export PYTHONUSERBASE=$PWD/_deployment
export PATH=/mnt/software/a/anaconda2/4.2.0/bin:$PATH

make clean
rm -rf _deployment
mkdir -p _deployment

python setup.py install --user --boost=$BOOST_ROOT
